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

#ifndef __JS_MODULE_UTILS_H_
#define __JS_MODULE_UTILS_H_ 1

namespace JS {

namespace modules {

#define JS_ENFORCE_ARGS_NUMBER(cx, args, expected) \
	if (args.length() != expected) {\
		JS_ReportError(cx, "wrong number of arguments, expected %d, passed %d", expected, args.length());\
		return false;\
	}

#define JS_MINIMUM_ARGS_NUMBER(cx, args, expected) \
	if (args.length() < expected) {\
		JS_ReportError(cx, "minimum number of arguments - %d, passed %d", expected, args.length());\
		return false;\
	}

};

};

#endif
