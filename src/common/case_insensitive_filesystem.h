#pragma once

#include <ghc/filesystem.hpp>
#include <system_error>
#include <unordered_map>

class FileWriter;

namespace case_insensitive_filesystem {

namespace fs = ghc::filesystem;

struct cacheinfo {
	fs::path realfilename;
	fs::file_time_type mtime = fs::file_time_type::min();
};

fs::path canonical(const fs::path&, std::error_code&);
bool exists(const fs::path&);
FileWriter create_file(const fs::path&);

class directory_iterator {
  public:
	directory_iterator(const fs::path& dirname);
	const fs::path& operator*() const;
	directory_iterator& operator++();
	bool operator==(const directory_iterator&) const;
	bool operator!=(const directory_iterator&) const;
	directory_iterator begin();
	directory_iterator end() const;

  private:
	directory_iterator();
	fs::path lcdirname;
	std::unordered_map<fs::path, cacheinfo>::iterator it;
};

} // namespace case_insensitive_filesystem