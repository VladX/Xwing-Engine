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

struct write_t {
	uv_write_t wr;
	DynamicArray<chunk_t *> usedChunks;
};

static_assert(offsetof(write_t, wr) == 0, "");

static uv_loop_t * loop;
static uv_timer_t timer_scavenger;
static StaticMemoryPool<transfer_t, SERVER_PREALLOC_TRANSFER_T> server_pool;
static StaticMemoryPool<write_t, SERVER_PREALLOC_WRITE_T> write_pool;
static StaticMemoryPool<uv_shutdown_t, SERVER_PREALLOC_WRITE_T> shutdown_pool;
static StaticMemoryPool<chunk_t, SERVER_PREALLOC_CHUNKS> chunk_pool;

transfer_t::transfer_t () {
	uv_tcp_t * stream = &this->handle;
	memset(stream, 0, sizeof(* stream));
	stream->data = this;
	//padding = 0;
	recordBytes = 0;
}

transfer_t::~transfer_t () {
	for (size_t i = 0; i < preservedChunks.size(); ++i)
		chunk_pool.deallocate(preservedChunks[i].chunk);
}

static void on_close (uv_handle_t * handle) {
	DEBUG_PRINT_3("Closing transfer buffer: " << handle->data);
	server_pool.deallocate(reinterpret_cast<transfer_t *>(handle->data));
}

static void on_shutdown (uv_shutdown_t * req, int status) {
	DEBUG_PRINT_3("Shutdown with status " << status);
	if (status) {
		// FIXME: Properly handle failure
		LOG_ERROR("shutdown: " << uv_strerror(status));
	}
	shutdown_pool.deallocate(req);
	uv_close(reinterpret_cast<uv_handle_t *>(req->handle), on_close);
}

static void after_write (uv_write_t * wr, int status) {
	DEBUG_PRINT_3("Writing completed with status " << status);
	if (status) {
		// FIXME: Properly handle failure
		LOG_ERROR("write: " << uv_strerror(status));
	}
	auto usedChunks = &reinterpret_cast<write_t *>(wr)->usedChunks;
	for (size_t i = 0; i < usedChunks->size(); ++i)
		chunk_pool.deallocate(usedChunks->operator[](i));
	write_pool.deallocate(reinterpret_cast<write_t *>(wr));
}

/* FIXME: What will happen if handler writes too fast? This call should block probably, or fail with error... */
void request_t::write (const char * data, size_t len) {
	if (unlikely(writeBuffer == nullptr)) {
		writeBuffer = chunk_pool.allocate();
		static_assert(sizeof(writeBuffer->body) <= 0xFFFF, "SERVER_CHUNK_SIZE was set to wrong value");
		writeBufferRemaining = sizeof(writeBuffer->body);
	}
	if (likely(len < writeBufferRemaining)) {
		memcpy(writeBuffer->buf + sizeof(chunk_t) - writeBufferRemaining, data, len);
		writeBufferRemaining -= len;
		return;
	}
	memcpy(writeBuffer->buf + sizeof(chunk_t) - writeBufferRemaining, data, writeBufferRemaining);
	data += writeBufferRemaining;
	len -= writeBufferRemaining;
	* ((FCGI_Header *) writeBuffer->hdr) = FCGI_Header(FCGI_STDOUT, rid, sizeof(writeBuffer->body));
	write_t * wr = write_pool.allocate();
	wr->usedChunks.push_back(writeBuffer);
	size_t bufsz = 1 + len / sizeof(writeBuffer->body);
	uv_buf_t bufs[bufsz];
	bufs[0] = {.base = writeBuffer->buf, .len = sizeof(chunk_t)};
	for (uint i = 1; len >= sizeof(writeBuffer->body); ++i, data += sizeof(writeBuffer->body), len -= sizeof(writeBuffer->body)) {
		bufs[i] = {.base = chunk_pool.allocate()->buf, .len = sizeof(chunk_t)};
		* ((FCGI_Header *) bufs[i].base) = FCGI_Header(FCGI_STDOUT, rid, sizeof(writeBuffer->body));
		memcpy(bufs[i].base + sizeof(writeBuffer->hdr), data, sizeof(writeBuffer->body));
		wr->usedChunks.push_back((chunk_t *) bufs[i].base);
	}
	if (likely(len)) {
		writeBuffer = chunk_pool.allocate();
		writeBufferRemaining = sizeof(writeBuffer->body) - len;
		memcpy(writeBuffer->body, data, len);
	}
	else {
		writeBuffer = nullptr;
	}
	uv_write(&wr->wr, reinterpret_cast<uv_stream_t *>(&transfer->handle), bufs, bufsz, after_write);
}

void request_t::flush () {
	write_t * wr = write_pool.allocate();
	if (closeAfterFinish) {
		uv_read_stop(reinterpret_cast<uv_stream_t *>(&transfer->handle));
		uv_shutdown_t * sht = shutdown_pool.allocate();
		uv_shutdown(sht, reinterpret_cast<uv_stream_t *>(&transfer->handle), on_shutdown);
	}
	//uv_tcp_nodelay(&transfer->handle, 1);
	if (unlikely(writeBuffer == nullptr)) {
		uv_buf_t buf[1] = {{.base = reinterpret_cast<char *>(hdrEndReq), .len = sizeof(hdrEndReq)}};
		uv_write(&wr->wr, reinterpret_cast<uv_stream_t *>(&transfer->handle), buf, 1, after_write);
		return;
	}
	* ((FCGI_Header *) writeBuffer->hdr) = FCGI_Header(FCGI_STDOUT, rid, sizeof(writeBuffer->body) - writeBufferRemaining);
	uv_buf_t buf[2] = {
		{.base = writeBuffer->buf, .len = sizeof(chunk_t) - writeBufferRemaining},
		{.base = reinterpret_cast<char *>(hdrEndReq), .len = sizeof(hdrEndReq)}
	};
	wr->usedChunks.push_back(writeBuffer);
	writeBuffer = nullptr;
	uv_write(&wr->wr, reinterpret_cast<uv_stream_t *>(&transfer->handle), buf, 2, after_write);
}

