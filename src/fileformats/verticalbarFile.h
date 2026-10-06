#pragma once

#include <ghc/filesystem.hpp>
#include <string>
#include <vector>

std::vector<std::vector<std::string>> ReadVerticalBarSeparatedValuesFile(const ghc::filesystem::path& path);