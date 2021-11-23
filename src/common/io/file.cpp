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

filereader::filereader(filereader&& other) {
	m_file = other.m_file;
	other.m_file = nullptr;
}

filereader& filereader::operator=(filereader&& other) {
	if (m_file) {
		close();
	}
	m_file = other.m_file;
	other.m_file = nullptr;
	return *this;
}

filereader::~filereader() {
	if (m_file) {
		close();
	}
}

bool filereader::is_open() const {
	return m_file != nullptr;
}

void filereader::close() {
	if (m_file == nullptr) {
		throw io_error("Attempted closing an empty filereader object");
	}
	// TODO: do anything if fclose returns an error?
	fclose(m_file);
	m_file = nullptr;
}

size_t filereader::read_some(uint8_t* buf, size_t n) {
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

uint8_t filereader::peek() {
	int next_char = fgetc(m_file);
	if (next_char == EOF) {
		throw io_unexpectedeof();
	}
	if (ungetc(next_char, m_file) == EOF) {
		throw io_error(fmt::format("Error ocurred calling ungetc, errno = {}", errno));
	}
	// safe cast, fgetc returns the character read as an unsigned char cast to an int.
	return static_cast<uint8_t>(next_char);
}

bool filereader::has_more_data() {
	int next_char = fgetc(m_file);
	if (next_char == EOF) {
		return false;
	}
	if (ungetc(next_char, m_file) == EOF) {
		throw io_error(fmt::format("Error ocurred calling ungetc, errno = {}", errno));
	}
	return true;
}

filewriter::filewriter() = default;

filewriter::filewriter(const std::string& filename, write_open_type open_type) {
	// TODO: Windows filenames
	switch (open_type) {
		case write_open_type::write_open_truncate:
			m_file = fopen(filename.c_str(), "wb");
			break;
		case write_open_type::write_open_append:
			m_file = fopen(filename.c_str(), "ab");
			break;
	}
	if (m_file == nullptr) {
		throw io_error(fmt::format("Error opening '{}', errno {}", filename, errno));
	}
}

filewriter::filewriter(filewriter&& other) {
	m_file = other.m_file;
	other.m_file = nullptr;
}

filewriter& filewriter::operator=(filewriter&& other) {
	if (m_file) {
		close();
	}
	m_file = other.m_file;
	other.m_file = nullptr;
	return *this;
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

size_t filewriter::write_some(const uint8_t* buf, size_t n) {
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

void filewriter::flush() {
	if (m_file == nullptr) {
		throw io_error("Attempted flushing an empty filewriter object");
	}
	fflush(m_file);
}