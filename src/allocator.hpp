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

template<typename T>
class GlobalAllocator : public std::allocator<T>
{
private:
	typedef void * (* alloc_fn_t) (size_t);
	typedef void * (* realloc_fn_t) (void *, size_t);
	typedef void (* free_fn_t) (void *);
	alloc_fn_t alloc_fn;
	realloc_fn_t realloc_fn;
	free_fn_t free_fn;
	
public:
	GlobalAllocator ();
	GlobalAllocator (alloc_fn_t alloc_fn, realloc_fn_t realloc_fn, free_fn_t free_fn, void (* init_fn)(void) = 0);
	~GlobalAllocator () {};
	void set_functions (alloc_fn_t alloc_fn, realloc_fn_t realloc_fn, free_fn_t free_fn, void (* init_fn)(void) = 0);
	T * allocate (size_t size) throw (std::bad_alloc);
	T * allocate (size_t size, const T *) throw (std::bad_alloc) { return allocate(size); };
	T * reallocate (T * ptr, size_t new_size) throw (std::bad_alloc);
	void deallocate (T * ptr);
	void deallocate (T * ptr, size_t) { deallocate(ptr); };
};

template<typename T>
GlobalAllocator<T>::GlobalAllocator ()
{
	set_functions(reinterpret_cast<alloc_fn_t>(std::malloc), reinterpret_cast<realloc_fn_t>(std::realloc), reinterpret_cast<free_fn_t>(std::free));
}

template<typename T>
GlobalAllocator<T>::GlobalAllocator (alloc_fn_t alloc_fn, realloc_fn_t realloc_fn, free_fn_t free_fn, void (* init_fn)(void))
{
	set_functions(alloc_fn, realloc_fn, free_fn, init_fn);
}

template<typename T>
void GlobalAllocator<T>::set_functions (alloc_fn_t alloc_fn, realloc_fn_t realloc_fn, free_fn_t free_fn, void (* init_fn)(void))
{
	this->alloc_fn = alloc_fn;
	this->realloc_fn = realloc_fn;
	this->free_fn = free_fn;
	if (init_fn)
		init_fn();
}

template<typename T>
inline T * GlobalAllocator<T>::allocate (size_t size) throw (std::bad_alloc)
{
	T * ptr = reinterpret_cast<T *>(alloc_fn(sizeof(T) * size));
	if (ptr)
		return ptr;
	if (size != 0)
		throw std::bad_alloc();
	return ptr;
}

template<typename T>
inline T * GlobalAllocator<T>::reallocate (T * ptr, size_t new_size) throw (std::bad_alloc)
{
	ptr = reinterpret_cast<T *>(realloc_fn(ptr, sizeof(T) * new_size));
	if (likely(ptr))
		return ptr;
	if (new_size != 0)
		throw std::bad_alloc();
	return ptr;
}

template<typename T>
inline void GlobalAllocator<T>::deallocate (T * ptr)
{
	free_fn(ptr);
}

namespace opentube
{
	extern GlobalAllocator<char> allocator;
};

#endif
