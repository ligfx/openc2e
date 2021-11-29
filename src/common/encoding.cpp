#include "encoding.h"

#include "endianlove.h"

#include <array>
#include <assert.h>
#include <stdexcept>

static_vector<uint8_t, 4> utf8_encode(char32_t c) {
	static_vector<uint8_t, 4> buf;
	if (c <= 0x7f) {
		buf += c;
	} else if (c <= 0x7ff) {
		buf += 0xc0 | (c >> 6);
		buf += 0x80 | (c & 0x3f);
	} else if (c <= 0xffff) {
		buf += 0xe0 | (c >> 12);
		buf += 0x80 | ((c >> 6) & 0x3f);
		buf += 0x80 | (c & 0x3f);
	} else if (c <= 0x10ffff) {
		buf += 0xf0 | (c >> 18);
		buf += 0x80 | ((c >> 12) & 0x3f);
		buf += 0x80 | ((c >> 6) & 0x3f);
		buf += 0x80 | (c & 0x3f);
	}
	return buf;
}

std::string codepoint_to_utf8(char32_t c) {
	auto s = utf8_encode(c);
	if (s.size() == 0) {
		throw std::domain_error("Can't convert " + std::to_string(c) + " into UTF-8");
	}
	return std::string(s.begin(), s.end());
}

size_t utf8decode(const uint8_t* s, char32_t* c) {
	size_t n = 0;
	if (s[0] <= 0x7f) {
		n = 1;
		*c = (char32_t)s[0];
	} else if (
		s[0] >= 0xc0 && s[0] <= 0xdf &&
		s[1] >= 0x80 && s[1] <= 0xbf) {
		n = 2;
		*c = ((char32_t)(s[0] & 0x1f) << 6) |
			 ((char32_t)(s[1] & 0x3f));
	} else if (
		s[0] >= 0xe0 && s[0] <= 0xef &&
		s[1] >= 0x80 && s[1] <= 0xbf &&
		s[2] >= 0x80 && s[2] <= 0xbf) {
		n = 3;
		*c = ((char32_t)(s[0] & 0x0f) << 12) |
			 ((char32_t)(s[1] & 0x3f) << 6) |
			 ((char32_t)(s[2] & 0x3f));
	} else if (
		s[0] >= 0xf0 && s[0] <= 0xf7 &&
		s[1] >= 0x80 && s[1] <= 0xbf &&
		s[2] >= 0x80 && s[2] <= 0xbf &&
		s[3] >= 0x80 && s[3] <= 0xbf) {
		n = 4;
		*c = ((char32_t)(s[0] & 0x07) << 18) |
			 ((char32_t)(s[1] & 0x3f) << 12) |
			 ((char32_t)(s[2] & 0x3f) << 6) |
			 ((char32_t)(s[3] & 0x3f));
	} else {
		// TODO: 5- and 6-byte sequences
		return 0;
	}
	// no surrogate halves allowed
	if (*c >= 0xd800 && *c <= 0xdfff) {
		return 0;
	}
	// no overlongs
	if (*c <= 0x7f) {
		if (n != 1)
			return 0;
	} else if (*c <= 0x7ff) {
		if (n != 2)
			return 0;
	} else if (*c <= 0xffff) {
		if (n != 3)
			return 0;
	} else if (*c <= 0x1fffff) {
		if (n != 4)
			return 0;
	}
	return n;
}

static bool utf8_is_combining_diacritical(char32_t codepoint) {
	return codepoint >= 0x300 && codepoint <= 0x36f;
}

