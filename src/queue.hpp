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

#ifndef __QUEUE_H_
#define __QUEUE_H_ 1

#include "common.hpp"

/* Queue realization with O(1) indexed-access support (operator[]) and amortized O(1) pop_front().
   This realization should outperform standard queue from stl, which is based on linked-list, because it works in continuous memory segment.
 */
template<typename T>
class Queue : public DynamicArray<T> {
private:
	size_t qOffset;

public:
	Queue() : qOffset(0) {};
	Queue(size_t reserved) : qOffset(0), DynamicArray<T>(reserved) {};
	T & operator[](size_t n);
	const T & operator[](size_t n) const;
	void pop_front();
	void push_back(const T &);
};

template<typename T>
inline T & Queue<T>::operator[](size_t n) {
	ASSUME_ALIGNED(this->ptr, 16, T *);
	return this->ptr[qOffset + n];
}

template<typename T>
inline const T & Queue<T>::operator[](size_t n) const {
	ASSUME_ALIGNED(this->ptr, 16, T *);
	return this->ptr[qOffset + n];
}

template<typename T>
inline void Queue<T>::pop_front() {
	ASSUME_ALIGNED(this->ptr, 16, T *);
	++qOffset;
	if (unlikely(qOffset == this->usedSize)) {
		this->usedSize = 0;
		qOffset = 0;
	}
	else if (unlikely(qOffset > 16 && qOffset * 2 > this->usedSize)) {
		this->usedSize -= qOffset;
		memcpy(this->ptr, this->ptr + qOffset, this->usedSize * sizeof(T));
		qOffset = 0;
		this->shrink_to_fit(this->usedSize);
	}
}

template<typename T>
inline void Queue<T>::push_back(const T & el) {
	ASSUME_ALIGNED(this->ptr, 16, T *);
	if (unlikely(this->usedSize == this->actualSize)) {
		this->actualSize = this->grow(this->actualSize);
		this->ptr = GlobalAllocator<T>::reallocate(this->ptr, this->actualSize, this->usedSize, qOffset);
	}
	this->ptr[this->usedSize++] = el;
}

#endif
