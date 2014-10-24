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

#include "common.hpp"
#include "config.hpp"
#include "js.hpp"
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace xwing {
	Config config;
};

static long js_getfield (JS::HandleObject obj, const char * key, long def) {
	JS::RootedValue val(JS::MainInstance::cx);
	if (!JS_GetProperty(JS::MainInstance::cx, obj, key, &val) || val == JSVAL_VOID)
		return def;
	if (!val.isNumber()) {
		string msg = "'";
		msg += key;
		msg += "' must be number"_tr;
		throw msg.c_str();
	}
	return (long) val.toNumber();
}

static string js_getfield (JS::HandleObject obj, const char * key, string def) {
	JS::RootedValue val(JS::MainInstance::cx);
	if (!JS_GetProperty(JS::MainInstance::cx, obj, key, &val) || val == JSVAL_VOID)
		return def;
	if (!val.isString()) {
		string msg = "'";
		msg += key;
		msg += "' must be string"_tr;
		throw msg.c_str();
	}
	JS::RootedString str(JS::MainInstance::cx, val.toString());
	return JS_EncodeStringToUTF8(JS::MainInstance::cx, str);
}

static long js_getfield (JS::HandleObject obj, const char * key, bool def, bool) {
	JS::RootedValue val(JS::MainInstance::cx);
	if (!JS_GetProperty(JS::MainInstance::cx, obj, key, &val) || val == JSVAL_VOID)
		return def;
	if (!val.isBoolean()) {
		string msg = "'";
		msg += key;
		msg += "' must be boolean"_tr;
		throw msg.c_str();
	}
	return val.toBoolean();
}

void Config::load (vector<string> & config_files) {
	bool detach = false;
	JS::MainInstance::init();
	{
		JSContext * cx = JS::MainInstance::cx;
		JSAutoRequest ar(cx);
		JS::RootedObject global(cx, JS::MainInstance::new_global());
		if (!global)
			throw "can't create global JS object"_tr;
		JSAutoCompartment ac(cx, global);
		JS_InitStandardClasses(cx, global);
		JS::modules::define_path(cx, global);
		JS::modules::define_os(cx, global);
		for (size_t i = 0; i < config_files.size(); ++i)
			if (!JS::MainInstance::run_file(global, config_files[i].c_str()))
				throw "Unable to load configuration"_tr;
		JS::RootedValue val(cx);
		if (!JS_GetProperty(cx, global, "server", &val) || !val.isObject())
			LOG_CRITICAL("'server' object not found in config file"_tr);
		JS::RootedObject server(cx, &val.toObject());
		// server.log-file
		{
			string logfile = js_getfield(server, "log-file", string(UNIX_PROG_NAME) + ".log");
			auto logstrm = new ofstream(logfile.c_str(), ios_base::app);
			if (logstrm->fail())
				throw last_error_exception("\"" + logfile + "\"");
			xwing::logger.set_ostream(* logstrm, * logstrm);
			xwing::logger.osyslog = false;
			xwing::logger.ostdout = true;
		}
		// server.listen
		{
			string listen = js_getfield(server, "listen", "127.0.0.1");
			if (listen.length() > 4 && memcmp("file:", listen.c_str(), 5) == 0) {
				host = listen.substr(5);
				port = 0;
				DEBUG_PRINT_1("File socket: " << host);
			}
			else {
				auto p = listen.rfind(':');
				if (p < listen.length()) {
					host = listen.substr(0, p);
					auto pr = atoi(listen.substr(p + 1).c_str());
					if (pr < 1 || pr > 0xFFFF)
						throw "Invalid port"_tr;
					if (host.length() == 0)
						throw "Invalid host"_tr;
					port = pr;
				}
				else {
					host = listen;
					port = 80; // if port is not specified, assuming 80
				}
				if (host.length() > 2 && host[0] == '[' && host[host.length() - 1] == ']')
					host = host.substr(1, host.length() - 2);
				DEBUG_PRINT_1("Host: " << host << ", port: " << port);
			}
		}
		user = js_getfield(server, "user", "nobody");
		group = js_getfield(server, "group", "nobody");
		sockperm = js_getfield(server, "socket-file-permissions", 0x660L);
		if (js_getfield(server, "detached", false, false))
			detach = true;
	}
	JS::MainInstance::exit();
	if (detach) {
#ifdef WIN32
		FreeConsole();
#else
		pid_t pid = fork();
		if (pid != 0)
			process_exit(0);
		setsid();
#endif
	}
}
