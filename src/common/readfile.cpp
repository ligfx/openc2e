#include "readfile.h"

#include "common/io/file.h"

std::string readfile(const std::string& filename) {
	filereader in(filename);
	return readfile(in);
}

std::string readfile(reader& in) {
	std::vector<uint8_t> buf = in.read_to_end();
	return std::string(reinterpret_cast<char*>(buf.data()), buf.size());
}

std::vector<uint8_t> readfilebinary(const std::string& filename) {
	filereader in(filename);
	return readfilebinary(in);
}

std::vector<uint8_t> readfilebinary(reader& in) {
	return in.read_to_end();
}
