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

typedef struct {
	union {
		struct {
			unsigned char version;
			unsigned char type;
			unsigned char requestIdB1;
			unsigned char requestIdB0;
			unsigned char contentLengthB1;
			unsigned char contentLengthB0;
			unsigned char paddingLength;
			unsigned char reserved;
		};
		uint64_t word;
	};
	
	inline uint16_t getContentLength () const {
		return ((uint16_t) contentLengthB0) | (((uint16_t) contentLengthB1) << 8);
	}
	
	inline unsigned int getFullLength () const {
		return (unsigned int) getContentLength() + (unsigned int) paddingLength;
	}
	
	inline uint16_t getRequestId () const {
		return ((uint16_t) requestIdB0) | (((uint16_t) requestIdB1) << 8);
	}
} FCGI_Header;

size_t fcgi_find_ending (const char *, size_t);

void fcgi_process (transfer_t *, const char *, size_t);

#endif
