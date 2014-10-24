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

#ifndef __LOGGER_H_
#define __LOGGER_H_ 1

#include "common.hpp"

class Logger {
public:
	typedef std::ostream logstream;
	
private:
	logstream * out;
	logstream * err;
	
	void print_level (size_t, logstream &, bool);
	void print_time (logstream &);
	
public:
	bool osyslog;
	bool ostdout;
	
	Logger ();
	char * system_last_error ();
	logstream & notice ();
	logstream & error ();
	logstream & error (logstream &);
	logstream & critical ();
	logstream & critical (logstream &);
	logstream & debug (const char *, int);
	logstream & debug (logstream &, const char *, int);
	void syslog (std::string, int);
	void set_ostream (logstream & strm_out, logstream & strm_err);
};

namespace xwing {
	extern Logger logger;
};

#endif
