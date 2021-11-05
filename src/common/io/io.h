#pragma once

#include <stdexcept>
#include <string>
#include <vector>

class io_error : public std::runtime_error {
  public:
	using runtime_error::runtime_error;
};

class io_interrupted : public io_error {
  public:
	io_interrupted()
		: io_error("EINTR") {}
};

class io_notseekable : public io_error {
  public:
	io_notseekable()
		: io_error("") {}
};

class reader {
  public:
	virtual ~reader() = default;
	virtual size_t read_some(uint8_t* buf, size_t n) = 0;
	void read(uint8_t* buf, size_t n);
	virtual std::vector<uint8_t> read_to_end();
};

enum class seek_type {
	seek_set,
	seek_cur,
	seek_end
};

class seekablereader : public reader {
  public:
	virtual void seek(int64_t offset, seek_type whence = seek_type::seek_set) = 0;
	virtual size_t tell() = 0;
	std::vector<uint8_t> read_to_end() override;
};

class writer {
  public:
	virtual ~writer() = default;
	virtual size_t write_some(const uint8_t* buf, size_t n) = 0;
	void write(const uint8_t* buf, size_t n);
};