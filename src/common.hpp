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

#ifndef __COMMON_H_
#define __COMMON_H_ 1

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
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
#include "common_definitions.hpp"
#include "common_functions.hpp"
#include "allocator.hpp"
#include "localization.hpp"
#include "config.hpp"
#include "logger.hpp"
#include "external.hpp"
#include "static_memory_pool.hpp"
#include "hash_table.hpp"
#include "dynamic_array.hpp"
#include "queue.hpp"
using namespace std;

#endif
