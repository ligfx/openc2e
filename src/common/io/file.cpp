#include "file.h"

#include <errno.h>
#include <fmt/core.h>

filereader::filereader() = default;

filereader::filereader(const std::string& filename) {
	// TODO: Windows filenames
	m_file = fopen(filename.c_str(), "rb");
	if (m_file == nullptr) {
		throw io_error(fmt::format("Error opening '{}', errno {}", filename, errno));
	}
}

// filereader::filereader(const path& filename) {
// #ifdef _WIN32
// 	// TODO: only use _wfopen if we have non-ASCII characters or UNC path? Hmm.
// 	m_file = _wfopen(filename.to_wstring().c_str(), L"rb");
// #else
// 	m_file = fopen(filename.c_str(), "rb");
// #endif
// 	if (m_file == nullptr) {
// 		throw io_error(fmt::format("Error opening '{}', errno {}", filename, errno));
// 	}
// }

filereader::~filereader() {
	if (m_file) {
		close();
	}
}

void filereader::close() {
	if (m_file == nullptr) {
		throw io_error("Attempted closing an empty filereader object");
	}
	// TODO: do anything if fclose returns an error?
	fclose(m_file);
	m_file = nullptr;
}

size_t filereader::read(uint8_t* buf, size_t n) {
	if (m_file == nullptr) {
		throw io_error("Attempted reading from an empty filereader object");
	}
	if (n == 0) {
		return 0;
	}
	if (buf == nullptr) {
		throw io_error("Attempted reading into a null buffer");
	}
	size_t bytes_read = fread(buf, 1, n, m_file);
	if (bytes_read != n && ferror(m_file) != 0) {
		int err = errno;
		clearerr(m_file);
		if (err == EINTR) {
			if (bytes_read == 0) {
				throw io_interrupted();
			}
		} else {
			throw io_error(fmt::format("Error occurred reading file, errno = {}", err));
		}
	}
	return bytes_read;
}

void filereader::seek(int64_t pos, seek_type _whence) {
	if (m_file == nullptr) {
		throw io_notseekable();
	}
	const int whence = [&] {
		switch (_whence) {
			case seek_type::seek_set: return SEEK_SET;
			case seek_type::seek_cur: return SEEK_CUR;
			case seek_type::seek_end: return SEEK_END;
		}
	}();

	if (fseek(m_file, pos, whence) != 0) {
		int err = errno;
		clearerr(m_file);
		if (err == EPIPE) {
			// TODO: is this even useful?
			throw io_notseekable();
		}
		if (err == EINTR) {
			throw io_interrupted();
		} else {
			throw io_error(fmt::format("Error occurred seeking file, errno = {}", err));
		}
	}
}

size_t filereader::tell() {
	if (m_file == nullptr) {
		throw io_notseekable();
	}
	int64_t pos = ftell(m_file);
	if (pos == -1) {
		int err = errno;
		clearerr(m_file);
		if (err == EPIPE) {
			// TODO: is this even useful?
			throw io_notseekable();
		}
		if (err == EINTR) {
			throw io_interrupted();
		} else {
			throw io_error(fmt::format("Error occurred getting current position in file, errno = {}", err));
		}
	}
	return pos;
}

filewriter::filewriter() = default;

filewriter::filewriter(const std::string& filename) {
	// TODO: Windows filenames
	m_file = fopen(filename.c_str(), "wb");
	if (m_file == nullptr) {
		throw io_error(fmt::format("Error opening '{}', errno {}", filename, errno));
	}
}

filewriter::~filewriter() {
	if (m_file) {
		close();
	}
}

void filewriter::close() {
	if (m_file == nullptr) {
		throw io_error("Attempted closing an empty filewriter object");
	}
	while (true) {
		if (fflush(m_file) == 0) {
			break;
		}
		int err = errno;
		clearerr(m_file);
		if (err == EINTR) {
			continue;
		} else {
			throw io_error(fmt::format("Error occurred flushing file, errno = {}", err));
		}
	}
	int ret = fclose(m_file);
	m_file = nullptr;
	if (ret != 0) {
		throw io_error(fmt::format("Error occurred closing file, errno = {}", errno));
	}
}

size_t filewriter::write(const uint8_t* buf, size_t n) {
	if (m_file == nullptr) {
		throw io_error("Attempted writing to an empty filewriter object");
	}
	if (n == 0) {
		return 0;
	}
	if (buf == nullptr) {
		throw io_error("Attempted writing from a null buffer");
	}
	size_t bytes_written = fwrite(buf, 1, n, m_file);
	if (bytes_written != n && ferror(m_file) != 0) {
		int err = errno;
		clearerr(m_file);
		if (err == EINTR) {
			if (bytes_written == 0) {
				throw io_interrupted();
			}
		} else {
			throw io_error(fmt::format("Error occurred writing file, errno = {}", err));
		}
	}
	return bytes_written;
}