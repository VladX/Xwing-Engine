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

#include "cmdline_parser.hpp"
using namespace boost::property_tree;

CmdlineParser::CmdlineParser (int argc, char ** argv)
{
	const char copyright[] = "Copyright (C) 2011, 2012 - Xpast\nContact e-mail: <vvladxx@gmail.com>.";
	int i;
	if (argc < 2)
		read_json(CONFIG_PATH, pt);
	for (i = 1; i < argc; i++)
		if (strcmp(argv[i], "--version") == 0)
		{
			cout << PROG_NAME << ' ' << PROG_VERSION << endl;
			cout << endl << copyright << endl;
			throw 0;
		}
		else if (strcmp(argv[i], "--help") == 0)
		{
			string tmp;
			tmp += "  Usage: ";
			tmp += argv[0];
			tmp += ' ';
			size_t l = tmp.length();
			cout << tmp << "[-f config_file -f another_config_file ...]" << endl;
			tmp = ' ';
			while (--l)
				tmp += ' ';
			cout << tmp << "[-j \"{ foo: bar }\" ...]" << endl;
			cout << tmp << "[-s \"name\" \"value\" ...]" << endl;
			cout << endl << copyright << endl;
			throw 0;
		}
		else if (strcmp(argv[i], "-f") == 0)
		{
			if (i == argc - 1)
				throw "File name expected";
			ptree f;
			i++;
			read_json(argv[i], f);
			for (basic_ptree<string, string>::iterator it = f.begin(); it != f.end(); it++)
				pt.push_back(* it);
		}
		else if (strcmp(argv[i], "-j") == 0)
		{
			if (i == argc - 1)
				throw "JSON string expected";
			ptree j;
			i++;
			stringstream strm;
			strm << argv[i];
			read_json(strm, j);
			for (basic_ptree<string, string>::iterator it = j.begin(); it != j.end(); it++)
				pt.push_back(* it);
		}
		else if (strcmp(argv[i], "-s") == 0)
		{
			if (i >= argc - 2)
				throw "Key-value pair expected";
			pt.put(argv[i + 1], argv[i + 2]);
			i += 2;
		}
		else
			throw "Unknown option passed";
}
