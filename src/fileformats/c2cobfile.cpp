/*
 *  c2cobfile.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Fri Jan 18 2008.
 *  Copyright (c) 2008 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include "c2cobfile.h"

#include "common/Exception.h"
#include "common/encoding.h"
#include "common/endianlove.h"

#include <algorithm>
#include <assert.h>
#include <ctype.h>
#include <string.h>

c2cobfile::c2cobfile(std::string _path)
	: path(_path) {
	try {
		file = filereader(path);
	} catch (io_error&) {
		throw Exception(std::string("couldn't open COB file \"") + path + "\"");
	}

	// TODO: c1 cob support
	uint8_t majic[4];
	file.read(majic, 4);
	if (memcmp(majic, "cob2", 4) != 0)
		throw Exception(std::string("bad magic of C2 COB file \"") + path + "\"");


	while (file.has_more_data()) {
		// TODO: catch exceptions, and free all blocks before passing it up the stack
		cobBlock* b = new cobBlock(this);
		blocks.push_back(b);
	}
}

c2cobfile::~c2cobfile() {
	for (auto& block : blocks) {
		delete block;
	}
}

cobBlock::cobBlock(c2cobfile* p) {
	seekablereader& file = p->getStream();

	uint8_t cobtype[4];
	file.read(cobtype, 4);
	if (!is_valid_ascii(cobtype, 4)) {
		// TODO: is CP1252 allowed?
		// TODO: debug representation, instead of lossy UTF-8?
		throw Exception("bad type of C2 COB block \"" + to_utf8_lossy(cobtype, 4) + "\"");
	}
	type = ascii_to_utf8(cobtype, 4);

	size = read32le(file);

	offset = file.tell();
	file.ignore(size);

	loaded = false;
	buffer = 0;
	parent = p;
}

cobBlock::~cobBlock() {
	if (loaded)
		free();
}

void cobBlock::load() {
	assert(!loaded);
	seekablereader& file = parent->getStream();

	try {
		file.seek(offset);
	} catch (io_error&) {
		throw Exception("Failed to seek to block offset.");
	}

	loaded = true;

	buffer = new uint8_t[size];
	try {
		file.read(buffer, size);
	} catch (io_error&) {
		free();
		throw Exception("Failed to read block.");
	}
}

void cobBlock::free() {
	assert(loaded);

	loaded = false;

	delete[] buffer;
	buffer = 0;
}

// TODO: argh, isn't there a better way to do this?
std::string readstring(reader& file) {
	return file.read_cp1252_until('\0');
}

cobAgentBlock::cobAgentBlock(cobBlock* p) {
	parent = p;
	seekablereader& file = p->getParent()->getStream();

	try {
		file.seek(p->getOffset());
	} catch (io_error&) {
		throw Exception("Failed to seek to block offset.");
	}

	quantityremaining = read16le(file);
	lastusage = read32le(file);
	reuseinterval = read32le(file);
	usebyday = read8(file);
	usebymonth = read8(file);
	usebyyear = read16le(file);

	file.ignore(12); // unused

	name = readstring(file);
	description = readstring(file);
	installscript = readstring(file);
	removescript = readstring(file);

	unsigned short noevents = read16le(file);
	for (unsigned int i = 0; i < noevents; i++) {
		scripts.push_back(readstring(file));
	}

	unsigned short nodeps = read16le(file);
	for (unsigned int i = 0; i < nodeps; i++) {
		unsigned short deptype = read16le(file);
		deptypes.push_back(deptype);

		// depnames should be read as lower-case to ease comparison
		std::string depname = readstring(file);
		std::transform(depname.begin(), depname.end(), depname.begin(), (int (*)(int))tolower);
		depnames.push_back(depname);
	}

	thumbnail.width = read16le(file);
	thumbnail.height = read16le(file);
	thumbnail.format = if_rgb565;
	thumbnail.data = shared_array<uint8_t>(2 * thumbnail.width * thumbnail.height);
	file.read(thumbnail.data.data(), 2 * thumbnail.width * thumbnail.height);
}

cobAgentBlock::~cobAgentBlock() = default;

cobFileBlock::cobFileBlock(cobBlock* p) {
	parent = p;
	seekablereader& file = p->getParent()->getStream();

	try {
		file.seek(p->getOffset());
	} catch (io_error&) {
		throw Exception("Failed to seek to block offset.");
	}

	filetype = read16le(file);
	file.ignore(4); // unused
	filesize = read32le(file);

	// filenames should be read as lower-case to ease comparison
	filename = readstring(file);
	std::transform(filename.begin(), filename.end(), filename.begin(), (int (*)(int))tolower);
}

cobFileBlock::~cobFileBlock() {
}

unsigned char* cobFileBlock::getFileContents() {
	if (!parent->isLoaded())
		parent->load();

	return parent->getBuffer() + 10 + filename.size() + 1;
}

cobAuthBlock::cobAuthBlock(cobBlock* p) {
	parent = p;
	seekablereader& file = p->getParent()->getStream();

	try {
		file.seek(p->getOffset());
	} catch (io_error&) {
		throw Exception("Failed to seek to block offset.");
	}

	daycreated = read8(file);
	monthcreated = read8(file);
	yearcreated = read16le(file);
	version = read8(file);
	revision = read8(file);
	authorname = readstring(file);
	authoremail = readstring(file);
	authorurl = readstring(file);
	authorcomments = readstring(file);
}

cobAuthBlock::~cobAuthBlock() {
}

/* vim: set noet: */
