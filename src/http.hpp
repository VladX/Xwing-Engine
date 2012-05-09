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

#ifndef __HTTP_H_
#define __HTTP_H_ 1

#include "common.hpp"
#include <uv.h>

namespace protocol_HTTP
{

enum http_method { HTTP_GET, HTTP_POST, HTTP_UNSUPPORTED };

class HttpHeader
{
public:
	static_string key;
	static_string value;
	
	HttpHeader () {};
	HttpHeader (static_string & k, static_string & v) : key(k), value(v) {};
	
	static HttpHeader parse_header (char * &, size_t &);
};

bool parse_request_line (char * &, size_t &, enum http_method &, static_string &, uchar &);

};

#endif
