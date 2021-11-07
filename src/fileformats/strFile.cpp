#include "strFile.h"

#include "common/Exception.h"
#include "common/encoding.h"
#include "common/endianlove.h"
#include "common/io/file.h"

std::vector<std::string> ReadStrFile(const std::string& path) {
	filereader in;
	try {
		in = filereader(path);
	} catch (io_error&) {
		throw Exception("Couldn't open " + path);
	}

	int num_strings = read16le(in);
	std::vector<std::string> strings(num_strings);
	for (int i = 0; i < num_strings; i++) {
		int len = read8(in);
		if (len == 0)
			continue;

		strings[i] = cp1252_to_utf8(in.read_vector(len));
	}
	return strings;
}