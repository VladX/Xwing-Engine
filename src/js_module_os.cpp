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

#include "js.hpp"
#include "config.h"

namespace JS {

namespace modules {

bool define_os (JSContext * cx, HandleObject global_obj) {
	RootedObject obj(cx, JS_DefineObject(cx, global_obj, "os", nullptr, NullPtr(), JSPROP_READONLY));
	if (!obj)
		return false;
#ifdef LINUX
	JS_DefineProperty(cx, obj, "linux", 1, JSPROP_READONLY);
#else
	JS_DefineProperty(cx, obj, "linux", 0, JSPROP_READONLY);
#endif
#ifdef WIN32
	JS_DefineProperty(cx, obj, "windows", 1, JSPROP_READONLY);
#else
	JS_DefineProperty(cx, obj, "windows", 0, JSPROP_READONLY);
#endif
#ifdef OSX
	JS_DefineProperty(cx, obj, "osx", 1, JSPROP_READONLY);
#else
	JS_DefineProperty(cx, obj, "osx", 0, JSPROP_READONLY);
#endif
#ifdef BSD
	JS_DefineProperty(cx, obj, "bsd", 1, JSPROP_READONLY);
#else
	JS_DefineProperty(cx, obj, "bsd", 0, JSPROP_READONLY);
#endif
	return true;
}

};

};
