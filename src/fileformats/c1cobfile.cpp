#include "c1cobfile.h"

#include "c1defaultpalette.h"
#include "common/endianlove.h"
#include "common/io/file.h"
#include "common/throw_ifnot.h"

#include <cassert>

static std::string read_string(reader& in) {
	uint16_t length = read8(in);
	if (length == 255) {
		length = read16le(in);
	}
	std::vector<uint8_t> script(length);
	in.read(script.data(), length);
	return std::string(reinterpret_cast<char*>(script.data()), script.size());
}

c1cobfile read_c1cobfile(const std::string& path) {
	filereader in(path);
	return read_c1cobfile(in);
}

c1cobfile read_c1cobfile(reader& in) {
	c1cobfile cob;

	uint16_t version = read16le(in);
	THROW_IFNOT(version == 1);

	cob.quantity_available = read16le(in);
	cob.expiration_month = read32le(in);
	cob.expiration_day = read32le(in);
	cob.expiration_year = read32le(in);

	uint16_t num_object_scripts = read16le(in);
	uint16_t num_install_scripts = read16le(in);

	cob.quantity_used = read32le(in);

	for (size_t i = 0; i < num_object_scripts; ++i) {
		cob.object_scripts.push_back(read_string(in));
	}
	for (size_t i = 0; i < num_install_scripts; ++i) {
		cob.install_scripts.push_back(read_string(in));
	}

	cob.picture.width = read32le(in);
	cob.picture.height = read32le(in);
	cob.picture.format = if_index8;
	cob.picture.palette = getCreatures1DefaultPalette();
	uint16_t unknown_always_picture_width = read16le(in);
	// ABK- Egg Gender.cob has it zeroed
	THROW_IFNOT(unknown_always_picture_width == 0 || unknown_always_picture_width == cob.picture.width);

	if (cob.picture.width > 0 && cob.picture.height > 0) {
		cob.picture.data = shared_array<uint8_t>(cob.picture.width * cob.picture.height);
		for (size_t i = 0; i < cob.picture.height; ++i) {
			in.read(cob.picture.data.data() + cob.picture.width * (cob.picture.height - 1 - i), cob.picture.width);
		}
	}

	cob.name = read_string(in);

	return cob;
}