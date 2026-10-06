#pragma once

#include "common/Exception.h"

#include <ghc/filesystem.hpp>
#include <memory>
#include <string>
#include <vector>

class parseException : public Exception {
  public:
	explicit parseException(std::string message)
		: Exception(message) {}
	parseException(std::string message, int lineno_)
		: Exception(message), lineno{lineno_} {}

	std::shared_ptr<std::vector<struct caostoken> > context;
	int ctxoffset;
	ghc::filesystem::path filename;
	int lineno = -1;

	std::string prettyPrint() const;
};
