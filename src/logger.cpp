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

void Logger::print_time (ostream & err)
{
	const time_t curtime = time(0);
	char buf[32];
	strftime(buf, sizeof(buf), "%d/%m/%y %H:%M:%S", localtime(&curtime));
	err << buf << ' ';
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

char * Logger::system_last_error ()
{
#ifdef WIN32
	DWORD lasterror = GetLastError();
	LPCWCH lpvMessageBuffer = 0;
	static char utf8_str[256];
	int bufsize;
	char * ret = utf8_str;
	
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, lasterror, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  (LPWSTR) &lpvMessageBuffer, 0, 0);
	bufsize = WideCharToMultiByte(CP_UTF8, 0, lpvMessageBuffer, -1, 0, 0, 0, 0);
	
	if (bufsize == 0 || bufsize > sizeof(utf8_str))
		bufsize = sizeof(utf8_str);
	if (WideCharToMultiByte(CP_UTF8, 0, lpvMessageBuffer, -1, utf8_str, bufsize, 0, 0) == 0)
		ret = "error";
	
	LocalFree((void *) lpvMessageBuffer);
	
	bufsize = (int) strlen(ret) - 1;
	while (isspace(ret[bufsize]))
	{
		ret[bufsize] = '\0';
		bufsize--;
	}
	
	SetLastError(lasterror);
	
	return ret;
#else
	return strerror(getlasterror());
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
	return error(* err);
}

ostream & Logger::critical ()
{
	return critical(* err);
}

ostream & Logger::error (ostream & err)
{
	if (err != cerr)
		print_time(err);
	print_level(1, err, err == cerr);
	return err;
}

ostream & Logger::critical (ostream & err)
{
	if (err != cerr)
		print_time(err);
	print_level(2, err, err == cerr);
	return err;
}

ostream & Logger::debug (const char * file, int line)
{
	return debug(* out, file, line);
}

ostream & Logger::debug (ostream & out, const char * file, int line)
{
	out << "Debug message (File \"" << cbasename(file) << "\", line " << line << "): ";
	return out;
}
