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

static uv_loop_t * loop;
static uv_timer_t timer_scavenger;
static StaticMemoryPool<transfer_t, SERVER_PREALLOC_TRANSFER_T> server_pool;
static StaticMemoryPool<chunk_t, SERVER_PREALLOC_CHUNKS> chunk_pool;

static void read_alloc (uv_handle_t *, size_t, uv_buf_t * buf) {
	try {
		buf->base = (char *) chunk_pool.allocate();
		buf->len = sizeof(chunk_t);
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
			chunk_pool.deallocate((chunk_t *) buf->base);
		server_pool.deallocate(transfer);
		return;
	}
	DEBUG_PRINT_3("Reading " << nread << " bytes from transfer: " << (void *) transfer);
	if (likely(nread)) {
		size_t len = nread, pos;
		char * ptr = buf->base;
		if (likely(transfer->padding < len))
			len -= transfer->padding, ptr += transfer->padding;
		else {
			transfer->padding -= len;
			len = 0;
		}
		while (len > 7 && (pos = fcgi_find_ending(ptr, len)) != len) {
			if (unlikely(transfer->preservedChunks.size())) {
				for (size_t i = 0; i < transfer->preservedChunks.size(); ++i) {
					fcgi_process(transfer, get<1>(transfer->preservedChunks[i]), get<2>(transfer->preservedChunks[i]));
					chunk_pool.deallocate(get<0>(transfer->preservedChunks[i]));
				}
				transfer->preservedChunks.resize(0);
			}
			fcgi_process(transfer, ptr, pos + sizeof(FCGI_Header));
			auto hdrEndPos = pos + ((FCGI_Header *) (ptr + pos))->paddingLength + sizeof(FCGI_Header);
			if (unlikely(hdrEndPos > len)) {
				transfer->padding = (uchar) (hdrEndPos - len);
				break;
			}
			else {
				ptr += hdrEndPos;
				len -= hdrEndPos;
			}
		}
		if (unlikely(len)) {
			transfer->preservedChunks.push_back(make_tuple((chunk_t *) buf->base, ptr, len));
			return;
		}
	}
	if (likely(buf->base))
		chunk_pool.deallocate((chunk_t *) buf->base);
}

static void on_connection (uv_stream_t * server, int status) {
	if (unlikely(status)) {
		LOG_ERROR("Connect: " << uv_strerror(status));
		return;
	}
	
	transfer_t * transfer = server_pool.allocate();
	uv_tcp_t * stream = &transfer->handle;
	memset(stream, 0, sizeof(* stream));
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

static void scavenger (uv_timer_t *) {
	DEBUG_PRINT_2("Performing cleaning operations");
	chunk_pool.free_unused();
	server_pool.free_unused();
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
	//JS::MainInstance::init();
	loop = uv_default_loop();
	tcp_setup();
	uv_timer_init(loop, &timer_scavenger);
	uv_unref((uv_handle_t *) &timer_scavenger);
	uv_timer_start(&timer_scavenger, scavenger, SERVER_SCAVENGER_TIMER, SERVER_SCAVENGER_TIMER);
	uv_run(loop, UV_RUN_DEFAULT);
	uv_loop_close(loop);
}
