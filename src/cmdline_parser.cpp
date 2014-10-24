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

#include "cmdline_parser.hpp"

CmdlineParser::CmdlineParser(int argc, char ** argv) {
	const char copyright[] = "Copyright (C) 2014 - Xpast\nContact e-mail: <vvladxx@gmail.com>.";
	int i;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
			cout << PROG_NAME << ' ' << PROG_VERSION << endl;
			cout << endl << copyright << endl;
			throw 0;
		}
		else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
			cout << " Usage: "_tr << cbasename(argv[0]) << " [-f config_file -f another_config_file ...]\n\n\
    -v, --version       Show version\n\
    -h, --help          Show this message\n\
    -f config_file      Load 'config_file'; this option can be passed more than once; if it is not present, '" CONFIG_PATH "' will be used as default"_tr << endl;
			cout << endl << copyright << endl;
			throw 0;
		}
		else if (strcmp(argv[i], "-f") == 0) {
			if (i == argc - 1)
				throw "File name expected."_tr;
			i++;
			config_files.push_back(argv[i]);
		}
		else
			throw "Unknown option passed."_tr;
	}
	if (config_files.size() == 0)
		config_files.push_back(CONFIG_PATH);
}
