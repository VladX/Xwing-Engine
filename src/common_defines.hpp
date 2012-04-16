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

#ifndef __COMMON_DEFINES_H_
#define __COMMON_DEFINES_H_ 1

#ifdef COMPILER_HAVE_BUILTIN_EXPECT
#define likely(X) __builtin_expect(!!(X), 1)
#define unlikely(X) __builtin_expect(!!(X), 0)
#else
#define likely(X) (X)
#define unlikely(X) (X)
#endif

#if DEBUG_LEVEL >= 1
#define DEBUG_1(X) X
#else
#define DEBUG_1(X)
#endif

#if DEBUG_LEVEL >= 2
#define DEBUG_2(X) X
#else
#define DEBUG_2(X)
#endif

#if DEBUG_LEVEL >= 3
#define DEBUG_3(X) X
#else
#define DEBUG_3(X)
#endif

#define LOG_NOTICE(X) opentube::logger.notice() << X << std::endl
#define LOG_ERROR(X) { \
	if (opentube::logger.osyslog) \
	{ \
		std::stringstream buf; \
		buf << X; \
		opentube::logger.syslog(buf.str(), 1); \
	} \
	if (opentube::logger.ostdout) \
		opentube::logger.error(cerr) << X << std::endl; \
	opentube::logger.error() << X << std::endl; \
}
#define LOG_CRITICAL(X) { \
	if (opentube::logger.osyslog) \
	{ \
		std::stringstream buf; \
		buf << X; \
		opentube::logger.syslog(buf.str(), 2); \
	} \
	if (opentube::logger.ostdout) \
		opentube::logger.critical(cerr) << X << std::endl; \
	opentube::logger.critical() << X << std::endl; \
	exit(EXIT_FAILURE); \
}

#define DEBUG_PRINT_1(X) DEBUG_1({ \
	if (opentube::logger.ostdout) \
		opentube::logger.debug(cout, __FILE__, __LINE__) << X << std::endl; \
	opentube::logger.debug(__FILE__, __LINE__) << X << std::endl; \
})
#define DEBUG_PRINT_2(X) DEBUG_2({ \
	if (opentube::logger.ostdout) \
		opentube::logger.debug(cout, __FILE__, __LINE__) << X << std::endl; \
	opentube::logger.debug(__FILE__, __LINE__) << X << std::endl; \
})
#define DEBUG_PRINT_3(X) DEBUG_3({ \
	if (opentube::logger.ostdout) \
		opentube::logger.debug(cout, __FILE__, __LINE__) << X << std::endl; \
	opentube::logger.debug(__FILE__, __LINE__) << X << std::endl; \
})

#define LAST_ERROR_EXCEPTION(M) boost::system::system_error(getlasterror(), boost::system::get_system_category(), M)

#endif
