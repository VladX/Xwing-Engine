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
#include "server.hpp"
#include "process.hpp"
#include "cmdline_parser.hpp"

char ** argv;
int argc;

static void config ()
{
	try
	{
		CmdlineParser parser(argc, argv);
		opentube::config.load(parser.get_files());
	}
	catch (int e)
	{
		process_exit(e);
	}
	catch (const char * err)
	{
		LOG_CRITICAL(translate(err));
	}
	catch (exception & e)
	{
		LOG_CRITICAL(e.what());
	}
}

static void setup_locale ()
{
	generator gen;
	gen.add_messages_path(LOCALE_DIR);
	gen.add_messages_domain(GETTEXT_DOMAIN);
	locale::global(gen(""));
	cout.imbue(locale());
	cerr.imbue(locale());
	cin.imbue(locale());
}

int main (int argc, char ** argv)
{
	::argc = argc;
	::argv = argv;
	setup_locale();
	allocator_setup();
	config();
	process_init();
	server_init();
	return 0;
}
