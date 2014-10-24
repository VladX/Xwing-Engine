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

#include "common.hpp"
#include "allocator.hpp"

extern "C" {
void * malloc(size_t);
void * calloc(size_t, size_t);
void * realloc(void *, size_t);
void free(void *);
};

namespace xwing {

void * (* alloc_fn)(size_t) = malloc;
void * (* realloc_fn)(void *, size_t) = realloc;
void * (* calloc_fn)(size_t, size_t) = calloc;
void (* free_fn)(void *) = free;

GlobalAllocator<char> allocator;

};

void * operator new(size_t size) {
	return xwing::allocator.allocate(size);
}

void * operator new(size_t size, const std::nothrow_t &) throw() {
	return xwing::alloc_fn(size);
}

void * operator new[](size_t size) {
	return xwing::allocator.allocate(size);
}

void * operator new[](size_t size, const std::nothrow_t &) throw() {
	return xwing::alloc_fn(size);
}

void operator delete(void * ptr) throw() {
	xwing::allocator.deallocate(reinterpret_cast<char *>(ptr));
}

void operator delete(void * ptr, const std::nothrow_t &) throw() {
	xwing::allocator.deallocate(reinterpret_cast<char *>(ptr));
}

void operator delete[](void * ptr) throw() {
	xwing::allocator.deallocate(reinterpret_cast<char *>(ptr));
}

void operator delete[](void * ptr, const std::nothrow_t &) throw() {
	xwing::allocator.deallocate(reinterpret_cast<char *>(ptr));
}
