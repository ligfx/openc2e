#include "common/Image.h"
#include "common/shared_array.h"

#include <filesystem>

class Reader;

shared_array<Color> ReadPaletteFile(const std::filesystem::path& path);
shared_array<Color> ReadPaletteFile(Reader& in);