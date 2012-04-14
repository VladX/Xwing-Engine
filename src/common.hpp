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

#ifndef __COMMON_H_
#define __COMMON_H_ 1

#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <algorithm>
#include <string>
#include <iostream>
#include <stdexcept>
#include <boost/locale.hpp>
#include <boost/exception/all.hpp>
#include "config.h"
#ifdef HAVE_CPP0X_STYLE_HEADERS
#include <cstdlib>
#include <cstdint>
#include <cctype>
#include <cstring>
#else
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#endif
#include "common_types.hpp"
#include "common_defines.hpp"
#include "allocator.hpp"
#include "config.hpp"
#include "static_memory_pool.hpp"
using namespace boost::locale;
using namespace std;

#endif
