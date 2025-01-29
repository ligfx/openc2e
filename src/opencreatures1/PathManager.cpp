#include "PathManager.h"

#include "common/Exception.h"
#include "common/case_insensitive_filesystem.h"
#include "common/io/FileWriter.h"
#include "common/wildcard_match.h"

namespace fs = ghc::filesystem;

PathManager::PathManager() {
}

void PathManager::set_main_directory(fs::path main_dir) {
	m_main_dir = main_dir;
}

static fs::path append_path_type(fs::path main_dir, PathType type) {
	switch (type) {
		case PATH_TYPE_MAIN:
			return main_dir;
		case PATH_TYPE_IMAGE:
			return main_dir / "Images";
		case PATH_TYPE_SOUND:
			return main_dir / "Sounds";
	}
}

fs::path PathManager::find_path(PathType type, const std::string& name) {
	if (m_main_dir.empty()) {
		throw Exception("PathManager main_dir is not set!");
	}
	auto dirname = append_path_type(m_main_dir, type);
	std::error_code err;
	// ignore error, just return empty path
	return case_insensitive_filesystem::canonical(dirname / name, err);
}

std::vector<fs::path> PathManager::find_path_wildcard(PathType type, const std::string& pattern) {
	if (m_main_dir.empty()) {
		throw Exception("PathManager main_dir is not set!");
	}
	auto dirname = append_path_type(m_main_dir, type);

	// assume pattern is a single filename, cannot have directory separators
	std::vector<fs::path> results;
	for (const auto& entry : case_insensitive_filesystem::directory_iterator(dirname)) {
		if (wildcard_match_ignore_case(pattern, entry.filename().string())) {
			results.push_back(entry.filename());
		}
	}

	return results;
}

FileWriter PathManager::create_file(PathType type, const std::string& name) {
	auto dirname = append_path_type(m_main_dir, type);
	return case_insensitive_filesystem::create_file(dirname / name);
}