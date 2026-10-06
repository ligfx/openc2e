#pragma once

#include <ghc/filesystem.hpp>
#include <string>
#include <vector>

std::vector<std::string> ReadStrFile(const ghc::filesystem::path& path);