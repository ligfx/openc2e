#include "osstring.h"

#include "encoding.h"

template <typename T>
size_t osstringimpl<T>::size() const {
	return t()->m_data.size();
}

template <typename T>
bool osstringimpl<T>::empty() const {
	return t()->m_data.empty();
}

template <typename T>
void osstringimpl<T>::append(const T& other) {
	t()->m_data.append(other.m_data);
}
template <typename T>
void osstringimpl<T>::operator+=(const T& other) {
	t()->m_data += other.m_data;
}

template <typename T>
T osstringimpl<T>::operator+(const T& other) {
	T copy = *t();
	copy += other;
	return copy;
}

template <typename T>
T osstringimpl<T>::substr(size_t pos, size_t len) const {
	T result;
	result.m_data = t()->m_data.substr(pos, len);
	return result;
}

template <typename T>
bool osstringimpl<T>::operator==(const T& other) const {
	return t()->m_data == other.m_data;
}

template <typename T>
bool osstringimpl<T>::operator!=(const T& other) const {
	return !(*this == other);
}

template <typename T>
T* osstringimpl<T>::t() {
	return static_cast<T*>(this);
}

template <typename T>
const T* osstringimpl<T>::t() const {
	return static_cast<const T*>(this);
}

template class osstringimpl<unixosstring>;
template class osstringimpl<windowsosstring>;


unixosstring::unixosstring() {
}

unixosstring::unixosstring(const char* s)
	: unixosstring(std::string(s)) {
}

unixosstring::unixosstring(const std::string& s)
	: m_data(reinterpret_cast<const uint8_t*>(s.data()), s.size()) {
}

std::string unixosstring::to_string_lossy() const {
	return to_utf8_lossy(m_data);
}

const uint8_t* unixosstring::as_bytes() const {
	return reinterpret_cast<const uint8_t*>(m_data.c_str());
}


windowsosstring::windowsosstring() {
}

windowsosstring::windowsosstring(const char* s)
	: windowsosstring(std::string(s)) {
}

windowsosstring::windowsosstring(const std::string& s) {
	const uint8_t* p = reinterpret_cast<const uint8_t*>(s.data());
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

		auto buf = utf16_encode(codepoint);
		m_data.append(buf.begin(), buf.end());
	}
}

windowsosstring windowsosstring::from_wide(const uint16_t* s) {
	windowsosstring result;
	result.m_data = s;
	return result;
}

windowsosstring windowsosstring::from_wide(const char16_t* s) {
	return from_wide(reinterpret_cast<const uint16_t*>(s));
}

std::string windowsosstring::to_string_lossy() const {
	std::string result;
	const uint16_t* p = m_data.data();
	while (true) {
		char32_t codepoint;
		size_t units_read = utf16_decode(p, &codepoint);
		if (units_read == 0) {
			result.append("\xef\xbf\xbd");
			p += 1;
			continue;
		}
		if (codepoint == 0) {
			break;
		}
		p += units_read;

		auto buf = utf8_encode(codepoint);
		result.append(buf.begin(), buf.end());
	}

	return result;
}

const uint16_t* windowsosstring::as_wide() const {
	return m_data.data();
}
