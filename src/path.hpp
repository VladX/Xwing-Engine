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

#ifndef __PATH_H_
#define __PATH_H_ 1

#include "common.hpp"

class path {
private:
#ifdef WIN32
	static constexpr const char * default_separator = "\\";
#else
	static constexpr const char * default_separator = "/";
#endif
	bool absolute = false;
protected:
	vector<string> components;
	
public:
	path (const string &);
	
	string serialize () const;
	
	void append (const string &);
	
	static path current ();
	
	bool isdir ();
	
	bool isfile ();
};

#endif
