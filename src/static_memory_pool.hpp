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

#ifndef __STATIC_MEMORY_POOL_H_
#define __STATIC_MEMORY_POOL_H_ 1

#include "common.hpp"

template<typename T, size_t reserved_blocks>
class StaticMemoryPool {
private:
	struct memblock_header {
		struct memblock_header * next;
	};
	struct block {
		/* Don't change this struct */
		struct block * next;
		struct block * prev;
		T data;
		/* --- */
	};
	struct block * allocated_list;
	struct block * free_list;
	struct memblock_header * mem;
	size_t mem_size;
	
	void add_new_memblock();
	
public:
	StaticMemoryPool ();
	T * allocate ();
	T * allocate (size_t) { return allocate(); };
	T * allocate (size_t, const T *) { return allocate(); };
	void deallocate (T * ptr);
	void deallocate (T * ptr, size_t) { deallocate(ptr); };
	void free_unused ();
	~StaticMemoryPool ();
};

template<typename T, size_t reserved_blocks>
inline void StaticMemoryPool<T, reserved_blocks>::add_new_memblock() {
	struct memblock_header * tmp = mem;
	mem = (struct memblock_header *) xwing::allocator.allocate(sizeof(struct memblock_header) + (sizeof(struct block) * reserved_blocks));
	mem->next = tmp;
	free_list = (struct block *) (((uchar *) mem) + sizeof(struct memblock_header));
	size_t i;
	free_list[0].next = &free_list[1];
	free_list[0].prev = 0;
	for (i = 1; i < reserved_blocks - 1; i++) {
		free_list[i].next = &free_list[i + 1];
		free_list[i].prev = &free_list[i - 1];
	}
	free_list[i].next = 0;
	free_list[i].prev = &free_list[i - 1];
	mem_size++;
}

template<typename T, size_t reserved_blocks>
StaticMemoryPool<T, reserved_blocks>::StaticMemoryPool() {
	allocated_list = 0;
	mem = 0;
	mem_size = 0;
	add_new_memblock();
}

template<typename T, size_t reserved_blocks>
inline T * StaticMemoryPool<T, reserved_blocks>::allocate () {
	if (!free_list)
		add_new_memblock();
	struct block * tmp = allocated_list;
	allocated_list = free_list;
	free_list = free_list->next;
	allocated_list->next = tmp;
	if (allocated_list->next)
		allocated_list->next->prev = allocated_list;
	if (free_list)
		free_list->prev = 0;
	new(&allocated_list->data) T;
	
	return &allocated_list->data;
}

template<typename T, size_t reserved_blocks>
inline void StaticMemoryPool<T, reserved_blocks>::deallocate (T * ptr) {
	ptr->~T();
	struct block * tmp = (struct block *) (((uchar *) ptr) - 2 * sizeof(void *));
	if (tmp->prev)
		tmp->prev->next = tmp->next;
	else
		allocated_list = tmp->next;
	if (tmp->next)
		tmp->next->prev = tmp->prev;
	tmp->next = free_list;
	tmp->prev = 0;
	free_list = tmp;
}

template<typename T, size_t reserved_blocks>
void StaticMemoryPool<T, reserved_blocks>::free_unused () {
	struct memblock_header * m = mem, * tmp = 0, * p = 0;
	struct block * b;
	size_t ndeallocated = 1; // 1 is because we should keep one mem block for later use
	while (m && ndeallocated < mem_size) {
		for (b = allocated_list; b; b = b->next)
			if ((uchar *) b > (uchar *) m && (uchar *) b < ((uchar *) m) + sizeof(struct memblock_header) + (sizeof(struct block) * reserved_blocks))
				break;
		if (!b && tmp)
			tmp->next = m->next;
		else if (!b)
			mem = m->next;
		p = tmp;
		tmp = m;
		m = m->next;
		if (!b) {
			for (b = free_list; b; b = b->next)
				if ((uchar *) b > (uchar *) tmp && (uchar *) b < ((uchar *) tmp) + sizeof(struct memblock_header) + (sizeof(struct block) * reserved_blocks)) {
					if (b->prev)
						b->prev->next = b->next;
					else
						free_list = b->next;
					if (b->next)
						b->next->prev = b->prev;
				}
			xwing::allocator.deallocate((char *) tmp);
			tmp = p;
			ndeallocated++;
		}
	}
	mem_size -= ndeallocated - 1;
}

template<typename T, size_t reserved_blocks>
StaticMemoryPool<T, reserved_blocks>::~StaticMemoryPool() {
	struct memblock_header * m = mem, * tmp;
	while (m) {
		tmp = m;
		m = m->next;
		xwing::allocator.deallocate((char *) tmp);
	}
}

#endif
