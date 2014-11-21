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

#ifndef __COMMON_DEFINES_H_
#define __COMMON_DEFINES_H_ 1

#ifdef COMPILER_HAVE_BUILTIN_EXPECT
#define likely(X) __builtin_expect(!!(X), 1)
#define unlikely(X) __builtin_expect(!!(X), 0)
#else
#define likely(X) (X)
#define unlikely(X) (X)
#endif

#if defined(__GNUC__)
#define ATTRIBUTE_RESTRICT __restrict__
#elif defined(_MSC_VER)
#define ATTRIBUTE_RESTRICT __restrict
#else
#define ATTRIBUTE_RESTRICT
#endif

#ifdef COMPILER_HAVE_ASSUME_ALIGNED
#define ASSUME_ALIGNED(X, A, TYPE) X = (TYPE) __builtin_assume_aligned(X, A)
#else
#define ASSUME_ALIGNED(X, A, TYPE)
#endif

#if defined(__GNUC__)
#define threadlocal __thread
#else
#define threadlocal __declspec(thread)
#endif

#if DEBUG_LEVEL == 0
#undef assert
#define assert(X)
#else
#include <assert.h>
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

#define LOG_NOTICE(X) xwing::logger.notice() << X << std::endl
#define LOG_ERROR(X) { \
	if (xwing::logger.osyslog) { \
		std::stringstream buf; \
		buf << X; \
		xwing::logger.syslog(buf.str(), 1); \
	} \
	if (xwing::logger.ostdout) \
		xwing::logger.error(std::cerr) << X << std::endl; \
	xwing::logger.error() << X << std::endl; \
}
#define LOG_CRITICAL(X) { \
	if (xwing::logger.osyslog) { \
		std::stringstream buf; \
		buf << X; \
		xwing::logger.syslog(buf.str(), 2); \
	} \
	if (xwing::logger.ostdout) \
		xwing::logger.critical(std::cerr) << X << std::endl; \
	xwing::logger.critical() << X << std::endl; \
	process_exit(EXIT_FAILURE); \
}
#define LOG_CRITICAL_P(X) { \
	if (xwing::logger.osyslog) { \
		std::stringstream buf; \
		buf << X << ": " << xwing::logger.system_last_error(); \
		xwing::logger.syslog(buf.str(), 2); \
	} \
	if (xwing::logger.ostdout) \
		xwing::logger.critical(std::cerr) << X << ": " << xwing::logger.system_last_error() << std::endl; \
	xwing::logger.critical() << X << ": " << xwing::logger.system_last_error() << std::endl; \
	process_exit(EXIT_FAILURE); \
}

#define DEBUG_PRINT_1(X) DEBUG_1({ \
	if (xwing::logger.ostdout) \
		xwing::logger.debug(std::cout, __FILE__, __LINE__) << X << std::endl; \
	xwing::logger.debug(__FILE__, __LINE__) << X << std::endl; \
})
#define DEBUG_PRINT_2(X) DEBUG_2({ \
	if (xwing::logger.ostdout) \
		xwing::logger.debug(std::cout, __FILE__, __LINE__) << X << std::endl; \
	xwing::logger.debug(__FILE__, __LINE__) << X << std::endl; \
})
#define DEBUG_PRINT_3(X) DEBUG_3({ \
	if (xwing::logger.ostdout) \
		xwing::logger.debug(std::cout, __FILE__, __LINE__) << X << std::endl; \
	xwing::logger.debug(__FILE__, __LINE__) << X << std::endl; \
})

#ifdef WIN32
int getlasterror ();
#else
#define getlasterror() errno
#endif

class last_error_exception : public std::exception {
public:
	std::string msg;
	
	last_error_exception(const std::string info) {
		msg = strerror(getlasterror());
		msg += ": ";
		msg += info;
	}
	
	const char * what() const throw() { return msg.c_str(); }
};

#endif
