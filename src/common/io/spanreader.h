#pragma once

#include "io.h"

class spanreader : public bufferedseekablereader {
  public:
	spanreader()
		: spanreader(nullptr, 0) {}

	spanreader(const uint8_t* buffer, size_t buffer_size)
		: m_buffer(buffer), m_buffer_size(buffer_size) {}

	template <typename T>
	spanreader(const T& t)
		: spanreader(t.data(), t.size()) {}

	size_t read_some(uint8_t* buf, size_t n) override;
	void seek(int64_t offset, seek_type whence = seek_type::seek_set) override;
	size_t tell() override;

	uint8_t peek() override;
	bool has_more_data() override;

  private:
	const uint8_t* m_buffer = nullptr;
	size_t m_buffer_size = 0;
	size_t m_position = 0;
};