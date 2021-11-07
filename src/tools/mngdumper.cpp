#include "common/endianlove.h"
#include "common/enumerate.h"
#include "common/io/file.h"
#include "common/zip.h"
#include "fileformats/mngfile.h"

#include <fmt/format.h>
#include <ghc/filesystem.hpp>
#include <iostream>
#include <utility>

namespace fs = ghc::filesystem;

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "syntax: mngdumper filename" << std::endl;
		exit(1);
	}

	fs::path input_path(argv[1]);
	if (!fs::exists(input_path)) {
		std::cerr << "File " << input_path << " doesn't exist" << std::endl;
		exit(1);
	}

	fs::path stem = input_path.stem();
	fs::path output_directory = stem;

	if (!fs::create_directories(output_directory)) {
		if (!fs::is_directory(output_directory)) {
			std::cerr << "Couldn't create output directory " << output_directory << std::endl;
			exit(1);
		}
	}

	MNGFile file(argv[1]);

	{
		fs::path script_filename((output_directory / stem).native() + ".txt");
		fmt::print("{}\n", script_filename.string());
		filewriter script(script_filename);
		script.write_str(file.script);
	}

	for (auto kv : zip(file.getSampleNames(), file.samples)) {
		fs::path sample_filename((output_directory / kv.first).native() + ".wav");
		fmt::print("{}\n", sample_filename.string());

		filewriter out((output_directory / kv.first).native() + ".wav");
		out.write(kv.second.data(), kv.second.size());
	}
}