#pragma once

#include "static_vector.h"

#include <string>

// If a string is UTF-8, simply return a copy (minus any BOM), otherwise assume
// it's CP-1252 and convert it.
std::string ensure_utf8(const std::string& str);

// If a string is UTF-8, convert it, otherwise assume it's already in CP-1252.
std::string ensure_cp1252(const std::string& str);

// translate strings, throwing if it's not possible
std::string cp1252_to_utf8(const std::string& cp1252_str);
std::string utf8_to_cp1252(const std::string& utf8_str);

// translate characters, throwing if it's not possible
char32_t cp1252_to_codepoint(unsigned char cp1252_char);
std::string codepoint_to_utf8(char32_t codepoint);
unsigned char unicode_to_cp1252(char32_t codepoint);

std::string utf16le_to_utf8(uint8_t* data, size_t num_bytes);

size_t utf8decode(const uint8_t* buf, char32_t* out);
size_t utf16le_decode(const uint8_t* buf, char32_t* out);
size_t utf16_decode(const uint16_t* buf, char32_t* out);
size_t utf32_decode(const uint32_t* buf, char32_t* out);
size_t wchar_decode(const wchar_t* buf, char32_t* out);

static_vector<uint8_t, 4> utf8_encode(char32_t codepoint);
static_vector<uint16_t, 2> utf16_encode(char32_t codepoint);
static_vector<uint32_t, 1> utf32_encode(char32_t codepoint);

std::string wstring_to_string(const std::wstring&);
std::wstring string_to_wstring(const std::string&);

// helper functions
std::string to_utf8_lossy(const std::basic_string<uint8_t>& s);
bool is_valid_utf8(const std::string& str);
bool cp1252_isprint(unsigned char cp1252_char);