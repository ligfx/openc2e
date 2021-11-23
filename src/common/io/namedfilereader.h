#pragma once

#include "common/io/file.h"

#include <ghc/filesystem.hpp>

class namedfilereader : public filereader {
  public:
	namedfilereader() = default;
	namedfilereader(ghc::filesystem::path p)
		: filereader(p), m_name(p) {}
	const ghc::filesystem::path& name() const {
		return m_name;
	}
	// namedfilereader(namedfilereader&& other) {
	//     m_reader = std::move(other.m_reader);
	//     m_name = std::move(other.m_name);
	// }
	// namedfilereader& operator=(namedfilereader&& other) {
	//     m_reader = std::move(other.m_reader);
	//     m_name = std::move(other.m_name);
	//     return *this;
	// }

  private:
	ghc::filesystem::path m_name;
};