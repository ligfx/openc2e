#pragma once

#include "common/io/io.h"

#include <ghc/filesystem.hpp>
#include <map>
#include <string>

std::map<std::string, std::string> readcfgfile(ghc::filesystem::path);
std::map<std::string, std::string> readcfgfile(reader& in);