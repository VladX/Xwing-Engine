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

#ifndef __JS_H_
#define __JS_H_ 1

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif

#include <jsapi.h>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

namespace JS {

namespace modules {

bool define_path (JSContext *, HandleObject);
bool define_os (JSContext *, HandleObject);

};

class MainInstance {
private:
	static void error_reporter (JSContext *, const char *, JSErrorReport *) throw();
	
public:
	static JSRuntime * rt;
	static JSContext * cx;
	static const size_t stack_chunk_size;
	
	static void init ();
	static JSObject * new_global ();
	static bool run_file (HandleObject, const char * filename);
	static void exit ();
};

};

#endif
