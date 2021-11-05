#pragma once

#include "io.h"

#include <stdio.h>

class filereader final : public seekablereader {
  public:
	filereader();
	filereader(const std::string& filename);
	~filereader() override;
	void close();

	size_t read_some(uint8_t* buf, size_t n) override;
	void seek(int64_t offset, seek_type whence) override;
	size_t tell() override;

  private:
	FILE* m_file = nullptr;
};

class filewriter final : public writer {
  public:
	filewriter();
	filewriter(const std::string& filename);
	~filewriter() override;
	void close();

	size_t write_some(const uint8_t* buf, size_t n) override;

  private:
	FILE* m_file = nullptr;
};