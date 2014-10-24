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

#ifndef __ALLOCATOR_H_
#define __ALLOCATOR_H_ 1

#include "common.hpp"

namespace xwing {

extern void * (* alloc_fn)(size_t);
extern void * (* realloc_fn)(void *, size_t);
extern void * (* calloc_fn)(size_t, size_t);
extern void (* free_fn)(void *);

};

template<typename T>
class GlobalAllocator : public std::allocator<T> {
public:
	T * allocate(size_t);
	T * allocate(size_t size, const T *) { return allocate(size); };
	T * zallocate(size_t);
	T * reallocate(T *, size_t);
	void deallocate(T *);
	void deallocate(T * ptr, size_t) { deallocate(ptr); };
};

template<typename T>
inline T * GlobalAllocator<T>::allocate(size_t size) {
	T * ptr = reinterpret_cast<T *>(xwing::alloc_fn(sizeof(T) * size));
	if (likely(ptr))
		return ptr;
	if (size != 0)
		throw std::bad_alloc();
	return ptr;
}

template<typename T>
inline T * GlobalAllocator<T>::zallocate(size_t size) {
	T * ptr = reinterpret_cast<T *>(xwing::calloc_fn(sizeof(T), size));
	if (likely(ptr))
		return ptr;
	if (size != 0)
		throw std::bad_alloc();
	return ptr;
}

template<typename T>
inline T * GlobalAllocator<T>::reallocate(T * ptr, size_t new_size) {
	ptr = reinterpret_cast<T *>(xwing::realloc_fn(ptr, sizeof(T) * new_size));
	if (likely(ptr))
		return ptr;
	if (new_size != 0)
		throw std::bad_alloc();
	return ptr;
}

template<typename T>
inline void GlobalAllocator<T>::deallocate(T * ptr) {
	xwing::free_fn(ptr);
}

namespace xwing {

extern GlobalAllocator<char> allocator;

};

void * operator new(size_t size);
void * operator new(size_t size, const std::nothrow_t &) throw();
void * operator new[](size_t size);
void * operator new[](size_t size, const std::nothrow_t &) throw();
void operator delete(void * ptr) throw();
void operator delete(void * ptr, const std::nothrow_t &) throw();
void operator delete[](void * ptr) throw();
void operator delete[](void * ptr, const std::nothrow_t &) throw();

void allocator_setup();

#endif
