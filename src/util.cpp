#include "util.h"

#include <fstream>
#include <stdexcept>

std::string readfile(const std::string &filename) {
	std::ifstream in(filename);
	if (!in) {
		throw std::runtime_error("Couldn't open \"" + filename + "\"");
	}
	in.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	return readfile(in);
}

std::string readfile(std::istream &in) {
    return std::string(std::istreambuf_iterator<char>(in), {});
}

std::vector<unsigned char> readfile_binary(const std::string &filename) {
	std::ifstream in(filename);
	if (!in) {
		throw std::runtime_error("Couldn't open \"" + filename + "\"");
	}
	in.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	return std::vector<unsigned char>(std::istreambuf_iterator<char>(in), {});
}
