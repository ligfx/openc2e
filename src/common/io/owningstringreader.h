#pragma once

#include "io.h"
#include "spanreader.h"

class owningstringreader final : public spanreader {
  public:
	owningstringreader()
		: owningstringreader("") {}
	owningstringreader(std::string buffer) {
		m_string = buffer;
		spanreader::operator=(spanreader(reinterpret_cast<const uint8_t*>(m_string.data()), m_string.size()));
	}
	owningstringreader(const owningstringreader&) = delete;
	owningstringreader& operator=(const owningstringreader&) = delete;

	const std::string& string() const {
		return m_string;
	}

  private:
	std::string m_string;
	spanreader m_spanreader;
};