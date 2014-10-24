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
#include "js_module_utils.hpp"
#include "path.hpp"

namespace JS {

namespace modules {

static bool isfile (JSContext * cx, unsigned argc, Value * vp) {
	CallArgs args = CallArgsFromVp(argc, vp);
	JS_ENFORCE_ARGS_NUMBER(cx, args, 1);
	RootedString str(cx, args[0].toString());
	path p(JS_EncodeStringToUTF8(cx, str));
	args.rval().set(BOOLEAN_TO_JSVAL(p.isfile()));
	return true;
}

static bool isdir (JSContext * cx, unsigned argc, Value * vp) {
	CallArgs args = CallArgsFromVp(argc, vp);
	JS_ENFORCE_ARGS_NUMBER(cx, args, 1);
	RootedString str(cx, args[0].toString());
	path p(JS_EncodeStringToUTF8(cx, str));
	args.rval().set(BOOLEAN_TO_JSVAL(p.isdir()));
	return true;
}

static bool current (JSContext * cx, unsigned argc, Value * vp) {
	CallArgs args = CallArgsFromVp(argc, vp);
	JS_ENFORCE_ARGS_NUMBER(cx, args, 0);
	args.rval().set(StringValue(JS_NewStringCopyZ(cx, path::current().serialize().c_str())));
	return true;
}

static bool join (JSContext * cx, unsigned argc, Value * vp) {
	CallArgs args = CallArgsFromVp(argc, vp);
	JS_MINIMUM_ARGS_NUMBER(cx, args, 1);
	RootedString str(cx, args[0].toString());
	path p(JS_EncodeStringToUTF8(cx, str));
	for (size_t i = 1; i < args.length(); ++i) {
		str = args[i].toString();
		p.append(JS_EncodeStringToUTF8(cx, str));
	}
	args.rval().set(StringValue(JS_NewStringCopyZ(cx, p.serialize().c_str())));
	return true;
}

static JSFunctionSpec path_methods[] = {
	JS_FS("isFile", isfile, 1, JSFUN_GENERIC_NATIVE),
	JS_FS("isDir", isdir, 1, JSFUN_GENERIC_NATIVE),
	JS_FS("current", current, 0, JSFUN_GENERIC_NATIVE),
	JS_FS("join", join, 1, JSFUN_GENERIC_NATIVE),
	JS_FS_END
};

bool define_path (JSContext * cx, HandleObject global_obj) {
	RootedObject obj(cx, JS_DefineObject(cx, global_obj, "path", nullptr, NullPtr(), JSPROP_READONLY));
	if (!obj)
		return false;
	if (!JS_DefineFunctions(cx, obj, path_methods))
		return false;
	return true;
}

};

};
