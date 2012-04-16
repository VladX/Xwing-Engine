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

#include "lua_modules.hpp"
using namespace boost::filesystem;

static int path_exists (lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	path p(name);
	lua_pushboolean(L, exists(p));
	return 1;
}

static int path_isfile (lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	path p(name);
	lua_pushboolean(L, is_regular_file(p));
	return 1;
}

static int path_isdir (lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	path p(name);
	lua_pushboolean(L, is_directory(p));
	return 1;
}

static int path_issymlink (lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	path p(name);
	lua_pushboolean(L, is_symlink(p));
	return 1;
}

static int path_absolute (lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	path p(name);
	try
	{
		p = absolute(p);
	}
	catch (exception & e)
	{
		lua_pushnil(L);
		lua_pushstring(L, e.what());
		return 2;
	}
	lua_pushstring(L, p.c_str());
	return 1;
}

static int path_current (lua_State * L)
{
	path p;
	try
	{
		p = current_path();
	}
	catch (exception & e)
	{
		lua_pushnil(L);
		lua_pushstring(L, e.what());
		return 2;
	}
	lua_pushstring(L, p.c_str());
	return 1;
}

static int path_canonical (lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	path p(name);
	try
	{
		p = canonical(p);
	}
	catch (exception & e)
	{
		lua_pushnil(L);
		lua_pushstring(L, e.what());
		return 2;
	}
	lua_pushstring(L, p.c_str());
	return 1;
}

static int path_size (lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	path p(name);
	uintmax_t sz;
	try
	{
		sz = file_size(p);
	}
	catch (exception & e)
	{
		lua_pushnil(L);
		lua_pushstring(L, e.what());
		return 2;
	}
	lua_pushnumber(L, sz);
	return 1;
}

static int path_mtime (lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	path p(name);
	std::time_t mtime;
	try
	{
		mtime = last_write_time(p);
	}
	catch (exception & e)
	{
		lua_pushnil(L);
		lua_pushstring(L, e.what());
		return 2;
	}
	lua_pushnumber(L, mtime);
	return 1;
}

static int path_basename (lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	path p(name);
	lua_pushstring(L, p.filename().c_str());
	return 1;
}

static int path_parent (lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	path p(name);
	lua_pushstring(L, p.remove_filename().c_str());
	return 1;
}

static int path_ext (lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	path p(name);
	lua_pushstring(L, p.extension().c_str());
	return 1;
}

static int path_join (lua_State * L)
{
	const char * parent = luaL_checkstring(L, 1);
	const char * name;
	path p(parent);
	for (int i = 2; (name = luaL_optstring(L, i, 0)); i++)
		p /= name;
	lua_pushstring(L, p.c_str());
	return 1;
}

static const luaL_Reg syslib_path[] = {
	{"exists", path_exists},
	{"isfile", path_isfile},
	{"isdir", path_isdir},
	{"issymlink", path_issymlink},
	{"absolute", path_absolute},
	{"canonical", path_canonical},
	{"current", path_current},
	{"basename", path_basename},
	{"parent", path_parent},
	{"ext", path_ext},
	{"join", path_join},
	{"mtime", path_mtime},
	{"file_size", path_size},
	{NULL, NULL}
};

LUAMOD_API int luaopen_path (lua_State * L)
{
	luaL_newlib(L, syslib_path);
	return 1;
}

void lua_load_module_path (lua_State * L)
{
	luaL_requiref(L, "path", luaopen_path, 1);
	lua_pop(L, 1);
}
