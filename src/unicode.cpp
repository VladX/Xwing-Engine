/*
 * This file is part of Opentube - Open video hosting engine
 *
 * Copyright (C) 2011 - Xpast; http://xpast.me/; <vvladxx@gmail.com>
 *
 * Opentube is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Opentube is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Opentube.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "unicode.hpp"
#ifdef WIN32
#include <windows.h>
#endif

namespace unicode
{

#ifdef WIN32
string utf16_to_utf8 (wstring & ws)
{
	int bufsize = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, 0, 0, 0, 0);
	if (bufsize == 0)
		bufsize = ws.length() * 2;
	string out;
	char * buf = new char[bufsize];
	bufsize = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, buf, bufsize, 0, 0);
	if (bufsize)
		out = buf;
	else
		out = "?";
	delete[] buf;
	return out;
}
#endif

};
