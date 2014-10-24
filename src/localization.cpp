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
#include "unicode.hpp"
#include "path.hpp"
#ifdef WIN32
#include <windows.h>
#endif

namespace localization {

static HashTable<const char *, HASH_TABLE_LOCALIZATION_BUCKETS> translations;
static char * translation_strings = 0;

static void loadstrings (const string domain, const string locale_dir, string & localename) {
	size_t pos = localename.find('-');
	if (pos != string::npos)
		localename.resize(pos);
	pos = localename.find('_');
	if (pos != string::npos)
		localename.resize(pos);
	pos = localename.find('.');
	if (pos != string::npos)
		localename.resize(pos);
	path mopath(locale_dir);
	mopath.append(localename);
	mopath.append("LC_MESSAGES");
	mopath.append(domain + ".mo");
	DEBUG_PRINT_2("Looking for localization *.mo file: " << mopath.serialize());
	ifstream mo(mopath.serialize(), ifstream::in | ifstream::binary);
	if (!mo.good()) {
		LOG_ERROR("Can't open localization file: " << mopath.serialize());
		return;
	}
	uint32_t offset_orig, offset_tr, size = 0;
	mo.read(reinterpret_cast<char *>(&offset_tr), 4);
	if (offset_tr != 0x950412de)
		LOG_CRITICAL("Invalid .mo file format");
	mo.read(reinterpret_cast<char *>(&offset_tr), 4);
	mo.read(reinterpret_cast<char *>(&size), 4);
	if (size == 0)
		return;
	mo.read(reinterpret_cast<char *>(&offset_orig), 4);
	mo.read(reinterpret_cast<char *>(&offset_tr), 4);
	mo.seekg(offset_orig, mo.beg);
	uint32_t orig[size][2];
	for (pos = 0; pos < size; ++pos) {
		mo.read(reinterpret_cast<char *>(&orig[pos][0]), 4);
		mo.read(reinterpret_cast<char *>(&orig[pos][1]), 4);
	}
	mo.seekg(offset_tr, mo.beg);
	uint32_t tr[size][2];
	size_t total_size = size;
	for (pos = 0; pos < size; ++pos) {
		mo.read(reinterpret_cast<char *>(&tr[pos][0]), 4);
		mo.read(reinterpret_cast<char *>(&tr[pos][1]), 4);
		total_size += tr[pos][0];
	}
	translation_strings = new char[total_size];
	mo.seekg(tr[0][1], mo.beg);
	mo.read(translation_strings, total_size);
	mo.seekg(orig[0][1], mo.beg);
	for (pos = 0; pos < size; ++pos) {
		char tmp[orig[pos][0] + 1];
		mo.read(tmp, orig[pos][0] + 1);
		translations[tmp] = translation_strings + tr[pos][1] - tr[0][1];
	}
}

void setup_locale () {
#ifdef WIN32
	wchar_t localebuf[LOCALE_NAME_MAX_LENGTH];
	GetUserDefaultLocaleName(localebuf, LOCALE_NAME_MAX_LENGTH);
	wstring wlocalename = localebuf;
	string localename = unicode::utf16_to_utf8(wlocalename);
	cout.rdbuf(&unicode::win_console_cout);
	cerr.rdbuf(&unicode::win_console_cerr);
#else
	locale default_locale("");
	string localename = default_locale.name();
	default_locale = default_locale.combine<num_put<char>>(locale::classic());
	default_locale = default_locale.combine<numpunct<char>>(locale::classic());
	locale::global(default_locale);
	cout.imbue(default_locale);
	cerr.imbue(default_locale);
	cin.imbue(default_locale);
#endif
	DEBUG_PRINT_1("System locale: " << localename);
	loadstrings(GETTEXT_DOMAIN, LOCALE_DIR, localename);
}

const char * translate (const char * from) {
	const char * tr = translations.find(from);
	return (tr) ? tr : from;
}

const char * translate (const uint64_t hash) {
	return translations.find(hash);
}

};
