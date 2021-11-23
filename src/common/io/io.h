#pragma once

#include "common/span.h"

#include <stdexcept>
#include <string>
#include <vector>

struct io_error : std::runtime_error {
	using runtime_error::runtime_error;
};

struct io_interrupted : io_error {
	io_interrupted()
		: io_error("EINTR") {}
};

struct io_notseekable : io_error {
	io_notseekable()
		: io_error("") {}
};

struct io_unexpectedeof : io_error {
	io_unexpectedeof()
		: io_error("") {}
};

struct reader {
	virtual ~reader() = default;
	virtual size_t read_some(uint8_t* buf, size_t n) = 0;

	void read(uint8_t* buf, size_t n);
	std::vector<uint8_t> read_vector(size_t n);
	virtual std::vector<uint8_t> read_to_end();

	void ignore(size_t n);
	virtual size_t ignore_some(size_t n);
};

struct writer {
	virtual ~writer() = default;
	virtual size_t write_some(const uint8_t* buf, size_t n) = 0;
	virtual void flush() = 0;

	void write(const uint8_t* buf, size_t n);
	void write_str(const char* buf, size_t n);
	void write_str(const std::string&);
};

enum class seek_type {
	seek_set,
	seek_cur,
	seek_end
};

struct seekablereader : virtual reader {
	virtual void seek(int64_t offset, seek_type whence = seek_type::seek_set) = 0;
	virtual size_t tell() = 0;
	std::vector<uint8_t> read_to_end() override;
	size_t ignore_some(size_t n) override;
};

struct bufferedreader : virtual reader {
	virtual uint8_t peek() = 0;
	virtual bool has_more_data() = 0;
};

struct bufferedseekablereader : bufferedreader, seekablereader {
};