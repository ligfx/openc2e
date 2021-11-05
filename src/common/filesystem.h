#pragma once

#include "path.h"

#include <fmt/core.h>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#error not implemented
#else
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#endif

class filesystem_error : public std::runtime_error {
  public:
	using runtime_error::runtime_error;
};

namespace filesystem {
bool exists(const std::string& filename);
bool is_directory(const std::string& filename);

class directory_entry {
  public:
	directory_entry(const path& path)
		: m_path(path) {}
	const path& path() const { return m_path; }

  private:
	::path m_path;
};

class directory_iterator {
	class view;

  public:
	directory_iterator() = default;
	directory_iterator(const path& dirname) {
#ifdef _WIN32
#error not implemented
#else
		while ((m_dir = opendir(reinterpret_cast<const char*>(dirname.as_bytes()))) == nullptr && errno == EINTR)
			;
		if (!m_dir) {
			throw filesystem_error(fmt::format("Error opening directory '{}', errno={}", dirname.to_string_lossy(), errno));
		}

		// get first directory entry
		++(*this);

#endif
	}
	directory_iterator(const directory_iterator&) = delete;
	directory_iterator(directory_iterator&& other) {
		m_dir = other.m_dir;
		m_dirent = other.m_dirent;
		other.m_dir = nullptr;
		other.m_dirent = nullptr;
	}
	directory_iterator& operator=(const directory_iterator&) = delete;
	directory_iterator& operator=(directory_iterator&& other) {
		if (m_dir) {
			closedir(m_dir); // ignore errors?
		}
		m_dir = other.m_dir;
		m_dirent = other.m_dirent;
		other.m_dir = nullptr;
		other.m_dirent = nullptr;
		return *this;
	}
	~directory_iterator() {
		if (m_dir) {
			closedir(m_dir); // ignore errors?
		}
	}
	directory_entry operator*() const {
		if (!m_dirent) {
			throw std::runtime_error("Dereferencing invalid directory_iterator");
		}
		return directory_entry(path(m_dirent->d_name));
	}
	directory_iterator& operator++() {
		if (!m_dir) {
			return *this;
		}

		while (true) {
			errno = 0;
			m_dirent = readdir(m_dir);
			if (m_dirent == nullptr) {
				if (errno == EINTR) {
					continue;
				} else if (errno != 0) {
					throw filesystem_error(fmt::format("Error in readdir, errno={}", errno));
				} else {
					closedir(m_dir);
					m_dir = nullptr;
					break;
				}
			} else if (strcmp(m_dirent->d_name, ".") == 0 || strcmp(m_dirent->d_name, "..") == 0) {
				continue;
			} else {
				break;
			}
		}

		return *this;
	}
	bool operator==(const directory_iterator& other) const {
		if (m_dir == nullptr && other.m_dir == nullptr) {
			return true;
		}
		return false;
	}
	bool operator!=(const directory_iterator& other) const {
		return !(*this == other);
	}
	view begin() {
		return view(*this);
	}
	view end() const {
		static directory_iterator empty_iterator;
		return view(empty_iterator);
	}

  private:
#ifdef _WIN32
#error not implemented
#else
	DIR* m_dir = nullptr;
	dirent* m_dirent = nullptr;
#endif

	class view final {
	  public:
		view(directory_iterator& it)
			: m_it(it) {}
		auto operator*() const {
			return *m_it;
		}
		view& operator++() {
			++m_it;
			return *this;
		}
		bool operator==(const view& other) const {
			return m_it == other.m_it;
		}
		bool operator!=(const view& other) const {
			return !(*this == other);
		}

	  private:
		directory_iterator& m_it;
	};

}; // namespace filesystem
} // namespace filesystem