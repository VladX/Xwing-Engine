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

#ifndef __DYNAMIC_ARRAY_H_
#define __DYNAMIC_ARRAY_H_ 1

#include "common.hpp"

/* Warning: This structure is not suitable for storage C++ objects which has constructor/destructor! Caller must take care about all initialization and resource freeing. */
template<typename T>
class DynamicArray {
private:
	size_t usedSize;
	size_t actualSize;
	T * ptr;
	/* Make the growth ratio close to the golden ratio */
	inline double grow(double x) { return x * 1.4 + 6.0; }
	inline double shrink(double x) { return x * 2.75 + 14.0; }

public:
	DynamicArray();
	DynamicArray(size_t);
	~DynamicArray();
	void push_back(const T &);
	T & operator[](size_t n);
	const T & operator[](size_t n) const;
	size_t size() const;
	void resize(size_t);
	void reserve(size_t);
};

template<typename T>
inline DynamicArray<T>::DynamicArray() : usedSize(0), actualSize(0), ptr(0) {}

template<typename T>
inline DynamicArray<T>::DynamicArray(size_t reserved) : usedSize(0) {
	actualSize = reserved;
	ptr = xwing::allocator.allocate(reserved * sizeof(T));
}

template<typename T>
inline DynamicArray<T>::~DynamicArray() {
	xwing::allocator.deallocate((char *) ptr);
}

/* Insert element in the end. O(1) amortized complexity. */
template<typename T>
inline void DynamicArray<T>::push_back(const T & el) {
	if (unlikely(usedSize == actualSize)) {
		actualSize = grow(actualSize);
		ptr = (T *) xwing::allocator.reallocate((char *) ptr, actualSize * sizeof(T), usedSize * sizeof(T));
	}
	ptr[usedSize++] = el;
}

template<typename T>
inline T & DynamicArray<T>::operator[](size_t n) {
	return ptr[n];
}

template<typename T>
inline const T & DynamicArray<T>::operator[](size_t n) const {
	return ptr[n];
}

template<typename T>
inline size_t DynamicArray<T>::size() const {
	return usedSize;
}

/* O(1) amortized complexity for sequental calling with constant step. */
template<typename T>
inline void DynamicArray<T>::resize(size_t ns) {
	if (ns > actualSize) {
		actualSize = grow(ns);
		ptr = (T *) xwing::allocator.reallocate((char *) ptr, actualSize * sizeof(T), usedSize * sizeof(T));
	}
	else if (shrink(ns) < actualSize) {
		ptr = (T *) xwing::allocator.reallocate((char *) ptr, ns * sizeof(T), usedSize * sizeof(T));
		actualSize = ns;
	}
	usedSize = ns;
}

template<typename T>
inline void DynamicArray<T>::reserve(size_t reserved) {
	ptr = (T *) xwing::allocator.reallocate((char *) ptr, reserved * sizeof(T), usedSize * sizeof(T));
	actualSize = reserved;
	if (reserved < usedSize)
		usedSize = reserved;
}

#endif
