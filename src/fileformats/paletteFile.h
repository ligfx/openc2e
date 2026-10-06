#include "common/Image.h"
#include "common/shared_array.h"

#include <ghc/filesystem.hpp>

class Reader;

shared_array<Color> ReadPaletteFile(const ghc::filesystem::path& path);
shared_array<Color> ReadPaletteFile(Reader& in);