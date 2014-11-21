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

#ifndef __EXTERNAL_H_
#define __EXTERNAL_H_ 1

#include "config.h"

#if !defined(IS_HOST_BIG_ENDIAN)
#include "external/cityhash_constexpr.hpp"
#define EXTERNAL_CONSTEXPR_CITYHASH64 external::consthash::city64
#else
#define EXTERNAL_CONSTEXPR_CITYHASH64 external::cityhash64
#endif

namespace external {

uint64_t cityhash64 (const char *, size_t);

};

#endif
