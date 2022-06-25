#pragma once

/*

Provides a global `format_to(iter, spec, args...)` function that
will be implemented by passing its arguments to `fmt::format_to`.

This is meant to be used alongside Fmt.h, so that custom types
can be formatted simply by including FmtFwd.h and defining a
`format(val, Ctx)` function, like so:

template <typename FormatContext>
auto format(const my_type& val, FormatContext& ctx) {
	return format_to(ctx.out(), "{}", val.as_some_fundamental_type());
}

*/

template <typename OutputIt, typename S, typename... Args>
auto format_to(OutputIt out, const S& s, Args&&... args);