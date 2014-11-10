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

typedef struct {
	char buf[SERVER_CHUNK_SIZE];
} chunk_t;

typedef struct {
	bool closeAfterFinish;
} request_t;

typedef struct {
	uv_tcp_t handle;
	DynamicArray<request_t> requests;
	/*unsigned char partialHeader[8];
	uint16_t currentRequestId;
	uint16_t bytesRemaining;
	unsigned char padBytesRemaining;
	unsigned char partialHeaderLength;
	unsigned char type;
	bool markForClose;*/
	DynamicArray< tuple<chunk_t *, char *, size_t> > preservedChunks;
	uchar padding;
} transfer_t;

void server_init ();

#endif
