#include "common/encoding.h"

#include <gtest/gtest.h>

TEST(encoding, is_valid_utf8) {
	// surrogate half
	EXPECT_FALSE(is_valid_utf8("\xed\xa0\x80"));
	// overlong
	EXPECT_FALSE(is_valid_utf8("\xc0\xa0"));
}

TEST(encoding, ensure_utf8) {
	EXPECT_EQ(ensure_utf8("hello world"), "hello world");
	EXPECT_EQ(ensure_utf8("tr\xc3\xa8s cool"), "tr\xc3\xa8s cool");
	EXPECT_EQ(ensure_utf8("\xef\xbb\xbftr\xc3\xa8s cool"), "tr\xc3\xa8s cool");
	EXPECT_EQ(ensure_utf8("tr\xe8s cool"), "tr\xc3\xa8s cool");
}

TEST(encoding, cp1252_to_utf8) {
	// TODO: don't allow control/nonprintable characters, as they're likely not CP1252?
	EXPECT_EQ(cp1252_to_utf8("tr\xe8s cool"), "tr\xc3\xa8s cool");
	EXPECT_EQ(cp1252_to_utf8("\x80"), "\xe2\x82\xac");
}

TEST(encoding, utf8_to_cp1252) {
	EXPECT_EQ(utf8_to_cp1252("tr\xc3\xa8s cool"), "tr\xe8s cool");
}

TEST(encoding, utf8_to_cp1252_combining_diacriticals) {
	EXPECT_EQ(utf8_to_cp1252("tre\xcc\x80s cool"), "tr\xe8s cool");
}

std::basic_string<uint8_t> operator""_uint8(const char* arr, size_t size) {
	static_assert(sizeof(char) == sizeof(uint8_t), "");
	return std::basic_string<uint8_t>(reinterpret_cast<const uint8_t*>(arr),
		size);
}

TEST(encoding, to_utf8_lossy) {
	// no change
	EXPECT_EQ(to_utf8_lossy("tre\xcc\x80s cool"_uint8), "tre\xcc\x80s cool");
	// replacement character
	EXPECT_EQ(to_utf8_lossy("tr\xe8s cool"_uint8), "tr\xef\xbf\xbds cool");
}

TEST(encoding, utf16_decode) {
	const uint16_t buf[] = {0xd801, 0xdc37};
	char32_t codepoint;
	size_t units_read = utf16_decode(buf, &codepoint);
	EXPECT_EQ(units_read, 2);
	EXPECT_EQ(codepoint, 0x10437);
}

TEST(encoding, wchar_decode) {
	const std::wstring buf = L"très cool 醜いトマト 𐐷";
	EXPECT_EQ(wstring_to_string(L"très cool 醜いトマト 𐐷"),
		"tr\xC3\xA8s cool "
		"\xE9\x86\x9C\xE3\x81\x84\xE3\x83\x88\xE3\x83\x9E\xE3\x83\x88 "
		"\xF0\x90\x90\xB7");
}