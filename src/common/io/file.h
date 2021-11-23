#pragma once

#include "io.h"

#include <stdio.h>

class filereader : public bufferedseekablereader {
  public:
	filereader();
	filereader(const std::string& filename);
	filereader(const filereader&) = delete;
	filereader& operator=(const filereader&) = delete;
	filereader(filereader&&);
	filereader& operator=(filereader&&);
	~filereader() override;

	bool is_open() const;
	void close();

	// reader
	size_t read_some(uint8_t* buf, size_t n) override;

	// seekable
	void seek(int64_t offset, seek_type whence = seek_type::seek_set) override;
	size_t tell() override;

	// buffered
	uint8_t peek() override;
	bool has_more_data() override;

  private:
	FILE* m_file = nullptr;
};

enum class write_open_type {
	write_open_truncate,
	write_open_append,
};

class filewriter : public writer {
  public:
	filewriter();
	filewriter(const std::string& filename, write_open_type open_type = write_open_type::write_open_truncate);
	filewriter(const filewriter&) = delete;
	filewriter& operator=(const filewriter&) = delete;
	filewriter(filewriter&&);
	filewriter& operator=(filewriter&&);
	~filewriter() override;
	void close();

	size_t write_some(const uint8_t* buf, size_t n) override;
	void flush() override;

  private:
	FILE* m_file = nullptr;
};