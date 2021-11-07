#pragma once

#include "common/Image.h"
#include "common/io/io.h"

#include <string>

void WritePngFile(const Image& image, const std::string& path);
void WritePngFile(const Image& image, writer& out);
