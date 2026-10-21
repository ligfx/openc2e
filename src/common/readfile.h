#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

class Reader;

std::string readfile(const std::filesystem::path& filename);
std::string readfile(Reader& in);
std::vector<uint8_t> readfilebinary(const std::filesystem::path& filename);
