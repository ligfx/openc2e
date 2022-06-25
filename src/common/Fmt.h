#pragma once

/*

Provides a way to simply define formatters for custom types without
including all of <fmt/core.h>.

Include this instead of <fmt/core.h> in your application code, then
for custom types simply include FmtFwd.h and define a
`format(val, Ctx)` function, like so:

template <typename FormatContext>
auto format(const my_type& val, FormatContext& ctx) {
	return format_to(ctx.out(), "{}", val.as_some_fundamental_type());
}

*/

#include <fmt/core.h>

template <typename OutputIt, typename S, typename... Args>
auto format_to(OutputIt out, const S& s, Args&&... args) {
	return fmt::format_to(out, s, args...);
}

namespace fmt {

namespace detail {

class DummyContext {
  public:
	char* out();
};

template <class, class = void>
struct is_formattable : std::false_type {};

template <typename T>
struct is_formattable<T, void_t<decltype(format(std::declval<const T&>(),
							 std::declval<DummyContext&>()))>> : std::true_type {};


template <typename T, typename FormatContext>
auto call_format_adl(const T& val, FormatContext& ctx) {
	return format(val, ctx);
}

// Formats an object of type T that has an overloaded ostream operator<<.
template <typename T, typename Char>
struct fallback_formatter<T, Char, enable_if_t<is_formattable<T>::value>> {
	template <typename ParseContext>
	constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const T& val, FormatContext& ctx) {
		return call_format_adl(val, ctx);
	}
};

} // namespace detail

} // namespace fmt