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

#include "common.hpp"
#include "allocator.hpp"

extern "C"
{
	void * je_malloc (size_t);
	void * je_calloc (size_t, size_t);
	void * je_realloc (void *, size_t);
	void je_free (void *);
	size_t je_malloc_usable_size (void *);
	void malloc_init_hard ();
	void jemalloc_darwin_init ();
};

#ifdef OSX
static void * osx_malloc_hook (size_t);
#endif

#if defined(__GLIBC__) && !defined(__UCLIBC__)
void * (* __malloc_hook)(size_t) = je_malloc;
void * (* __realloc_hook)(void *, size_t) = je_realloc;
void (* __free_hook)(void *) = je_free;
#endif

namespace opentube
{

#if defined(WIN32)
void * (* alloc_fn)(size_t) = malloc;
void * (* realloc_fn)(void *, size_t) = realloc;
void (* free_fn)(void *) = free;
void (* init_fn)(void) = malloc_init_hard;
#elif defined(OSX)
void * (* alloc_fn)(size_t) = osx_malloc_hook;
void * (* realloc_fn)(void *, size_t) = je_realloc;
void (* free_fn)(void *) = je_free;
void (* init_fn)(void) = jemalloc_darwin_init;
#elif defined(BSD)
void * (* alloc_fn)(size_t) = malloc;
void * (* realloc_fn)(void *, size_t) = realloc;
void (* free_fn)(void *) = free;
void (* init_fn)(void) = 0;
#else
void * (* alloc_fn)(size_t) = je_malloc;
void * (* realloc_fn)(void *, size_t) = je_realloc;
void (* free_fn)(void *) = je_free;
void (* init_fn)(void) = 0;
#endif

GlobalAllocator<char> allocator;

};

#if defined(__GNUC__) || defined(CLANG) || defined(MINGW)
__attribute__((constructor))
#elif defined(MSVC)
#pragma section(".CRT$XCU",read)
static void __cdecl allocator_init (void);
__declspec(allocate(".CRT$XCU")) void (__cdecl*allocator_init_)(void) = allocator_init;
#endif
static void allocator_init ()
{
	if (opentube::init_fn)
	{
		opentube::init_fn();
		opentube::init_fn = 0;
	}
}

#ifdef OSX
static void * osx_malloc_hook (size_t size)
{
	if (opentube::init_fn)
	{
		opentube::init_fn();
		opentube::init_fn = 0;
	}
	opentube::alloc_fn = je_malloc;
	return opentube::alloc_fn(size);
}
#endif

#ifdef WIN32
static void win32_free_hook (void * ptr)
{
	if (je_malloc_usable_size(ptr))
		je_free(ptr);
	else
		free(ptr);
}

static void allocator_exit ()
{
	opentube::free_fn = win32_free_hook;
}
#endif

void allocator_setup ()
{
#ifdef WIN32
	opentube::alloc_fn = je_malloc;
	opentube::realloc_fn = je_realloc;
	opentube::free_fn = je_free;
	process_before_exit(allocator_exit);
#endif
}
