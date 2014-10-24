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

#include "server.hpp"
#include "js.hpp"
#include "fcgi.hpp"

typedef struct {
	unsigned char buf[SERVER_SMALL_CHUNK_SIZE];
} small_chunk_t;

typedef struct {
	char buf[SERVER_LARGE_CHUNK_SIZE];
} large_chunk_t;

static uv_loop_t * loop;
static StaticMemoryPool<transfer_t, SERVER_PREALLOC_TRANSFER_T> server_pool;
static StaticMemoryPool<small_chunk_t, SERVER_PREALLOC_TRANSFER_T> small_pool;
static StaticMemoryPool<large_chunk_t, SERVER_PREALLOC_LARGE_CHUNKS> large_pool;

static void read_alloc (uv_handle_t *, size_t, uv_buf_t * buf) {
	try {
		buf->base = (char *) small_pool.allocate(); // TODO: Выделять из большого пула, если заголовки прочитаны
		buf->len = sizeof(small_chunk_t);
	}
	catch (bad_alloc) {
		buf->base = 0;
		buf->len = 0;
	}
}

static void after_read (uv_stream_t * handle, ssize_t nread, const uv_buf_t * buf) {
	transfer_t * transfer = reinterpret_cast<transfer_t *>(handle->data);
	if (unlikely(nread < 0)) {
		if (unlikely(nread != UV_EOF))
			LOG_ERROR("read: " << uv_strerror(nread));
		
		DEBUG_PRINT_3("Deallocating all transfer buffers: " << (void *) transfer);
		uv_read_stop(handle);
		if (buf->base)
			small_pool.deallocate((small_chunk_t *) buf->base);
		server_pool.deallocate(transfer);
		return;
	}
	DEBUG_PRINT_3("Reading " << nread << " bytes from transfer: " << (void *) transfer);
	if (likely(nread))
		fcgi_process(transfer, buf->base, nread);
	if (likely(buf->base))
		small_pool.deallocate((small_chunk_t *) buf->base);
}

static void on_connection (uv_stream_t * server, int status) {
	if (unlikely(status)) {
		LOG_ERROR("Connect: " << uv_strerror(status));
		return;
	}
	
	transfer_t * transfer = server_pool.allocate();
	uv_tcp_t * stream = &transfer->handle;
	memset(transfer, 0, sizeof(* transfer));
	new(&transfer->requests) vector<void *>;
	stream->data = transfer;
	
	DEBUG_PRINT_2("New connection received, transfer buffer was allocated from a pool: " << reinterpret_cast<void *>(transfer));
	
	int ret = uv_tcp_init(loop, stream);
	if (unlikely(ret)) {
		LOG_ERROR("uv_tcp_init: " << uv_strerror(ret));
		server_pool.deallocate(transfer);
		return;
	}
	
	ret = uv_accept(server, reinterpret_cast<uv_stream_t *>(stream));
	if (ret) {
		LOG_ERROR("Accept: " << uv_strerror(ret));
		server_pool.deallocate(transfer);
		return;
	}
	
	ret = uv_read_start(reinterpret_cast<uv_stream_t *>(stream), read_alloc, after_read);
	if (ret) {
		LOG_ERROR("uv_read_start: " << uv_strerror(ret));
		server_pool.deallocate(transfer);
		return;
	}
}

static bool is_valid_ipv4_addr (const char * addr) {
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

void tcp_setup () {
	int ret;
	if (xwing::config.port == 0) {
		static uv_pipe_t pipe;
		ret = uv_pipe_init(loop, &pipe, 0);
		if (ret)
			LOG_CRITICAL("uv_pipe_init: " << uv_strerror(ret));
#ifdef WIN32
		ret = uv_pipe_bind(&pipe, string("\\\\.\\pipe\\" + xwing::config.host).c_str());
#else
		unlink(xwing::config.host.c_str());
		ret = uv_pipe_bind(&pipe, xwing::config.host.c_str());
#endif
		if (ret)
			LOG_CRITICAL("bind(" << xwing::config.host << "): " << uv_strerror(ret));
		chmod(xwing::config.host.c_str(), xwing::config.sockperm);
		ret = uv_listen((uv_stream_t *) &pipe, SOMAXCONN, on_connection);
		if (ret)
			LOG_CRITICAL("listen: " << uv_strerror(ret));
	}
	else {
		const struct sockaddr * addrptr;
		static uv_tcp_t tcpServer;
		ret = uv_tcp_init(loop, &tcpServer);
		if (ret)
			LOG_CRITICAL("uv_tcp_init: " << uv_strerror(ret));
		if (is_valid_ipv4_addr(xwing::config.host.c_str())) {
			struct sockaddr_in addr;
			ret = uv_ip4_addr(xwing::config.host.c_str(), xwing::config.port, &addr);
			if (ret)
				LOG_CRITICAL(uv_strerror(ret));
			addrptr = (const struct sockaddr *) &addr;
		}
		else {
			struct sockaddr_in6 addr;
			ret = uv_ip6_addr(xwing::config.host.c_str(), xwing::config.port, &addr);
			if (ret)
				LOG_CRITICAL(uv_strerror(ret));
			addrptr = (const struct sockaddr *) &addr;
		}
		ret = uv_tcp_bind(&tcpServer, addrptr, 0);
		if (ret)
			LOG_CRITICAL("bind(" << xwing::config.host << ":" << xwing::config.port << "): " << uv_strerror(ret));
		ret = uv_listen((uv_stream_t *) &tcpServer, SOMAXCONN, on_connection);
		if (ret)
			LOG_CRITICAL("listen(" << xwing::config.host << ":" << xwing::config.port << "): " << uv_strerror(ret));
	}
}

void server_init () {
	JS::MainInstance::init();
	loop = uv_default_loop();
	tcp_setup();
	uv_run(loop, UV_RUN_DEFAULT);
	uv_loop_delete(loop);
}
