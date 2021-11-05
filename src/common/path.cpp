#include "path.h"

#include "encoding.h"

static bool is_ascii_letter(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

unixpath::unixpath(const char* s)
	: unixpath(std::string(s)) {
}

unixpath::unixpath(const std::string& s)
	: m_data(s) {
	// TODO: any sort of checking? hopefully it's UTF-8
}

bool unixpath::operator==(const unixpath& other) const {
	return m_data == other.m_data;
}
bool unixpath::operator!=(const unixpath& other) const {
	return !(*this == other);
}

unixosstring unixpath::prefix() const {
	return {};
}
bool unixpath::has_root() const {
	return m_data.size() && m_data.as_bytes()[0] == dirsep;
}
bool unixpath::is_absolute() const {
	return has_root();
}
bool unixpath::is_relative() const {
	return !is_absolute();
}

std::string unixpath::to_string_lossy() const {
	return m_data.to_string_lossy();
}

windowspath::windowspath(const char* s)
	: windowspath(std::string(s)) {
}

windowspath::windowspath(const std::string& s)
	: m_data(s) {
	// TODO: validate against list of reserved characters?
	//   https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-file#naming-conventions
	// TODO: validate the prefix?
}

bool windowspath::operator==(const windowspath& other) const {
	return m_data == other.m_data;
}
bool windowspath::operator!=(const windowspath& other) const {
	return !(*this == other);
}

windowsosstring windowspath::prefix() const {
	if (!m_data.size()) {
		return {};
	}

	auto p = m_data.as_wide();
	const auto start = p;
	// drive letter
	if (is_ascii_letter(*p) && *(p + 1) == ':') {
		p += 2;
		return m_data.substr(0, p - start);
	}
	// all other formats start with double-slash
	if (!(*p == '\\' && *(p + 1) == '\\')) {
		return {};
	}
	p += 2;
	// device namespace
	if (*p == '.' && *(p + 1) == '\\') {
		p += 2;
		if (*p == '\\' || *p == '\0') {
			// need at least one character in device name
			return {};
		}
		// until next slash or end of line
		while (!(*p == '\\' || *p == '\0')) {
			p++;
		}
		return m_data.substr(0, p - start);
	}
	// verbatim
	if (*p == '?' && *(p + 1) == '\\') {
		p += 2;
		// verbatim drive letter
		if (is_ascii_letter(*p) && *(p + 1) == ':') {
			p += 2;
			return m_data.substr(0, p - start);
		}
		// verbatim UNC
		if (*p == 'U' && *(p + 1) == 'N' && *(p + 2) == 'C' && *(p + 3) == '\\') {
			p += 4;
			if (*p == '\\' || *p == '\0') {
				// need at least one character in server name
				// TODO: is this correct? should it return just the verbatim part?
				return {};
			}
			// server name
			while (!(*p == '\\' || *p == '\0')) {
				p++;
			}
			if (*p != '\0') {
				p++;
			}
			if (*p == '\\' || *p == '\0') {
				// need at least one character in share name
				// TODO: is this correct? should it return just the server name?
				return {};
			}
			// share name
			while (!(*p == '\\' || *p == '\0')) {
				p++;
			}
			return m_data.substr(0, p - start);
		}
		return m_data.substr(0, p - start);
	}
	// UNC
	{
		if (*p == '\\' || *p == '\0') {
			// need at least one character in server name
			return {};
		}
		// server name
		while (!(*p == '\\' || *p == '\0')) {
			p++;
		}
		if (*p != '\0') {
			p++;
		}
		if (*p == '\\' || *p == '\0') {
			// need at least one character in share name
			// TODO: is this correct? should it return just the server name?
			return {};
		}
		// share name
		while (!(*p == '\\' || *p == '\0')) {
			p++;
		}
		return m_data.substr(0, p - start);
	}

	return {};
}

bool windowspath::has_root() const {
	if (!m_data.size()) {
		return false;
	}
	auto p = m_data.as_wide();
	// UNC, device, or verbatim
	if (*p == '\\') {
		// TODO: even for invalid prefixes?
		return true;
	}
	// drive letters
	if (is_ascii_letter(*p) && *(p + 1) == ':' && *(p + 2) == '\\') {
		return true;
	}
	return false;
}

bool windowspath::is_absolute() const {
	return prefix().size() && has_root();
}
bool windowspath::is_relative() const {
	return !is_absolute();
}

std::string windowspath::to_string_lossy() const {
	return m_data.to_string_lossy();
}