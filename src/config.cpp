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
#include "config.hpp"
#include "lua.hpp"

namespace opentube
{
	Config config;
};

static double lua_getfield (lua_State * L, const char * key, double def)
{
	int result;
	lua_pushstring(L, key);
	lua_gettable(L, -2);
	if (lua_isnil(L, -1))
		return def;
	if (!lua_isnumber(L, -1))
	{
		string msg = "'";
		msg += key;
		msg += "' must be number";
		throw msg.c_str();
	}
	result = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return result;
}

static string lua_getfield (lua_State * L, const char * key, string def)
{
	string result;
	lua_pushstring(L, key);
	lua_gettable(L, -2);
	if (lua_isnil(L, -1))
	{
		lua_pop(L, 2);
		return def;
	}
	if (!lua_isstring(L, -1))
	{
		string msg = "'";
		msg += key;
		msg += "' must be string";
		throw msg.c_str();
	}
	result = lua_tostring(L, -1);
	lua_pop(L, 1);
	return result;
}

void Config::load (vector<string> & config_files)
{
	lua_State * L = lua_new();
	if (!L)
		throw "lua_open() error";
	lua_load_module_base(L);
	lua_load_module_os(L);
	lua_load_module_path(L);
	for (size_t i = 0; i < config_files.size(); i++)
		if (luaL_dofile(L, config_files[i].c_str()))
			throw lua_tostring(L, -1);
	lua_getglobal(L, "server");
	if (!lua_istable(L, -1))
		throw "'server' must be table";
	// server.log-file
	{
		string logfile = boost::filesystem::absolute(lua_getfield(L, "log-file", "")).string();
		auto logstrm = new ofstream(logfile, ios_base::app);
		if (logstrm->fail())
			throw LAST_ERROR_EXCEPTION(logfile);
		opentube::logger.set_ostream(* logstrm, * logstrm);
		opentube::logger.osyslog = false;
		opentube::logger.ostdout = true;
	}
	// server.listen
	{
		string listen = lua_getfield(L, "listen", "localhost");
		auto p = listen.rfind(':');
		if (p < listen.length())
		{
			host = listen.substr(0, p);
			auto pr = atoi(listen.substr(p + 1).c_str());
			if (pr < 1 || pr > 0xFFFF)
				throw "Invalid port";
			if (host.length() == 0)
				throw "Invalid host";
			port = pr;
		}
		else
		{
			host = listen;
			port = 80; // if port not found, assuming 80
		}
		DEBUG_PRINT_1("Host: " << host << ", port: " << port);
	}
	lua_close(L);
}
