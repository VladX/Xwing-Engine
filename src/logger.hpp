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

#ifndef __LOGGER_H_
#define __LOGGER_H_ 1

#include "common.hpp"

class Logger
{
private:
	std::ostream * out;
	std::ostream * err;
	
	void print_level (size_t, std::ostream &, bool);
	
public:
	bool osyslog;
	bool ostdout;
	
	Logger () : out(&std::cout), err(&std::cerr), osyslog(true), ostdout(false) {};
	std::ostream & notice ();
	std::ostream & error ();
	std::ostream & error (std::ostream &);
	std::ostream & critical ();
	std::ostream & critical (std::ostream &);
	std::ostream & debug (const char *, int);
	std::ostream & debug (std::ostream &, const char *, int);
	void syslog (std::string, int);
	void set_ostream (std::ostream & strm_out, std::ostream & strm_err);
};

namespace opentube
{
	extern Logger logger;
};

#endif
