#pragma once

#include "common/Ascii.h"

#include <ghc/filesystem.hpp>

inline ghc::filesystem::path with_extension(ghc::filesystem::path p, const ghc::filesystem::path& ext) {
	p.replace_extension(ext);
	return p;
}