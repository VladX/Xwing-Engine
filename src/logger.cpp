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

#include "common.hpp"
#ifdef WIN32
#include <windows.h>
#endif
#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif

namespace opentube
{
	Logger logger;
};

void Logger::set_ostream (ostream & strm_out, ostream & strm_err)
{
	out = &strm_out;
	err = &strm_err;
}

void Logger::print_level (size_t id, ostream & ostrm, bool colorize)
{
	static const char * const names[] = {"Notice", "Error", "Critical"};
#ifdef WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	if (colorize)
	{
		if (id == 1)
#ifdef WIN32
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_RED);
#else
			ostrm << "\x1b[0;37;41m";
#endif
		else if (id == 2)
#ifdef WIN32
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_RED);
#else
			ostrm << "\x1b[1;37;41m";
#endif
	}
	ostrm << '[' << translate(names[id]) << ']';
#ifdef WIN32
	if (colorize)
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	ostrm << ' ';
#else
	if (colorize)
		ostrm << "\x1b[0;0;0m ";
	else
		ostrm << ' ';
#endif
}

void Logger::syslog (string str, int level)
{
#ifdef HAVE_SYSLOG_H
	if (!osyslog)
		return;
	if (level == 1)
		level = LOG_ERR;
	else if (level == 2)
		level = LOG_CRIT;
	::openlog(PROG_NAME, LOG_PID, LOG_DAEMON);
	::syslog(level, "%s", str.c_str());
	::closelog();
#endif
}

ostream & Logger::notice ()
{
	return (* out);
}

ostream & Logger::error ()
{
	print_level(1, * err, err == &cerr);
	return (* err);
}

ostream & Logger::critical ()
{
	print_level(2, * err, err == &cerr);
	return (* err);
}

ostream & Logger::debug (const char * file, int line)
{
	(* out) << "Debug message (File \"" << cbasename(file) << "\", line " << line << "): ";
	return (* out);
}
