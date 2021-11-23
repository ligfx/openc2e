#pragma once

#include "common/io/io.h"

#include <string>
#include <vector>

std::string readfile(const std::string& filename);
std::string readfile(reader& in);
std::vector<uint8_t> readfilebinary(const std::string& filename);
std::vector<uint8_t> readfilebinary(reader& in);
