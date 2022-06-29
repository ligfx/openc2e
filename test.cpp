#include <iostream>
#include <array>
#include <vector>


template <typename OutputIter>
class outputiteratorstreambuf : public std::streambuf {
public:
    outputiteratorstreambuf(OutputIter iter_) : iter(iter_) {}

    std::streamsize xsputn(const char_type* s, std::streamsize n) override 
    {
        for (std::streamsize i = 0; i < n; ++i) {
            *iter++ = *s++;
        }
        return n;
    };

    // int_type overflow(int_type ch) override 
    // { 
    //     return EOF;
    //     // *iter++ = static_cast<char>(ch);
    //     // return 1;
    // }

    OutputIter iter;
};

template <typename OutputIter>
auto make_outputiteratorstreambuf(OutputIter&& iter) {
    return outputiteratorstreambuf<OutputIter>(std::forward<OutputIter>(iter));
}

template <typename Handler>
constexpr int parse_format_string(const char* it, Handler &&handler) {
  int num_args = 0;
  auto start = it;
  while (*it) {
    char ch = *it++;
    if (ch != '{' && ch != '}') continue;
    // skip double curly braces
    if (*it == ch) {
      handler.on_text(start, it);
      start = ++it;
      continue;
    }
    // whoops, got an unexpected right curly brace
    if (ch == '}') {
      handler.on_error("unmatched '}' in format string");
      return 0;
    }
    // must be followed by closing brace
    if (*it != '}') {
        handler.on_error("'{' must be followed by '}'");
        return 0;
    }

    // handle previous text
    handler.on_text(start, it - 1);
    
    // output current arg
    handler.on_arg(num_args++);

    // keep going
    start = ++it;
  }
  handler.on_text(start, it);
  return num_args;
}

void format(int i) {
    printf("%i", i);
}

void format(const char * s) {
    printf("%s", s);
}

using formatter_t = void(*)(const void*);

template <typename T>
auto make_formatter() {
    return [](const void* data) {
        const T& val = *static_cast<const T*>(data);
        format(val);
    };
}

template< class ... > using void_t = void;

template <class, class = void>
struct is_formattable : std::false_type {};

template <typename T>
struct is_formattable<T, void_t<decltype(format(std::declval<const T&>()))>> : std::true_type {};


struct FormatArg {
    const void *data;
    formatter_t formatter;
};

template <typename T>
struct Check {
    static_assert(is_formattable<T>::value, "Whoops");
};

template <typename T, typename = std::enable_if_t<is_formattable<T>::value>>
FormatArg make_format_arg(const T& val) {
    // Check<T> x;
    return {
        static_cast<const void*>(&val),
        make_formatter<T>()
    };
}

template <typename ...Args>
auto make_format_args(const Args&... args) {
    return std::array<FormatArg, sizeof...(Args)>({
        make_format_arg(args)...
    });
}

class FormatArgs {
    public:
    template <size_t N>
    FormatArgs(const std::array<FormatArg, N>& args) : n(N), data(args.data()) {}
    size_t n;
    const FormatArg *data;

    void on_text(const char *start, const char *it) {
        printf("%.*s", static_cast<int>(it - start), start);
    }
    void on_error(const char *message) {
        throw message;
    }
    void on_arg(size_t n) {
        data[n].formatter(data[n].data);
    }
};

namespace ufmt {

    void vprint(const char *s, FormatArgs args) {
        printf("vprint: '%s' %i\n", s, args.n);
        
        parse_format_string(s, args);
    }

    template <typename ...Args>
    constexpr void print(const char *s, const Args&... args) {
        int result = parse_format_string(s , BasicHandler());
        auto fa = make_format_args(args...);
        vprint(s, fa);
    }
};

int main() {

    std::vector<char> v;

    auto buf = make_outputiteratorstreambuf(std::back_inserter(v));
    std::ostream out(&buf);

    out << 5.0000100;
    out << "hello there kenobi";

    v.push_back('\0');

    printf("{%s}\n", v.data());

    // constexpr const char* format_string = "hello{}poop";
    // constexpr int result = parse_format_string(format_string, BasicHandler());
    // constexpr int result2 = parse_format_string("nahh", BasicHandler());
    // parse_format_string(format_string, PrintHandler());
    // printf("\n");

    // auto fa = make_format_args(5, "hello");

    ufmt::print("hello{}poop{}", 5, "hello");

    // ufmtprint("hello{}poop(hi)", 5, "hello");

    return 0;
}