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
#include "cmdline_parser.hpp"

static void config (int argc, char ** argv)
{
	try
	{
		CmdlineParser parser(argc, argv);
		opentube::config.load(parser.get_ptree());
	}
	catch (int e)
	{
		exit(e);
	}
	catch (const char * err)
	{
		cerr << translate("Error") << ": " << translate(err) << endl;
		exit(EXIT_FAILURE);
	}
	catch (exception & e)
	{
		cerr << translate("Error") << ": " << e.what() << endl;
		exit(EXIT_FAILURE);
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
	setup_locale();
	config(argc, argv);
	return 0;
}