static char32_t utf8_combine_diacriticals(char32_t c, char32_t combining) {
	// basic NFC normalization for characters that exist in both Unicode and CP-1252
	// clang-format off
	if (combining == 0x300 && c == 0x41) return 0xc0;
	if (combining == 0x300 && c == 0x45) return 0xc8;
	if (combining == 0x300 && c == 0x49) return 0xcc;
	if (combining == 0x300 && c == 0x4f) return 0xd2;
	if (combining == 0x300 && c == 0x55) return 0xd9;
	if (combining == 0x300 && c == 0x61) return 0xe0;
	if (combining == 0x300 && c == 0x65) return 0xe8;
	if (combining == 0x300 && c == 0x69) return 0xec;
	if (combining == 0x300 && c == 0x6f) return 0xf2;
	if (combining == 0x300 && c == 0x75) return 0xf9;
	if (combining == 0x301 && c == 0x41) return 0xc1;
	if (combining == 0x301 && c == 0x45) return 0xc9;
	if (combining == 0x301 && c == 0x49) return 0xcd;
	if (combining == 0x301 && c == 0x4f) return 0xd3;
	if (combining == 0x301 && c == 0x55) return 0xda;
	if (combining == 0x301 && c == 0x59) return 0xdd;
	if (combining == 0x301 && c == 0x61) return 0xe1;
	if (combining == 0x301 && c == 0x65) return 0xe9;
	if (combining == 0x301 && c == 0x69) return 0xed;
	if (combining == 0x301 && c == 0x6f) return 0xf3;
	if (combining == 0x301 && c == 0x75) return 0xfa;
	if (combining == 0x301 && c == 0x79) return 0xfd;
	if (combining == 0x302 && c == 0x41) return 0xc2;
	if (combining == 0x302 && c == 0x45) return 0xca;
	if (combining == 0x302 && c == 0x49) return 0xce;
	if (combining == 0x302 && c == 0x4f) return 0xd4;
	if (combining == 0x302 && c == 0x55) return 0xdb;
	if (combining == 0x302 && c == 0x61) return 0xe2;
	if (combining == 0x302 && c == 0x65) return 0xea;
	if (combining == 0x302 && c == 0x69) return 0xee;
	if (combining == 0x302 && c == 0x6f) return 0xf4;
	if (combining == 0x302 && c == 0x75) return 0xfb;
	if (combining == 0x303 && c == 0x41) return 0xc3;
	if (combining == 0x303 && c == 0x4e) return 0xd1;
	if (combining == 0x303 && c == 0x4f) return 0xd5;
	if (combining == 0x303 && c == 0x61) return 0xe3;
	if (combining == 0x303 && c == 0x6e) return 0xf1;
	if (combining == 0x303 && c == 0x6f) return 0xf5;
	if (combining == 0x308 && c == 0x41) return 0xc4;
	if (combining == 0x308 && c == 0x45) return 0xcb;
	if (combining == 0x308 && c == 0x49) return 0xcf;
	if (combining == 0x308 && c == 0x4f) return 0xd6;
	if (combining == 0x308 && c == 0x55) return 0xdc;
	if (combining == 0x308 && c == 0x59) return 0x178;
	if (combining == 0x308 && c == 0x61) return 0xe4;
	if (combining == 0x308 && c == 0x65) return 0xeb;
	if (combining == 0x308 && c == 0x69) return 0xef;
	if (combining == 0x308 && c == 0x6f) return 0xf6;
	if (combining == 0x308 && c == 0x75) return 0xfc;
	if (combining == 0x308 && c == 0x79) return 0xff;
	if (combining == 0x30a && c == 0x41) return 0xc5;
	if (combining == 0x30a && c == 0x61) return 0xe5;
	if (combining == 0x30c && c == 0x53) return 0x160;
	if (combining == 0x30c && c == 0x5a) return 0x17d;
	if (combining == 0x30c && c == 0x73) return 0x161;
	if (combining == 0x30c && c == 0x7a) return 0x17e;
	if (combining == 0x327 && c == 0x43) return 0xc7;
	if (combining == 0x327 && c == 0x63) return 0xe7;
	// clang-format on
	return 0;
}

static char32_t utf8_to_codepoint(const std::string& s, size_t& pos) {
	char32_t c;
	size_t bytes_read = utf8decode((uint8_t*)s.data() + pos, &c);
	if (bytes_read == 0) {
		throw std::domain_error("Invalid UTF-8 codepoint starting with " + std::to_string(s[pos]));
	}
	pos += bytes_read;
	return c;
}

bool is_valid_utf8(const char* s) {
	if (s == nullptr) {
		return true;
	}
	size_t pos = 0;
	while (s[pos] != '\0') {
		char32_t codepoint;
		size_t bytes_read = utf8decode((unsigned char*)s + pos, &codepoint);
		if (bytes_read == 0) {
			return false;
		}
		pos += bytes_read;
	}
	return true;
}

bool is_valid_utf8(const std::string& str) {
	return is_valid_utf8(str.c_str());
}

std::string to_utf8_lossy(const std::basic_string<uint8_t>& s) {
	std::string result;
	const uint8_t* p = s.data();
	while (*p != '\0') {
		char32_t codepoint;
		size_t bytes_read = utf8decode(p, &codepoint);
		if (bytes_read > 0) {
			result.insert(result.end(), p, p + bytes_read);
			p += bytes_read;
		} else {
			result += "\xef\xbf\xbd";
			p += 1;
		}
	}
	return result;
}

