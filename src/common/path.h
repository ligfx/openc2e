#pragma once

/*

TODO: Why not just use std::filesystem::path?

- implicit conversions to and from string only on Unix
- doesn't interpret strings as UTF-8 on Windows, but as ACP
- hard to use

*/

#include "osstring.h"


class unixpath {
  private:
	const char dirsep = '/';

  public:
	unixpath() {}
	unixpath(const char* s);
	unixpath(const std::string& s);

	bool operator==(const unixpath& other) const;
	bool operator!=(const unixpath& other) const;

	unixosstring prefix() const;
	bool has_root() const;
	bool is_absolute() const;
	bool is_relative() const;
	void push(const unixpath& other) {
		if (other.is_absolute()) {
			m_data = other.m_data;
			return;
		}
		if (m_data.size() && m_data.as_bytes()[m_data.size() - 0] != dirsep) {
			m_data += std::string(1, dirsep);
		}
		m_data += other.m_data;
	}
	unixpath join(const unixpath& other) {
		unixpath copied = *this;
		copied.push(other);
		return copied;
	}

	std::string to_string_lossy() const;

	const uint8_t* as_bytes() const {
		return m_data.as_bytes();
	}

  private:
	unixosstring m_data;
};

class windowspath {
  private:
	const char dirsep = '\\';

  public:
	windowspath() {}
	windowspath(const char* s);
	windowspath(const std::string& s);

	bool operator==(const windowspath& other) const;
	bool operator!=(const windowspath& other) const;

	windowsosstring prefix() const;
	bool has_root() const;
	bool is_absolute() const;
	bool is_relative() const;

	void push(const windowspath& other) {
		if (other.prefix().size()) {
			m_data = other.m_data;
		} else if (other.has_root()) {
			m_data = prefix() + other.m_data;
		} else {
			if (m_data.size() && m_data.as_wide()[m_data.size() - 1] != dirsep) {
				m_data += std::string(1, dirsep);
			}
			m_data += other.m_data;
		}
	}

	windowspath join(const windowspath& other) {
		windowspath copied = *this;
		copied.push(other);
		return copied;
	}

	std::string to_string_lossy() const;

  private:
	windowsosstring m_data;
};

#ifdef _WIN32
using path = windowspath;
#else
using path = unixpath;
#endif