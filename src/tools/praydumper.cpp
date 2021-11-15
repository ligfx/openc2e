#include "common/endianlove.h"
#include "common/io/file.h"
#include "common/io/io.h"
#include "common/io/spanreader.h"
#include "common/io/text.h"
#include "common/optional.h"
#include "common/span.h"
#include "fileformats/PrayFileReader.h"

#include <array>
#include <ghc/filesystem.hpp>
#include <iostream>

namespace fs = ghc::filesystem;

const std::array<std::string, 11> tagblocks = {
	"AGNT", // C3 agent
	"DSAG", // DS agent
	"MACH", // SM agent
	"HAND", // SM agent
	"LIVE", // SM agent
	"MONK", // SM agent
	"EXPC", // C3 creature info
	"DSEX", // DS creature info
	"SFAM", // C3 starter family
	"EGGS", // eggs
	"DFAM" // DS starter family
};

static bool is_printable(const std::string& s) {
	for (auto c : s) {
		if (static_cast<unsigned char>(c) < 32) {
			return false;
		}
	}
	return true;
}

optional<PrayTagBlock> get_block_as_tags(PrayFileReader& file, int i) {
	for (auto tagblock : tagblocks) {
		if (file.getBlockType(i) == tagblock) {
			return file.getBlockTags(i);
		}
	}
	if (file.getBlockType(i) == "FILE") {
		return {};
	}
	auto buf = file.getBlockRawData(i);
	// maybe it's a tag block anyways?
	spanreader s(buf);
	try {
		unsigned int nointvalues = read32le(s);

		std::map<std::string, unsigned int> integerValues;
		for (unsigned int i = 0; i < nointvalues; i++) {
			unsigned int keylength = read32le(s);
			if (keylength > 256) {
				return {};
			}

			std::string key = read_cp1252_string(s, keylength);
			if (!is_printable(key)) {
				return {};
			}

			unsigned int value = read32le(s);
			integerValues[key] = value;
		}

		unsigned int nostrvalues = read32le(s);
		std::map<std::string, std::string> stringValues;
		for (unsigned int i = 0; i < nostrvalues; i++) {
			unsigned int keylength = read32le(s);

			std::string key = read_cp1252_string(s, keylength);
			if (!is_printable(key)) {
				return {};
			}

			unsigned int valuelength = read32le(s);
			std::string value = read_cp1252_string(s, valuelength);
			if (!is_printable(value)) {
				return {};
			}

			stringValues[key] = value;
		}

		// s.peek();
		// if (!s.eof()) {
		// 	return {};
		// }

		std::cerr << "Unknown block type '" << file.getBlockType(i) << "' looks like a tag block" << std::endl;
		return PrayTagBlock(integerValues, stringValues);
	} catch (io_error&) {
		return {};
	}
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "syntax: praydumper filename" << std::endl;
		return 1;
	}

	fs::path inputfile = fs::path(argv[1]);
	if (!fs::exists(inputfile)) {
		std::cerr << "input file doesn't exist!" << std::endl;
		return 1;
	}

	fs::path output_directory = inputfile.stem();
	if (fs::exists(output_directory)) {
		std::cerr << "Output directory " << output_directory << " already exists" << std::endl;
		exit(1);
	}
	if (!fs::create_directory(output_directory)) {
		std::cerr << "Couldn't create output directory " << output_directory << std::endl;
		exit(1);
	}

	std::string pray_source_filename = (output_directory / inputfile.stem()).string() + ".txt";
	std::ofstream pray_source(pray_source_filename);
	std::cout << "Writing \"" << pray_source_filename << "\"" << std::endl;
	pray_source << "(- praydumper-generated PRAY file from '" << inputfile.filename().string() << "' -)" << std::endl;
	pray_source << std::endl
				<< "\"en-GB\"" << std::endl;

	filereader in;
	try {
		in = filereader(inputfile);
	} catch (io_error&) {
		std::cerr << "Error opening file " << inputfile << std::endl;
		exit(1);
	}

	PrayFileReader file(in);

	for (size_t i = 0; i < file.getNumBlocks(); i++) {
		// TODO: s/"/\\"/ in the data (use find/replace methods of string)

		auto tags = get_block_as_tags(file, i);
		if (tags) {
			pray_source << std::endl
						<< "group " << file.getBlockType(i) << " \"" << file.getBlockName(i) << "\"" << std::endl;

			auto int_tags = tags->first;
			auto string_tags = tags->second;

			for (auto y : int_tags) {
				pray_source << "\"" << y.first << "\" " << y.second << std::endl;
			}

			for (auto y : string_tags) {
				std::string name = y.first;
				if ((name.substr(0, 7) == "Script ") || (name.substr(0, 13) == "Remove script")) {
					name = file.getBlockName(i) + " - " + name + ".cos";
					std::cout << "Writing " << (output_directory / name) << std::endl;
					filewriter output(output_directory / name);
					output.write(y.second.c_str(), y.second.size());
					pray_source << "\"" << y.first << "\" @ \"" << name << "\"" << std::endl;
				} else {
					pray_source << "\"" << y.first << "\" \"" << y.second << "\"" << std::endl;
				}
			}
		} else {
			pray_source << std::endl
						<< "inline " << file.getBlockType(i) << " \"" << file.getBlockName(i) << "\" \"" << file.getBlockName(i) << "\"" << std::endl;
			std::cout << "Writing " << (output_directory / file.getBlockName(i)) << std::endl;
			filewriter output(output_directory / file.getBlockName(i));
			auto buf = file.getBlockRawData(i);
			output.write(buf.data(), buf.size());
		}
	}
}