std::string cp1252_to_utf8(const std::string& cp1252_str) {
	std::string utf8_str;
	for (unsigned char c : cp1252_str) {
		utf8_str += codepoint_to_utf8(cp1252_to_codepoint(c));
	}
	return utf8_str;
}

std::string utf8_to_cp1252(const std::string& utf8_str) {
	// TODO: assert valid utf8?
	std::string cp1252_str;
	size_t pos = 0;
	while (pos < utf8_str.size()) {
		char32_t codepoint = utf8_to_codepoint(utf8_str, pos);
		if (utf8_is_combining_diacritical(codepoint) && cp1252_str.size() > 0) {
			char32_t combined = utf8_combine_diacriticals(cp1252_str.back(), codepoint);
			if (combined != 0) {
				cp1252_str.back() = unicode_to_cp1252(combined);
				continue;
			}
		}
		cp1252_str += unicode_to_cp1252(codepoint);
	}
	return cp1252_str;
}

std::string ensure_utf8(const std::string& str) {
	if (str.substr(0, 3) == "\xef\xbb\xbf") {
		return str.substr(3);
	}
	if (is_valid_utf8(str)) {
		return str;
	} else {
		return cp1252_to_utf8(str);
	}
}

std::string ensure_cp1252(const std::string& str) {
	if (is_valid_utf8(str)) {
		return utf8_to_cp1252(str);
	} else {
		return str.c_str();
	}
}

/*
mapping table available at
ftp://ftp.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
*/

char32_t cp1252_to_codepoint(unsigned char cp1252_char) {
	if (cp1252_char <= 0x7f) {
		return cp1252_char;
	}
	if (cp1252_char >= 0xa0 && cp1252_char <= 0xff) {
		return cp1252_char;
	}
	switch (cp1252_char) {
		case 0x80: return 0x20ac;
		case 0x82: return 0x201a;
		case 0x83: return 0x0192;
		case 0x84: return 0x201e;
		case 0x85: return 0x2026;
		case 0x86: return 0x2020;
		case 0x87: return 0x2021;
		case 0x88: return 0x02c6;
		case 0x89: return 0x2030;
		case 0x8a: return 0x0160;
		case 0x8b: return 0x2039;
		case 0x8c: return 0x0152;
		case 0x8e: return 0x017d;
		case 0x91: return 0x2018;
		case 0x92: return 0x2019;
		case 0x93: return 0x201c;
		case 0x94: return 0x201d;
		case 0x95: return 0x2022;
		case 0x96: return 0x2013;
		case 0x97: return 0x2014;
		case 0x98: return 0x02dc;
		case 0x99: return 0x2122;
		case 0x9a: return 0x0161;
		case 0x9b: return 0x203a;
		case 0x9c: return 0x0153;
		case 0x9e: return 0x017e;
		case 0x9f: return 0x0178;
	}
	// only for 0x81, 0x8d, 0x8f, 0x90, and 0x9d
	throw std::domain_error("Unsupported CP-1252 character " + std::to_string(cp1252_char));
}

unsigned char unicode_to_cp1252(char32_t codepoint) {
	if (codepoint <= 0x7f) {
		return codepoint;
	}
	if (codepoint >= 0xa0 && codepoint <= 0xff) {
		return codepoint;
	}
	switch (codepoint) {
		case 0x20ac: return 0x80;
		case 0x201a: return 0x82;
		case 0x0192: return 0x83;
		case 0x201e: return 0x84;
		case 0x2026: return 0x85;
		case 0x2020: return 0x86;
		case 0x2021: return 0x87;
		case 0x02c6: return 0x88;
		case 0x2030: return 0x89;
		case 0x0160: return 0x8a;
		case 0x2039: return 0x8b;
		case 0x0152: return 0x8c;
		case 0x017d: return 0x8e;
		case 0x2018: return 0x91;
		case 0x2019: return 0x92;
		case 0x201c: return 0x93;
		case 0x201d: return 0x94;
		case 0x2022: return 0x95;
		case 0x2013: return 0x96;
		case 0x2014: return 0x97;
		case 0x02dc: return 0x98;
		case 0x2122: return 0x99;
		case 0x0161: return 0x9a;
		case 0x203a: return 0x9b;
		case 0x0153: return 0x9c;
		case 0x017e: return 0x9e;
		case 0x0178: return 0x9f;
	}
	throw std::domain_error("Code point " + std::to_string(codepoint) + " doesn't exist in CP-1252");
}

