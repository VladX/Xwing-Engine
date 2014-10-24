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

#ifndef __UNICODE_H
#define __UNICODE_H 1

#include "common.hpp"
#ifdef WIN32
#include <windows.h>
#endif

namespace unicode {

#ifdef WIN32
class streambuf_utf8 : public streambuf {
private:
	HANDLE hstdconsole;
	size_t sz, p;
	char buf[4];
	
public:
	streambuf_utf8 (DWORD);
	
protected:
	virtual streamsize xsputn (const char *, streamsize);
	virtual int overflow (int);
};

extern streambuf_utf8 win_console_cout;
extern streambuf_utf8 win_console_cerr;

string utf16_to_utf8 (wstring & ws);
#endif

};

#endif
