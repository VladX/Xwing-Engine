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
	void malloc_init_hard ();
	void jemalloc_darwin_init ();
};

#if defined(__GLIBC__) && !defined(__UCLIBC__)
void * (* __malloc_hook)(size_t) = je_malloc;
void * (* __realloc_hook)(void *, size_t) = je_realloc;
void (* __free_hook)(void *) = je_free;
#endif

#if defined(__GNUC__) || defined(CLANG) || defined(MINGW)
__attribute__((constructor))
#elif defined(MSVC)
#pragma section(".CRT$XCU",read)
static void __cdecl allocator_init (void);
__declspec(allocate(".CRT$XCU")) void (__cdecl*allocator_init_)(void) = allocator_init;
#endif
static void allocator_init ()
{
#if defined(WIN32)
	malloc_init_hard();
#elif defined(OSX)
	jemalloc_darwin_init();
#endif
}

namespace opentube
{
#if defined(WIN32)
	GlobalAllocator<char> allocator(je_malloc, je_realloc, je_free, malloc_init_hard);
#elif defined(OSX)
	GlobalAllocator<char> allocator(je_malloc, je_realloc, je_free, jemalloc_darwin_init);
#elif defined(BSD)
	GlobalAllocator<char> allocator;
#else
	GlobalAllocator<char> allocator(je_malloc, je_realloc, je_free);
#endif
};