bool cp1252_isprint(unsigned char c) {
	// ASCII control characters
	if (c < 0x20) {
		return false;
	}
	// DEL
	if (c == 0x7f) {
		return false;
	}
	// characters that don't exist in CP-1252
	if (c == 0x81 || c == 0x8d || c == 0x8f || c == 0x90 || c == 0x9d) {
		return false;
	}
	return true;
}

size_t utf16_decode(const uint16_t* buf, char32_t* out) {
	assert(buf);
	assert(out);

	uint16_t c1 = *buf;
	if (c1 >= 0xd800 && c1 < 0xdc00) {
		uint16_t c2 = *(buf + 1);
		if (c2 >= 0xdc00 && c2 < 0xe000) {
			*out = ((c1 & 0x3ff) << 10) + (c2 & 0x3ff) + 0x10000;
			return 2;
		} else {
			return 0;
		}
	}
	*out = c1;
	return 1;
}

size_t utf16le_decode(const uint8_t* buf, char32_t* out) {
	assert(buf);
	assert(out);

	uint16_t c1 = read16le(buf);
	if (c1 >= 0xd800 && c1 < 0xdc00) {
		uint16_t c2 = read16le(buf + 2);
		*out = ((c1 & 0x3ff) << 10) + (c2 & 0x3ff) + 0x10000;
		return 4;
	}
	*out = c1;
	return 2;
}

size_t utf32_decode(const uint32_t* buf, char32_t* out) {
	assert(buf);
	assert(out);
	*out = buf[0];
	return 1;
}


template <int N>
size_t wchar_decode_impl(const wchar_t* buf, char32_t* out);

template <>
size_t wchar_decode_impl<2>(const wchar_t* buf, char32_t* out) {
	return utf16_decode((const uint16_t*)buf, out);
}

template <>
size_t wchar_decode_impl<4>(const wchar_t* buf, char32_t* out) {
	return utf32_decode((const uint32_t*)buf, out);
}

size_t wchar_decode(const wchar_t* buf, char32_t* out) {
	return wchar_decode_impl<sizeof(wchar_t)>(buf, out);
}

static_vector<uint16_t, 2> utf16_encode(char32_t codepoint) {
	static_vector<uint16_t, 2> buf;
	if (codepoint > 0xffff) {
		buf += ((codepoint - 0x10000) >> 10) + 0xd800;
		buf += (codepoint & 0x3ff) + 0xdc00;
	} else {
		buf += codepoint;
	}
	return buf;
}

static_vector<uint32_t, 1> utf32_encode(char32_t codepoint) {
	static_vector<uint32_t, 1> buf;
	buf += codepoint;
	return buf;
}

template <int N>
auto wchar_encode_impl(char32_t codepoint);

template <>
auto wchar_encode_impl<2>(char32_t codepoint) {
	return utf16_encode(codepoint);
}

template <>
auto wchar_encode_impl<4>(char32_t codepoint) {
	return utf32_encode(codepoint);
}

auto wchar_encode(char32_t codepoint) {
	return wchar_encode_impl<sizeof(wchar_t)>(codepoint);
}

std::string wstring_to_string(const std::wstring& s) {
	std::string result;
	const wchar_t* p = s.data();
	while (true) {
		char32_t codepoint;
		size_t units_read = wchar_decode(p, &codepoint);
		if (codepoint == 0) {
			break;
		}
		result += codepoint_to_utf8(codepoint);
		p += units_read;
	}
	return result;
}


std::wstring string_to_wstring(const std::string& s) {
	std::wstring result;

	const uint8_t* p = (const uint8_t*)s.c_str();
	while (true) {
		char32_t codepoint;
		size_t bytes_read = utf8decode(p, &codepoint);
		if (bytes_read == 0) {
			throw std::domain_error("Invalid UTF-8 string");
		}
		if (codepoint == 0) {
			break;
		}
		p += bytes_read;

		auto buf = wchar_encode(codepoint);
		result.append(buf.begin(), buf.end());
	}
	return result;
}

std::string utf16le_to_utf8(uint8_t* data, size_t num_bytes) {
	std::string s;
	uint8_t* p = data;
	while (p < data + num_bytes) {
		char32_t codepoint;
		size_t bytes_read = utf16le_decode(p, &codepoint);
		s += codepoint_to_utf8(codepoint);
		p += bytes_read;
	}
	return s;
}