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

#ifndef __SERVER_H_
#define __SERVER_H_ 1

#include "common.hpp"
#include <uv.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/stat.h>
#endif

struct chunk_t {
	union {
		char buf[SERVER_CHUNK_SIZE];
		struct {
			char hdr[8];
			char body[SERVER_CHUNK_SIZE - sizeof(hdr)];
		};
	};
};

static_assert(sizeof(chunk_t) == SERVER_CHUNK_SIZE, ""); // just to make sure...

struct request_t {
	chunk_t * writeBuffer;
	struct transfer_t * transfer;
	uint8_t hdrEndReq[24];
	uint16_t writeBufferRemaining;
	uint16_t rid;
	bool closeAfterFinish;
	
	void write (const char *, size_t);
	void flush ();
};

struct transfer_t {
	union {
		uv_tcp_t handle;
		uv_pipe_t phandle;
	};
	DynamicArray<request_t> requests;
	typedef struct PreservedChunk {
		chunk_t * chunk;
		const char * data;
		size_t size;
		
		static inline PreservedChunk make (chunk_t * chunk, const char * data, size_t size) { PreservedChunk t = {chunk, data, size}; return t; };
	} PreservedChunk;
	DynamicArray<PreservedChunk> preservedChunks;
	//uchar padding;
	uint8_t recordHeader[8];
	uint recordBytes;
	
	transfer_t();
	~transfer_t();
};

void server_init ();

#endif
