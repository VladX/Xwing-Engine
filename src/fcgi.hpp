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

#ifndef __FCGI_H_
#define __FCGI_H_ 1

#include "server.hpp"

#define FCGI_VERSION_1 1

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

#define FCGI_REQUEST_COMPLETE 0
#define FCGI_OVERLOADED       2

struct FCGI_Header {
	union {
		struct {
			uint8_t version;
			uint8_t type;
			uint8_t requestIdB1;
			uint8_t requestIdB0;
			uint8_t contentLengthB1;
			uint8_t contentLengthB0;
			uint8_t paddingLength;
			uint8_t reserved;
		};
		uint64_t word;
	};
	
	inline uint16_t getContentLength () const {
		return bswap_16_be(* ((uint16_t *) &contentLengthB1));
	}
	
	inline unsigned int getFullLength () const {
		return (unsigned int) getContentLength() + (unsigned int) paddingLength;
	}
	
	inline uint16_t getRequestId () const {
		return bswap_16_be(* ((uint16_t *) &requestIdB1));
	}
	
	inline FCGI_Header () {};
	
	inline FCGI_Header (uint8_t version, uint8_t type, uint8_t requestIdB1, uint8_t requestIdB0, uint8_t contentLengthB1,
						uint8_t contentLengthB0, uint8_t paddingLength, uint8_t reserved) :
						version(version), type(type), requestIdB1(requestIdB1), requestIdB0(requestIdB0), contentLengthB1(contentLengthB1),
						contentLengthB0(contentLengthB0), paddingLength(paddingLength), reserved(reserved) {};
	
	inline FCGI_Header (uint8_t type, uint16_t rid, uint16_t contentLength) : version(FCGI_VERSION_1), type(type), paddingLength(0) {
		* ((uint16_t *) &requestIdB1) = bswap_16_be(rid);
		* ((uint16_t *) &contentLengthB1) = bswap_16_be(contentLength);
	};
};

void fcgi_process_record (transfer_t *, FCGI_Header *, const char *, size_t);

#endif
