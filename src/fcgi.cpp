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

struct FCGI_BeginRequestBody {
	unsigned char roleB1;
	unsigned char roleB0;
	unsigned char flags;
	unsigned char reserved[5];
};

struct FCGI_EndRequestBody {
	unsigned char appStatusB3;
	unsigned char appStatusB2;
	unsigned char appStatusB1;
	unsigned char appStatusB0;
	unsigned char protocolStatus;
	unsigned char reserved[3];
};

struct FCGI_EndRequestRecord {
	FCGI_Header header;
	FCGI_EndRequestBody body;
};

void fcgi_process_record (transfer_t * transfer, FCGI_Header * hdr, const char * data, size_t length) {
	DEBUG_PRINT_3("RID=" << hdr->getRequestId() << "; content length=" << hdr->getContentLength() << "; type=" << (int) hdr->type);
	uint16_t rid = hdr->getRequestId();
	transfer->requests.extend_to_fit(rid);
	request_t * req = &transfer->requests[rid - 1];
	switch (hdr->type) {
		case FCGI_BEGIN_REQUEST:
			req->writeBuffer = nullptr;
			req->transfer = transfer;
			req->rid = rid;
			req->closeAfterFinish = ((FCGI_BeginRequestBody *) data)->flags == 0;
			* ((FCGI_Header *) req->hdrEndReq) = FCGI_Header(FCGI_STDOUT, rid, 0);
			* ((FCGI_EndRequestRecord *) &req->hdrEndReq[sizeof(FCGI_Header)]) = {FCGI_Header(FCGI_END_REQUEST, rid, sizeof(FCGI_EndRequestBody)), {0, 0, 0, 0, FCGI_REQUEST_COMPLETE, {0, 0, 0}}};
			DEBUG_PRINT_3("Keep connection=" << !req->closeAfterFinish);
			{
			DEBUG_PRINT_3("Writing");
			const char str[] = "Content-Type: text/plain\r\n\r\ntest";
			req->write(str, sizeof(str)-1);
			req->flush();
			}
			break;
		case FCGI_PARAMS:
			for (size_t i = 0, len = length; i < len;) {
				string name(data + i + 2, data[i]), value(data + i + 2 + data[i], data[i + 1]);
				DEBUG_PRINT_3(name << ':' << value);
				i += 2UL + data[i] + data[i + 1];
			}
			break;
		case FCGI_ABORT_REQUEST:
			DEBUG_PRINT_2("Request aborted");
			/* ... */
			break;
		default:
			break;
	}
}