static void read_alloc (uv_handle_t *, size_t, uv_buf_t * buf) {
	try {
		* buf = {.base = chunk_pool.allocate()->buf, .len = sizeof(chunk_t)};
	}
	catch (bad_alloc) {
		* buf = {.base = 0, .len = 0};
	}
}

static void after_read (uv_stream_t * handle, ssize_t nread, const uv_buf_t * buf) {
	transfer_t * transfer = reinterpret_cast<transfer_t *>(handle->data);
	if (unlikely(nread < 0)) {
		if (unlikely(nread != UV_EOF))
			LOG_ERROR("read: " << uv_strerror(nread));
		uv_read_stop(handle);
		if (buf->base)
			chunk_pool.deallocate((chunk_t *) buf->base);
		uv_close(reinterpret_cast<uv_handle_t *>(handle), on_close);
		return;
	}
	DEBUG_PRINT_3("Reading " << nread << " bytes from transfer: " << (void *) transfer);
	for (const char * ptr = buf->base; likely(nread);) {
		static_assert(sizeof(FCGI_Header) == sizeof(transfer->recordHeader), "");
		if (unlikely(transfer->recordBytes < sizeof(FCGI_Header))) {
			if (unlikely(transfer->recordBytes + (size_t) nread < sizeof(FCGI_Header))) {
				memcpy(transfer->recordHeader + transfer->recordBytes, ptr, nread);
				transfer->recordBytes += nread;
				break;
			}
			memcpy(transfer->recordHeader + transfer->recordBytes, ptr, sizeof(FCGI_Header) - transfer->recordBytes);
			nread -= sizeof(FCGI_Header) - transfer->recordBytes;
			ptr += sizeof(FCGI_Header) - transfer->recordBytes;
			transfer->recordBytes = sizeof(FCGI_Header);
		}
		uint recordSize = ((FCGI_Header *) transfer->recordHeader)->getFullLength() + sizeof(FCGI_Header);
		transfer->recordBytes += nread;
		if (unlikely(transfer->recordBytes < recordSize)) {
			transfer->preservedChunks.push_back(transfer_t::PreservedChunk::make((chunk_t *) buf->base, ptr, nread));
			/* TODO */
			return;
		}
		for (size_t i = 0; unlikely(i < transfer->preservedChunks.size()); ++i) {
			fcgi_process_record(transfer, (FCGI_Header *) transfer->recordHeader, transfer->preservedChunks[i].data, transfer->preservedChunks[i].size);
			chunk_pool.deallocate(transfer->preservedChunks[i].chunk);
		}
		transfer->preservedChunks.shrink_to_fit(0);
		fcgi_process_record(transfer, (FCGI_Header *) transfer->recordHeader, ptr, ((FCGI_Header *) transfer->recordHeader)->getContentLength());
		ptr += nread - (transfer->recordBytes - recordSize);
		nread = transfer->recordBytes - recordSize;
		transfer->recordBytes = 0;
	}
	if (likely(buf->base))
		chunk_pool.deallocate((chunk_t *) buf->base);
}

static inline void on_connection (transfer_t * transfer, uv_stream_t * server, uv_stream_t * stream) {
	DEBUG_PRINT_2("New connection received, transfer buffer was allocated from a pool: " << reinterpret_cast<void *>(transfer));
	
	int ret = uv_accept(server, stream);
	if (ret) {
		LOG_ERROR("Accept: " << uv_strerror(ret));
		server_pool.deallocate(transfer);
		return;
	}
	
	ret = uv_read_start(stream, read_alloc, after_read);
	if (ret) {
		LOG_ERROR("uv_read_start: " << uv_strerror(ret));
		server_pool.deallocate(transfer);
		return;
	}
}

static void on_connection_tcp (uv_stream_t * server, int status) {
	if (unlikely(status)) {
		LOG_ERROR("Connect: " << uv_strerror(status));
		return;
	}
	
	transfer_t * transfer = server_pool.allocate();
	uv_tcp_t * stream = &transfer->handle;
	
	int ret = uv_tcp_init(loop, stream);
	if (unlikely(ret)) {
		LOG_ERROR("uv_tcp_init: " << uv_strerror(ret));
		server_pool.deallocate(transfer);
		return;
	}
	
	on_connection(transfer, server, reinterpret_cast<uv_stream_t *>(stream));
}

static void on_connection_pipe (uv_stream_t * server, int status) {
	if (unlikely(status)) {
		LOG_ERROR("Connect: " << uv_strerror(status));
		return;
	}
	
	transfer_t * transfer = server_pool.allocate();
	uv_pipe_t * stream = &transfer->phandle;
	
	int ret = uv_pipe_init(loop, stream, 0);
	if (unlikely(ret)) {
		LOG_ERROR("uv_pipe_init: " << uv_strerror(ret));
		server_pool.deallocate(transfer);
		return;
	}
	
	on_connection(transfer, server, reinterpret_cast<uv_stream_t *>(stream));
}

static void scavenger (uv_timer_t *) {
	DEBUG_PRINT_2("Performing cleaning operations");
	chunk_pool.free_unused();
	server_pool.free_unused();
	write_pool.free_unused();
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

static void tcp_setup () {
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
		ret = uv_listen((uv_stream_t *) &pipe, SOMAXCONN, on_connection_pipe);
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
		ret = uv_listen((uv_stream_t *) &tcpServer, SOMAXCONN, on_connection_tcp);
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
