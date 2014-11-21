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

#ifndef __HASH_TABLE_H_
#define __HASH_TABLE_H_ 1

#include "common.hpp"

template<typename valT, size_t buckets>
class HashTable {
private:
	struct memblock_header {
		struct memblock_header * next;
	};
	struct block {
		struct block * next;
		uint64_t hash;
		valT data;
	};
	struct block * allocated_list[buckets];
	struct block * free_list;
	struct memblock_header * mem;
	
	void add_new_memblock();
	struct block * find_by_hash (const uint64_t, struct block *);
	
public:
	HashTable ();
	valT & operator[] (const static_string &);
	valT & operator[] (const uint64_t);
	valT find (const static_string &, valT fail_return = 0);
	valT find (const uint64_t, valT fail_return = 0);
	void clear ();
	~HashTable ();
};

template<typename valT, size_t buckets>
inline void HashTable<valT, buckets>::add_new_memblock() {
	struct memblock_header * tmp = mem;
	mem = (struct memblock_header *) xwing::allocator.allocate(sizeof(struct memblock_header) + (sizeof(struct block) * buckets));
	ASSUME_ALIGNED(mem, 16, struct memblock_header *);
	mem->next = tmp;
	free_list = (struct block *) (((uchar *) mem) + sizeof(struct memblock_header));
	size_t i;
	for (i = 0; i < buckets - 1; i++)
		free_list[i].next = &free_list[i + 1];
	free_list[i].next = 0;
}

template<typename valT, size_t buckets>
HashTable<valT, buckets>::HashTable() {
	size_t i;
	for (i = 0; i < buckets; i++)
		allocated_list[i] = 0;
	mem = 0;
	add_new_memblock();
}

template<typename valT, size_t buckets>
inline typename HashTable<valT, buckets>::block * HashTable<valT, buckets>::find_by_hash (const uint64_t hash, struct block * list) {
	while (likely(list)) {
		if (likely(list->hash == hash))
			return list;
		list = list->next;
	}
	return 0;
}

template<typename valT, size_t buckets>
inline valT & HashTable<valT, buckets>::operator[] (const uint64_t hash) {
	if (unlikely(!free_list))
		add_new_memblock();
	uint64_t idx = hash % buckets;
	struct block * tmp = find_by_hash(hash, allocated_list[idx]);
	if (likely(tmp))
		return tmp->data;
	tmp = allocated_list[idx];
	allocated_list[idx] = free_list;
	free_list = free_list->next;
	allocated_list[idx]->next = tmp;
	allocated_list[idx]->hash = hash;
	return allocated_list[idx]->data;
}

template<typename valT, size_t buckets>
inline valT & HashTable<valT, buckets>::operator[] (const static_string & key) {
	return (* this)[external::cityhash64(key.c_str(), key.length())];
}

template<typename valT, size_t buckets>
inline valT HashTable<valT, buckets>::find (const uint64_t hash, valT fail_return) {
	uint64_t idx = hash % buckets;
	struct block * tmp = find_by_hash(hash, allocated_list[idx]);
	return (tmp) ? tmp->data : fail_return;
}

template<typename valT, size_t buckets>
valT HashTable<valT, buckets>::find (const static_string & key, valT fail_return) {
	return this->find(external::cityhash64(key.c_str(), key.length()), fail_return);
}

template<typename valT, size_t buckets>
void HashTable<valT, buckets>::clear () {
	struct memblock_header * m = mem, * tmp;
	while (m->next) {
		tmp = m;
		m = m->next;
		xwing::allocator.deallocate((char *) tmp);
	}
	mem = m;
	free_list = (struct block *) (((uchar *) mem) + sizeof(struct memblock_header));
	size_t i;
	for (i = 0; i < buckets - 1; i++)
		free_list[i].next = &free_list[i + 1];
	free_list[i].next = 0;
	for (i = 0; i < buckets; i++)
		allocated_list[i] = 0;
}

template<typename valT, size_t buckets>
HashTable<valT, buckets>::~HashTable() {
	struct memblock_header * m = mem, * tmp;
	while (m) {
		tmp = m;
		m = m->next;
		xwing::allocator.deallocate((char *) tmp);
	}
}

#endif
