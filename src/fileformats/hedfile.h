#pragma once


#include "common/io/io.h"

#include <cstdint>
#include <string>
#include <vector>

struct hedfile {
	uint32_t frame_width;
	uint32_t frame_height;
	uint32_t numframes;
};

hedfile read_hedfile(const std::string& path);
hedfile read_hedfile(bufferedreader& in);