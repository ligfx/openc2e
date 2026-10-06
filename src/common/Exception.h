#pragma once

#include <fmt/core.h>
#include <stdexcept>
#include <string>

class Exception : public std::runtime_error {
  public:
	using runtime_error::runtime_error;
	virtual std::string prettyPrint() const { return std::string(what()); }
};

template <typename T = Exception>
[[noreturn]] void throw_exception(const char* message) {
	throw T(message);
}

template <typename T = Exception, typename... Args>
[[noreturn]] void throw_exception(fmt::format_string<Args...> fmt, Args&&... args) {
	throw T(fmt::format(fmt, std::forward<Args>(args)...));
}