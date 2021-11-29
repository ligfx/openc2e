#include "paletteFile.h"

#include "common/io/file.h"

#include <array>

shared_array<Color> ReadPaletteFile(const std::string& path) {
	filereader in(path);
	return ReadPaletteFile(in);
}

shared_array<Color> ReadPaletteFile(reader& in) {
	std::array<uint8_t, 768> palette_data;
	in.read(palette_data.data(), palette_data.size());

	shared_array<Color> palette(256);
	for (unsigned int i = 0; i < 256; i++) {
		palette[i].r = palette_data[i * 3] * 4;
		palette[i].g = palette_data[i * 3 + 1] * 4;
		palette[i].b = palette_data[i * 3 + 2] * 4;
		palette[i].a = 0xff;
	}
	return palette;
}