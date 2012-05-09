/*
 * This file is part of Opentube - Open video hosting engine
 *
 * Copyright (C) 2011 - Xpast; http://xpast.me/; <vvladxx@gmail.com>
 *
 * Opentube is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Opentube is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Opentube.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SERVER_H_
#define __SERVER_H_ 1

#include "common.hpp"
#include "http.hpp"
#include <uv.h>

typedef struct
{
	//http_client_t http;
	uv_tcp_t handle;
	uv_write_t wr;
	uv_shutdown_t req;
	char * data;
	size_t data_size;
	size_t data_real_size;
	static_string uri;
	unordered_map<static_string, protocol_HTTP::HttpHeader *> headers;
	enum protocol_HTTP::http_method http_method;
	uchar http_version;
} client_t;

void server_init ();

namespace protocol_HTTP { void http_handler (client_t *, size_t, uv_buf_t); };

#endif
