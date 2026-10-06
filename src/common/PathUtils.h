#pragma once

#include "common/Ascii.h"

#include <filesystem>

inline std::filesystem::path with_extension(std::filesystem::path p, const std::filesystem::path& ext) {
	p.replace_extension(ext);
	return p;
}