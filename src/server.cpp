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

#include "server.hpp"
using namespace protocol_HTTP;

static uv_loop_t * loop;
static uv_tcp_t tcpServer;
static StaticMemoryPool<client_t, SERVER_PREALLOC_CLIENT_T> server_pool;

static uv_buf_t read_alloc (uv_handle_t * handle, size_t suggested_size)
{
	client_t * client = reinterpret_cast<client_t *>(handle->data);
	if (unlikely((client->data_real_size - client->data_size) < suggested_size))
	{
		client->data_real_size = client->data_size + suggested_size;
		client->data = opentube::allocator.reallocate(client->data, client->data_real_size);
	}
	return uv_buf_init(client->data + client->data_size, suggested_size);
}

static void after_shutdown (uv_shutdown_t * req, int status)
{
	
}

static void after_read (uv_stream_t * handle, ssize_t nread, uv_buf_t buf)
{
	DEBUG_PRINT_3("read " << nread << " bytes");
	client_t * client = reinterpret_cast<client_t *>(handle->data);
	if (nread < 0)
	{
		if (uv_last_error(loop).code != UV_EOF)
			LOG_ERROR("read: " << uv_strerror(uv_last_error(loop)));
		
		opentube::allocator.deallocate(client->data);
		uv_shutdown(&client->req, handle, after_shutdown);
		return;
	}
	client->data_size += nread;
	assert(nread >= 0);
	http_handler(client, nread, buf);
}

static void on_connection (uv_stream_t * server, int status)
{
	if (unlikely(status))
	{
		LOG_ERROR("connect: " << uv_strerror(uv_last_error(loop)));
		return;
	}
	
	client_t * client = server_pool.allocate();
	uv_tcp_t * stream = &client->handle;
	
	if (unlikely(uv_tcp_init(loop, stream)))
	{
		LOG_ERROR("uv_tcp_init: " << uv_strerror(uv_last_error(loop)));
		server_pool.deallocate(client);
		return;
	}
	
	stream->data = client;
	client->data = 0;
	client->data_size = 0;
	client->data_real_size = 0;
	new(&client->headers) unordered_map<static_string, HttpHeader *>;
	
	if (uv_accept(server, reinterpret_cast<uv_stream_t *>(stream)))
	{
		LOG_ERROR("accept: " << uv_strerror(uv_last_error(loop)));
		server_pool.deallocate(client);
		return;
	}
	
	if (uv_read_start(reinterpret_cast<uv_stream_t *>(stream), read_alloc, after_read))
	{
		LOG_ERROR("uv_read_start: " << uv_strerror(uv_last_error(loop)));
		server_pool.deallocate(client);
		return;
	}
}

static bool is_valid_ipv4_addr (const char * addr)
{
	long n;
	char * a;
	n = strtol(addr, &a, 10);
	if (a == addr)
		return false;
	addr = a;
	if (!addr || * addr != '.' || n > 255 || n < 0)
		return false;
	addr++;
	n = strtol(addr, &a, 10);
	if (a == addr)
		return false;
	addr = a;
	if (!addr || * addr != '.' || n > 255 || n < 0)
		return false;
	addr++;
	n = strtol(addr, &a, 10);
	if (a == addr)
		return false;
	addr = a;
	if (!addr || * addr != '.' || n > 255 || n < 0)
		return false;
	addr++;
	n = strtol(addr, &a, 10);
	if (a == addr)
		return false;
	addr = a;
	if (n > 255 || n < 0)
		return false;
	return true;
}

void tcp_setup ()
{
	if (uv_tcp_init(loop, &tcpServer))
		LOG_CRITICAL("uv_tcp_init: " << uv_strerror(uv_last_error(loop)));
	
	if (is_valid_ipv4_addr(opentube::config.host.c_str()))
	{
		struct sockaddr_in addr4 = uv_ip4_addr(opentube::config.host.c_str(), opentube::config.port);
		if (uv_tcp_bind(&tcpServer, addr4))
			LOG_CRITICAL("bind: " << uv_strerror(uv_last_error(loop)));
	}
	else
	{
		struct sockaddr_in6 addr6 = uv_ip6_addr(opentube::config.host.c_str(), opentube::config.port);
		if (uv_tcp_bind6(&tcpServer, addr6))
			LOG_CRITICAL("bind: " << uv_strerror(uv_last_error(loop)));
	}
	
	if (uv_listen((uv_stream_t *) &tcpServer, SOMAXCONN, on_connection))
		LOG_CRITICAL("listen: " << uv_strerror(uv_last_error(loop)));
}

void server_init ()
{
	loop = uv_default_loop();
	tcp_setup();
	uv_run(loop);
}
