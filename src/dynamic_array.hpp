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
	static_assert(std::is_pod<T>::value, "DynamicArray can't be used with non-POD types");
	
protected:
	size_t usedSize;
	size_t actualSize;
	T * ptr;
	/* Make the growth ratio close to the golden ratio */
	inline double grow(size_t x) { return (x < 512 / sizeof(T)) ? 512 / sizeof(T) : (double) x * 1.5 + 2.5; }
	inline double shrink(size_t x) { return (x < 512 / sizeof(T)) ? 512 / sizeof(T) : (double) x * 2.25 + 6.25; }

public:
	DynamicArray();
	DynamicArray(size_t);
	~DynamicArray();
	void push_back(const T &);
	T & operator[](size_t n);
	const T & operator[](size_t n) const;
	size_t size() const;
	void resize(size_t);
	void extend_to_fit(size_t);
	void shrink_to_fit(size_t);
	void reserve(size_t);
};

template<typename T>
inline DynamicArray<T>::DynamicArray() : usedSize(0), actualSize(0), ptr(0) {}

template<typename T>
inline DynamicArray<T>::DynamicArray(size_t reserved) : usedSize(0) {
	actualSize = reserved;
	ptr = GlobalAllocator<T>::allocate(reserved);
}

template<typename T>
inline DynamicArray<T>::~DynamicArray() {
	GlobalAllocator<T>::deallocate(ptr);
}

/* Insert element in the end. O(1) amortized complexity. */
template<typename T>
inline void DynamicArray<T>::push_back(const T & el) {
	ASSUME_ALIGNED(ptr, 16, T *);
	if (unlikely(usedSize == actualSize)) {
		actualSize = grow(actualSize);
		ptr = GlobalAllocator<T>::reallocate(ptr, actualSize, usedSize);
	}
	ptr[usedSize++] = el;
}

template<typename T>
inline T & DynamicArray<T>::operator[](size_t n) {
	ASSUME_ALIGNED(ptr, 16, T *);
	DEBUG_1(
		if (n >= usedSize)
			throw std::out_of_range("");
	);
	return ptr[n];
}

template<typename T>
inline const T & DynamicArray<T>::operator[](size_t n) const {
	ASSUME_ALIGNED(ptr, 16, T *);
	DEBUG_1(
		if (n >= usedSize)
			throw std::out_of_range("");
	);
	return ptr[n];
}

template<typename T>
inline size_t DynamicArray<T>::size() const {
	return usedSize;
}

/* O(1) amortized complexity for sequental calling with constant step. */
template<typename T>
inline void DynamicArray<T>::resize(size_t ns) {
	ASSUME_ALIGNED(ptr, 16, T *);
	if (ns > actualSize) {
		actualSize = grow(ns);
		ptr = GlobalAllocator<T>::reallocate(ptr, actualSize, usedSize);
	}
	else if (shrink(ns) < actualSize) {
		ptr = GlobalAllocator<T>::reallocate(ptr, ns, usedSize);
		actualSize = ns;
	}
	usedSize = ns;
}

template<typename T>
inline void DynamicArray<T>::extend_to_fit(size_t ns) {
	ASSUME_ALIGNED(ptr, 16, T *);
	if (ns > actualSize) {
		actualSize = grow(ns);
		ptr = GlobalAllocator<T>::reallocate(ptr, actualSize, usedSize);
	}
	if (ns > usedSize)
		usedSize = ns;
}

template<typename T>
inline void DynamicArray<T>::shrink_to_fit(size_t ns) {
	ASSUME_ALIGNED(ptr, 16, T *);
	if (shrink(ns) < actualSize) {
		ptr = GlobalAllocator<T>::reallocate(ptr, ns, usedSize);
		actualSize = ns;
	}
	if (ns < usedSize)
		usedSize = ns;
}

template<typename T>
inline void DynamicArray<T>::reserve(size_t reserved) {
	ASSUME_ALIGNED(ptr, 16, T *);
	ptr = GlobalAllocator<T>::reallocate(ptr, reserved, usedSize);
	actualSize = reserved;
	if (reserved < usedSize)
		usedSize = reserved;
}

#endif
