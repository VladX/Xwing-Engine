/*
 * This file is part of Xwing - Open video hosting engine
 *
 * Copyright (C) 2014 - Xpast; http://xpast.me/; <vvladxx@gmail.com>
 *
 * Xwing is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xwing is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xwing.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common.hpp"
#include "fcgi.hpp"

#define FCGI_VERSION_1           1

#define FCGI_BEGIN_REQUEST       1
#define FCGI_ABORT_REQUEST       2
#define FCGI_END_REQUEST         3
#define FCGI_PARAMS              4
#define FCGI_STDIN               5
#define FCGI_STDOUT              6
#define FCGI_STDERR              7
#define FCGI_DATA                8
#define FCGI_GET_VALUES          9
#define FCGI_GET_VALUES_RESULT  10
#define FCGI_UNKNOWN_TYPE       11
#define FCGI_MAXTYPE (FCGI_UNKNOWN_TYPE)

typedef struct {
	unsigned char roleB1;
	unsigned char roleB0;
	unsigned char flags;
	unsigned char reserved[5];
} FCGI_BeginRequestBody;

size_t fcgi_find_ending (const char * data, size_t length) {
	FCGI_Header expect = {{{FCGI_VERSION_1, FCGI_PARAMS, 0, 0, 0, 0, 0, 0}}};
	FCGI_Header mask = {{{0xFF, 0xFE, 0, 0, 0xFF, 0xFF, 0, 0xFF}}};
	size_t len = length + 1 - sizeof(FCGI_Header);
	for (size_t i = 0; i < len; ++i)
		if (unlikely(((* ((uint64_t *) (data + i))) & mask.word) == expect.word))
			return i;
	return length;
}

void fcgi_process (transfer_t * transfer, const char * data, size_t length) {
	const char * end = data + length;
	DEBUG_PRINT_3(length);
	for (FCGI_Header * hdr = (FCGI_Header *) data; (const char *) hdr < end; hdr = (FCGI_Header *) data) {
		DEBUG_PRINT_3("RID=" << hdr->getRequestId() << "; content length=" << hdr->getContentLength() << "; type=" << (int) hdr->type);
		data += sizeof(FCGI_Header);
		uint16_t rid = hdr->getRequestId();
		switch (hdr->type) {
			case FCGI_BEGIN_REQUEST:
				transfer->requests.resize(rid);
				transfer->requests[--rid].closeAfterFinish = ((FCGI_BeginRequestBody *) data)->flags == 0;
				DEBUG_PRINT_3("Keep connection=" << transfer->requests[rid].closeAfterFinish);
				break;
			case FCGI_PARAMS:
				for (size_t i = 0, len = hdr->getContentLength(); i < len;) {
					string name(data + i + 2, data[i]), value(data + i + 2 + data[i], data[i + 1]);
					DEBUG_PRINT_3(name << ':' << value);
					i += 2UL + data[i] + data[i + 1];
				}
				break;
			default:
				break;
		}
		data += hdr->getFullLength();
	}
}
