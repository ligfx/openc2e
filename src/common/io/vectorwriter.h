#pragma once

#include "io.h"

#include <vector>

class vectorwriter final : public writer {
  public:
	vectorwriter() = default;
	size_t write_some(const uint8_t* buf, size_t n) {
		m_vector.insert(m_vector.end(), buf, buf + n);
		return n;
	}
	const std::vector<uint8_t>& vector() const {
		return m_vector;
	}

  private:
	std::vector<uint8_t> m_vector;
};