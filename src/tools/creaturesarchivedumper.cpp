#include "common/endianlove.h"
#include "common/io/file.h"
#include "common/io/spanreader.h"

#include <fmt/format.h>
#include <ghc/filesystem.hpp>
#include <iostream>
#include <zlib.h>

namespace fs = ghc::filesystem;

static const std::string CREATURES_ARCHIVE_MAGIC = "Creatures Evolution Engine - Archived information file. zLib 1.13 compressed.";

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "syntax: creaturesarchivedumper filename" << std::endl;
		exit(1);
	}

	fs::path input_path(argv[1]);
	if (!fs::exists(input_path)) {
		std::cerr << "File " << input_path << " doesn't exist" << std::endl;
		exit(1);
	}

	filereader in(input_path);
	auto data = in.read_to_end();
	fmt::print("data size = {}\n", data.size());

	spanreader s(data);
	std::string magic;
	magic.resize(CREATURES_ARCHIVE_MAGIC.size());
	s.read(reinterpret_cast<uint8_t*>(&magic[0]), magic.size());


	if (magic != CREATURES_ARCHIVE_MAGIC) {
		fmt::print(stderr, "Invalid magic - got \"{}\"\n", magic);
		exit(1);
	}

	fmt::print("magic = \"{}\"\n", magic);
	fmt::print("magic size = {}\n", magic.size());

	uint8_t sub = read8(s);
	uint8_t eot = read8(s);
	fmt::print("sub = 0x{:02x}\n", sub);
	fmt::print("eot = 0x{:02x}\n", eot);
	if (sub != 0x1a || eot != 0x04) {
		if (sub != 0x1a) {
			fmt::print(stderr, "Expected SUB (0x1a), got {:#x}\n", sub);
			exit(1);
		}
		if (eot != 0x04) {
			fmt::print(stderr, "Expected EOT (0x04), got {:#x}\n", sub);
			exit(1);
		}
	}

	fmt::print("stream position = {}\n", s.tell());

	std::vector<uint8_t> decompressed_data(data.size() * 20); // TODO: ???
	uLongf usize = decompressed_data.size();
	int r = uncompress((Bytef*)decompressed_data.data(), (uLongf*)&usize, (Bytef*)data.data() + s.tell(), data.size() - s.tell());
	if (r != Z_OK) {
		std::string o;
		switch (r) {
			case Z_MEM_ERROR: o = "Out of memory"; break;
			case Z_BUF_ERROR: o = "Out of buffer space"; break;
			case Z_DATA_ERROR: o = "Corrupt data"; break;
			default: o = "Unknown error"; break;
		}
		o = o + " while decompressing";
		fmt::print(stderr, "{}\n", o);
		exit(1);
	}

	fmt::print("decompressed data size = {}\n", usize);

	fs::path output_filename = input_path.filename().string() + ".out";
	fmt::print("writing to {}\n", output_filename.string());
	std::ofstream out(output_filename, std::ios::binary);
	out.write((char*)decompressed_data.data(), usize);
}