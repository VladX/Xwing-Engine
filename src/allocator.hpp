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

#ifndef __ALLOCATOR_H_
#define __ALLOCATOR_H_ 1

#include "common.hpp"

namespace opentube
{

extern void * (* alloc_fn)(size_t);
extern void * (* realloc_fn)(void *, size_t);
extern void (* free_fn)(void *);
extern void (* init_fn)(void);

};

template<typename T>
class GlobalAllocator : public std::allocator<T>
{
public:
	T * allocate (size_t);
	T * allocate (size_t size, const T *) { return allocate(size); };
	T * zallocate (size_t);
	T * reallocate (T *, size_t);
	void deallocate (T *);
	void deallocate (T * ptr, size_t) { deallocate(ptr); };
};

template<typename T>
inline T * GlobalAllocator<T>::allocate (size_t size)
{
	T * ptr = reinterpret_cast<T *>(opentube::alloc_fn(sizeof(T) * size));
	if (ptr)
		return ptr;
	if (size != 0)
		throw std::bad_alloc();
	return ptr;
}

template<typename T>
inline T * GlobalAllocator<T>::zallocate (size_t size)
{
	return reinterpret_cast<T *>(memset(allocate(size), 0, size));
}

template<typename T>
inline T * GlobalAllocator<T>::reallocate (T * ptr, size_t new_size)
{
	ptr = reinterpret_cast<T *>(opentube::realloc_fn(ptr, sizeof(T) * new_size));
	if (likely(ptr))
		return ptr;
	if (new_size != 0)
		throw std::bad_alloc();
	return ptr;
}

template<typename T>
inline void GlobalAllocator<T>::deallocate (T * ptr)
{
	opentube::free_fn(ptr);
}

namespace opentube
{

extern GlobalAllocator<char> allocator;

};

inline void * operator new (size_t size)
{
	return opentube::allocator.allocate(size);
}

inline void * operator new (size_t size, const std::nothrow_t &) throw()
{
	return opentube::alloc_fn(size);
}

inline void * operator new[] (size_t size)
{
	return opentube::allocator.allocate(size);
}

inline void * operator new[] (size_t size, const std::nothrow_t &) throw()
{
	return opentube::alloc_fn(size);
}

inline void operator delete (void * ptr)
{
	opentube::allocator.deallocate(reinterpret_cast<char *>(ptr));
}

inline void operator delete (void * ptr, const std::nothrow_t &) throw()
{
	opentube::allocator.deallocate(reinterpret_cast<char *>(ptr));
}

inline void operator delete[] (void * ptr)
{
	opentube::allocator.deallocate(reinterpret_cast<char *>(ptr));
}

inline void operator delete[] (void * ptr, const std::nothrow_t &) throw()
{
	opentube::allocator.deallocate(reinterpret_cast<char *>(ptr));
}

void allocator_setup ();

#endif
