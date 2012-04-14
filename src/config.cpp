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
using boost::property_tree::ptree;

namespace opentube
{
	Config config;
};

static const char * required_directives[] = {
	"server.listen", "server.user", "server.group", "server.pid-file", "server.log-file", "server.data",
	"http.document-root", "http.temp", "http.cache.prefix", "http.cache.update",
	"template.name", "database.servers"
};

void Config::load (ptree & pt)
{
	auto host = pt.get<string>("server.listen.host");
	auto port = pt.get<uint16_t>("server.listen.port");
}
