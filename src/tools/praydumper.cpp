#include "prayfile/PrayFileReader.h"
#include <iostream>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <array>
#include <set>
#include <regex>

namespace fs = ghc::filesystem;

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::vector;

const std::set<std::string> tagblocks = {
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
	"DFAM", // DS starter family
	"DSGB", // garden box
};

const std::set<std::string> rawblocks = {
	"FILE", // inlined file
	"GLST", // inlined glist.creature file
	"CREA", // inlined .creature file
	"GENE", // inlined .genetics file
	"PHOT", // inlined .photo file
};

static bool is_tag_block(PrayFileReader &file, size_t i) {
	if (tagblocks.count(file.getBlockType(i))) {
		return true;
	}
	if (rawblocks.count(file.getBlockType(i))) {
		return false;
	}
	try {
		file.getBlockTags(i);
	} catch (...) {
		return false;
	}
	return true;
}

bool do_caos2pray(std::string filename_stem, PrayFileReader &file) {
	bool seen_agnt = false;
	bool seen_dsag = false;
	bool seen_dsgb = false;
	
	bool have_tags = false;
	std::map<std::string, uint32_t> int_tags;
	std::map<std::string, std::string> string_tags;
	
	// std::vector<std::string> block_names;
	std::map<std::string, std::string> block_names_by_type;
	
	std::vector<std::string> files;
	
	for (size_t i = 0; i < file.getNumBlocks(); ++i) {
		auto type = file.getBlockType(i);
		if (!(type == "AGNT" || type == "DSAG" || type == "DSGB" || type == "FILE")) {
			return false;
		}
		if (type == "FILE") {
			files.push_back(file.getBlockName(i));
			continue;
		}
		
		// block_names.push_back(file.getBlockName(i));
		block_names_by_type[file.getBlockType(i)] = file.getBlockName(i);
		
		auto tags = file.getBlockTags(i);
		if (have_tags) {
			if (!(int_tags == tags.first && string_tags == tags.second)) {
				return false;
			}
		} else {
			int_tags = tags.first;
			string_tags = tags.second;
		}
	}
	
	// size_t min_name_size = SIZE_MAX;
	// for (auto &s : block_names) {
	// 	min_name_size = std::min(s.size(), min_name_size);
	// }
	// 
	// size_t i = 0;
	// for (; i < min_name_size; ++i) {
	// 	char c = block_names[0][i];
	// 	bool matches = true;
	// 	for (size_t j = 1; j < block_names.size(); ++j) {
	// 		if (block_names[j][i] != c) {
	// 			matches = false;
	// 			break;
	// 		}
	// 	}
	// 	if (!matches) break;
	// }
	// std::string common_name = block_names[0].substr(0, i);
	// 
	// if (common_name.empty()) {
	// 	common_name = block_names[0];
	// 	common_name = common_name.substr(0, common_name.rfind(" C3"));
	// 	common_name = common_name.substr(0, common_name.rfind("-C3"));
	// 	common_name = common_name.substr(0, common_name.rfind("(C3)"));
	// 	common_name = common_name.substr(0, common_name.rfind("<C3>"));
	// 	common_name = common_name.substr(0, common_name.rfind(" DS"));
	// 	common_name = common_name.substr(0, common_name.rfind("-DS"));
	// 	common_name = common_name.substr(0, common_name.rfind("(DS)"));
	// 	common_name = common_name.substr(0, common_name.rfind("<DS>"));
	// 	common_name = common_name.substr(0, common_name.rfind(" GB"));
	// 	common_name = common_name.substr(0, common_name.rfind("-GB"));
	// 	common_name = common_name.substr(0, common_name.rfind("(GB)"));
	// 	common_name = common_name.substr(0, common_name.rfind("<GB>"));
	// 	while (common_name.back() == ' ' || common_name.back() == '-') {
	// 		common_name = common_name.substr(0, common_name.size() - 1);
	// 	}
	// }
	
	printf("* CAOS2PRAY\n");
	for (auto &kv : block_names_by_type) {
		std::string type = kv.first.c_str();
		if (type == "AGNT") type = "C3";
		if (type == "DSAG") type = "DS";
		printf("*# %s-Name \"%s\"\n", type.c_str(), kv.second.c_str());
	}
	
	for (auto &kv : int_tags) {
		// if (kv.first == "Script Count") continue;
		// if (kv.first == "Dependency Count") continue;
		if (std::regex_search(kv.first, std::regex("^(Dependency Count|Script Count|Dependency Category \\d+)"))) continue;
		printf("*# %s = %i\n", kv.first.c_str(), kv.second);
	}
	
	std::vector<std::string> dependencies;
	std::vector<std::string> scripts;
	std::string remove_script;
	
	for (auto &kv : string_tags) {
		if (kv.first == "Remove script") {
			remove_script = kv.second;
			continue;
		}
		if (std::regex_search(kv.first, std::regex("^Script \\d+"))) {
			scripts.push_back(kv.second);
			continue;
		}
		if (std::regex_search(kv.first, std::regex("^Dependency \\d+"))) {
			dependencies.push_back(kv.second);
			continue;
		}
		// TODO: escape characters
		std::string value = kv.second.c_str();
		while (value.find("\n") != std::string::npos) {
			value = value.replace(value.find("\n"), 1, "\\n");
		}
		printf("*# %s = \"%s\"\n", kv.first.c_str(), value.c_str());
	}
	
	std::set<std::string> files_set(files.begin(), files.end());
	std::set<std::string> dependencies_set(dependencies.begin(), dependencies.end());
	
	for (auto &d : dependencies) {
		if (files_set.count(d)) {
			printf("*# attach \"%s\"\n", d.c_str());
		} else {
			printf("*# depend \"%s\"\n", d.c_str());
		}
	}
	for (auto &f : files) {
		if (!dependencies_set.count(f)) {
			printf("*# inline \"%s\"\n", f.c_str());
		}
	}
	
	printf("\n");
	if (scripts.size() == 1) {
		printf("%s\n", scripts[0].c_str());
	} else {
		for (size_t i = 0; i < scripts.size(); ++i) {
			std::string script_name = filename_stem + " - script " + std::to_string(i + 1) + ".cos";
			printf("*# link \"%s\"\n", script_name.c_str());
		}
	}
	
	if (!remove_script.empty()) {
		// TODO: if rscr already exists, append this to it
		// e.g. aquatilis pod, aquatilis caverna, etc.
		printf("\n");
		printf("rscr\n");
		printf("%s\n", remove_script.c_str());
	}
	
	
	
	// printf("common name: '%s'\n", common_name.c_str());
	// 
	// printf("i %i\n", i);
	// printf("%s\n", block_names[0].substr(0, i).c_str());
	// 
	// for (auto s : block_names) {
	// 	printf("  %s\n", s.c_str());
	// }
	
	return true;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		cerr << "syntax: praydumper filename" << endl;
		return 1;
	}

	fs::path inputfile = fs::path(argv[1]);
	if (!fs::exists(inputfile)) {
		cerr << "input file doesn't exist!" << endl;
		return 1;
	}

	fs::path output_directory = inputfile.stem();
	if (fs::exists(output_directory)) {
		cerr << "Output directory " << output_directory << " already exists" << endl;
		exit(1);
	}
	if (!fs::create_directory(output_directory)) {
		cerr << "Couldn't create output directory " << output_directory << endl;
		exit(1);
	}

	std::string pray_source_filename = (output_directory / inputfile.stem()).string() + ".txt";
	std::ofstream pray_source(pray_source_filename);
	cout << "Writing \"" << pray_source_filename << "\"" << endl;
	pray_source << "(- praydumper-generated PRAY file from " << fs::path(argv[1]).filename() << " -)" << endl;
	pray_source << endl << "\"en-GB\"" << endl;

	std::ifstream in(inputfile.string(), std::ios::binary);
	if (!in) {
		cerr << "Error opening file " << inputfile << endl;
		exit(1);
	}

	PrayFileReader file(in);

	do_caos2pray(inputfile.stem(), file);

	for (size_t i = 0; i < file.getNumBlocks(); i++) {
		// TODO: s/"/\\"/ in the data (use find/replace methods of string)
		
		if (is_tag_block(file, i)) {
			pray_source << endl << "group " << file.getBlockType(i) << " \"" << file.getBlockName(i) << "\"" << endl;

			auto tags = file.getBlockTags(i);
			auto int_tags = tags.first;
			auto string_tags = tags.second;

			for (auto y : int_tags) {
				pray_source << "\"" << y.first << "\" " << y.second << endl;
			}
			
			for (auto y : string_tags) {
				std::string name = y.first;
				if ((name.substr(0, 7) ==  "Script " || name == "Remove script") && y.second.size() > 100) {
					name = file.getBlockName(i) + " - " + name + ".cos";
					cout << "Writing " << (output_directory / name) << endl;
					ofstream output(output_directory / name);
					output.write(y.second.c_str(), y.second.size());
					pray_source << "\"" << y.first << "\" @ \"" << name << "\"" << endl;
				} else {
					pray_source << "\"" << y.first << "\" \"" << y.second << "\"" << endl;
				}
			}
		} else {
			pray_source << endl << "inline " << file.getBlockType(i) << " \"" << file.getBlockName(i) << "\" \"" << file.getBlockName(i) << "\"" << endl;
			cout << "Writing " << (output_directory / file.getBlockName(i)) << endl;
			ofstream output(output_directory / file.getBlockName(i));
			auto buf = file.getBlockRawData(i);
			output.write((char *)buf.data(), buf.size());
		}
	}
}

