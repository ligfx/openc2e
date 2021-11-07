#include "common/Image.h"
#include "common/io/io.h"
#include "common/shared_array.h"

#include <string>

shared_array<Color> ReadPaletteFile(const std::string& path);
shared_array<Color> ReadPaletteFile(reader& in);