#pragma once

#include <cstdint>
#include <ghc/filesystem.hpp>

class Reader;

struct hedfile {
	uint32_t frame_width;
	uint32_t frame_height;
	uint32_t numframes;
};

hedfile read_hedfile(const ghc::filesystem::path& path);
hedfile read_hedfile(Reader& in);