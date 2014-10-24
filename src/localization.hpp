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

#ifndef __LOCALIZATION_H_
#define __LOCALIZATION_H_ 1

#include "external.hpp"

namespace localization {

	void setup_locale ();
	const char * translate (const char *);
	const char * translate (const uint64_t);

};

#define gettext(x) localization::translate(x)

inline const char * operator "" _tr (const char * str, const size_t size) {
	const char * tr = localization::translate(EXTERNAL_CONSTEXPR_CITYHASH64(str, size));
	return (tr) ? tr : str;
}

#endif
