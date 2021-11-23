#pragma once

#include "common/io/io.h"

#include <string>
#include <vector>

float read_text_float(bufferedreader&);
int read_text_int(bufferedreader&);

std::vector<uint8_t> read_until(reader&, uint8_t delimiter);

std::string read_ascii_line(reader&);
std::string read_ascii_string(reader&, size_t);

std::string read_cp1252_line(reader&);
std::string read_cp1252_string(reader&, size_t);
