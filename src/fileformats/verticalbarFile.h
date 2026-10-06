#pragma once

#include <filesystem>
#include <string>
#include <vector>

std::vector<std::vector<std::string>> ReadVerticalBarSeparatedValuesFile(const std::filesystem::path& path);