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

#include "unicode.hpp"

namespace unicode {

#ifdef WIN32
string utf16_to_utf8 (wstring & ws) {
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

streambuf_utf8::streambuf_utf8 (DWORD nStdHandle) : sz(0), p(0) {
	hstdconsole = GetStdHandle(nStdHandle);
}

streamsize streambuf_utf8::xsputn (const char * s, streamsize n) {
	for (streamsize i = 0; i < n; ++i)
		this->overflow(s[i]);
	return n;
}

int streambuf_utf8::overflow (int c) {
	DWORD out;
	if (p < sz) {
		buf[p++] = (char) c;
		if (p == sz)
			WriteConsoleA(hstdconsole, buf, sz, &out, NULL);
		return c;
	}
	if (((unsigned char) c) < 128)
		WriteConsoleA(hstdconsole, &c, 1, &out, NULL);
	else if ((((unsigned char) c) & 224) == 192) {
		buf[0] = (char) c;
		p = 1;
		sz = 2;
	}
	else if ((((unsigned char) c) & 240) == 224) {
		buf[0] = (char) c;
		p = 1;
		sz = 3;
	}
	else {
		buf[0] = (char) c;
		p = 1;
		sz = 4;
	}
	return c;
}

streambuf_utf8 win_console_cout(STD_OUTPUT_HANDLE);
streambuf_utf8 win_console_cerr(STD_ERROR_HANDLE);

#endif

};
