#pragma once

#include <iosfwd>
#include <string>
#include <vector>

std::string readfile(const std::string &filename);
std::string readfile(std::istream &i);

std::vector<unsigned char> readfile_binary(const std::string &filename);