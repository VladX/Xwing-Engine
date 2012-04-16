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

#include "lua.hpp"

static void * lua_allocator_fn (void *, void * ptr, size_t, size_t nsize)
{
	if (nsize == 0)
	{
		opentube::allocator.deallocate(reinterpret_cast<char *>(ptr));
		return NULL;
	}
	else
		return opentube::allocator.reallocate(reinterpret_cast<char *>(ptr), nsize);
}

lua_State * lua_new ()
{
	return lua_newstate(lua_allocator_fn, 0);
}

void lua_load_module_base (lua_State * L)
{
	luaL_requiref(L, "_G", luaopen_base, 1);
	lua_pop(L, 1);
}

void lua_load_module_os (lua_State * L)
{
	luaL_requiref(L, "os", luaopen_os, 1);
	lua_pushstring(L, "windows");
#ifdef WIN32
	lua_pushboolean(L, 1);
#else
	lua_pushboolean(L, 0);
#endif
	lua_settable(L, -3);
	lua_pushstring(L, "linux");
#ifdef LINUX
	lua_pushboolean(L, 1);
#else
	lua_pushboolean(L, 0);
#endif
	lua_settable(L, -3);
	lua_pushstring(L, "darwin");
#ifdef OSX
	lua_pushboolean(L, 1);
#else
	lua_pushboolean(L, 0);
#endif
	lua_settable(L, -3);
	lua_pushstring(L, "bsd");
#ifdef BSD
	lua_pushboolean(L, 1);
#else
	lua_pushboolean(L, 0);
#endif
	lua_settable(L, -3);
	lua_pop(L, 1);
}
