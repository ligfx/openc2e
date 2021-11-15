#include "verticalbarFile.h"

#include "common/Exception.h"
#include "common/encoding.h"
#include "common/io/file.h"
#include "common/io/text.h"
#include "common/readfile.h"

std::vector<std::vector<std::string>> ReadVerticalBarSeparatedValuesFile(const std::string& path) {
	filereader in;
	try {
		in = filereader(path);
	} catch (io_error&) {
		throw Exception("Couldn't open " + path);
	}

	std::vector<std::vector<std::string>> lines;
	while (in.has_more_data()) {
		std::string line = read_cp1252_line(in);
		if (line.empty() || line[0] == '#') {
			continue;
		}

		std::vector<std::string> values;
		auto start = 0;
		auto end = line.find('|');
		while (true) {
			values.push_back(line.substr(start, end - start));
			if (end == std::string::npos)
				break;
			start = end + 1;
			end = line.find('|', start);
		}
		lines.push_back(values);
	}
	return lines;
}