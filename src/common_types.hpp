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

#ifndef __COMMON_TYPES_H_
#define __COMMON_TYPES_H_ 1

#include "external.hpp"

typedef unsigned char uchar;
typedef unsigned int uint;

class static_string {
private:
	const char * str;
	size_t len;
	
public:
	static_string () : str(""), len(0) {};
	static_string (const char * s) : str(s), len(strlen(s)) {};
	static_string (const char * s, size_t l) : str(s), len(l) {};
	inline static_string & operator= (const char * s) {
		str = s;
		len = strlen(s);
		return * this;
	};
	inline static_string & operator= (static_string & s) {
		str = s.c_str();
		len = s.length();
		return * this;
	};
	inline bool operator== (const static_string & s) const {
		return len == s.length() && memcmp(s.c_str(), str, len) == 0;
	};
	inline const char * c_str () const { return str; };
	inline size_t length () const { return len; };
};

struct static_string_eq {
	bool operator() (const static_string & s1, const static_string & s2) const {
		return s1 == s2;
	};
};

namespace std {
	template<>
	struct hash<static_string> : public unary_function<static_string, size_t> {
		size_t operator() (const static_string & s) const {
			return (size_t) external::cityhash64(s.c_str(), s.length());
		};
	};
};

#endif
