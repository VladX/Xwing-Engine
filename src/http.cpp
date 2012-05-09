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

#include "http.hpp"
#include "server.hpp"

namespace protocol_HTTP
{

StaticMemoryPool<HttpHeader, SERVER_PREALLOC_HTTP_HEADER> headers_pool;

bool parse_request_line (char * & data, size_t & len, enum http_method & method_out, static_string & uri_out, uchar & version_out)
{
	if (len < 14)
		return false;
	size_t i, l = len;
	enum http_method method = HTTP_UNSUPPORTED;
	char * p = data;
	if (p[0] == 'G' && p[3] == ' ')
	{
		method = HTTP_GET;
		p += 4;
		l -= 4;
	}
	else if (p[0] == 'P' && p[4] == ' ')
	{
		method = HTTP_POST;
		p += 5;
		l -= 5;
	}
	else
	{
		for (i = 0; i < l; i++)
			if (p[i] == ' ')
				break;
		if (i > l - 4)
			return false;
		p += i + 1;
		l -= i + 1;
	}
	for (i = 0; i < l; i++)
		if (p[i] == ' ')
			break;
	if (i > l - 11)
			return false;
	if (p[i + 9] != '\r' || p[i + 10] != '\n')
		return false;
	static_string uri(p, i);
	p[i] = '\0';
	method_out = method;
	uri_out = uri;
	version_out = (p[i + 6] - '0') * 10 + (p[i + 8] - '0');
	len = l - (i + 11);
	data = p + i + 11;
	return true;
}

HttpHeader HttpHeader::parse_header (char * & data, size_t & len)
{
	size_t i;
	HttpHeader empty;
	for (i = 0; i < len; i++)
		if (data[i] == ':')
			break;
		else
			data[i] = tolower(data[i]);
	if (i == len)
		return empty;
	data[i] = '\0';
	static_string key(data, i);
	++i;
	while (i < len && data[i] == ' ')
		++i;
	if (i == len)
		return empty;
	char * val = data + i;
	for (; i < len; ++i)
		if (data[i] == '\n' && data[i - 1] == '\r')
			break;
	if (i == len)
		return empty;
	data[i - 1] = '\0';
	static_string value(val, (i - (val - data)) - 1);
	HttpHeader hdr(key, value);
	data += i + 1;
	len -= i + 1;
	return hdr;
}

void http_handler (client_t * client, size_t nread, uv_buf_t buf)
{
	for (size_t i = 3; i < (size_t) nread; i++)
		if (buf.base[i] == '\n' && buf.base[i - 3] == '\r' && buf.base[i - 2] == '\n' && buf.base[i - 1] == '\r')
		{
			// headers ready
			char * data = client->data;
			size_t data_size = client->data_size;
			if (!parse_request_line(data, data_size, client->http_method, client->uri, client->http_version))
			{
				// error
			}
			DEBUG_PRINT_2("Request: uri=\"" << client->uri.c_str() << "\" method=" << client->http_method << " version=" << (uint) client->http_version);
			for (;;)
			{
				HttpHeader hdr = HttpHeader::parse_header(data, data_size);
				if (hdr.key.length() == 0)
					break;
				HttpHeader * h = headers_pool.allocate();
				* h = hdr;
				client->headers[h->key] = h;
				DEBUG_PRINT_3(h->key.c_str() << ": " << h->value.c_str());
			}
			break;
		}
}

static void cleanup (client_t * client)
{
	auto it = client->headers.begin();
	while (it != client->headers.end())
	{
		headers_pool.deallocate(it->second);
		it++;
	}
	client->headers.~unordered_map();
}

};
