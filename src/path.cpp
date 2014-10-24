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

#include "path.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

path::path (const string & p) : absolute(false) {
#ifdef WIN32
	string default_separator = this->default_separator;
	if (p.find('/') != string::npos)
		default_separator = "/";
	if (p.find(':') != string::npos)
		absolute = true;
#else
	if (p[0] == '/')
		absolute = true;
#endif
	for (size_t start, end = 0; end < p.length(); ++end) {
		start = end;
		end = p.find(default_separator, start);
		if (end == string::npos)
			end = p.length();
		if (start != end)
			components.push_back(p.substr(start, end - start));
	}
}

void path::append (const string & p) {
	path add(p);
	for (vector<string>::const_iterator it = add.components.cbegin(); it != add.components.cend(); ++it)
		components.push_back(* it);
}

bool path::isdir () {
#ifdef WIN32
	struct _stat st;
	if (_stat(serialize().c_str(), &st) == -1)
		return false;
	if ((_S_IFDIR & st.st_mode) == _S_IFDIR)
		return true;
#else
	struct stat st;
	if (stat(serialize().c_str(), &st) == -1)
		return false;
	if (S_ISDIR(st.st_mode))
		return true;
#endif
	return false;
}

bool path::isfile () {
#ifdef WIN32
	struct _stat st;
	if (_stat(serialize().c_str(), &st) == -1)
		return false;
	if ((_S_IFREG & st.st_mode) == _S_IFREG)
		return true;
#else
	struct stat st;
	if (stat(serialize().c_str(), &st) == -1)
		return false;
	if (S_ISREG(st.st_mode))
		return true;
#endif
	return false;
}

path path::current () {
	char buf[8*1024];
#ifdef WIN32
	GetCurrentDirectoryA(sizeof(buf), buf);
#else
	getcwd(buf, sizeof(buf));
#endif
	return path(buf);
}

string path::serialize () const {
	string result;
#ifndef WIN32
	if (absolute)
		result = default_separator;
#endif
	for (vector<string>::const_iterator it = components.cbegin(); it != components.cend(); ++it) {
		if (it != components.cbegin())
			result += default_separator;
		result += * it;
	}
	return result;
}
