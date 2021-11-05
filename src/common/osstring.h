#pragma once

#include <string>
#include <type_traits>

/*

  osstring();
  ossstring(const std::string&);
  osstring(const char*);
  
  // option<std::string> to_string() const noexcept;
  std::string to_string_lossy() const noexcept;
  // result<string, osstring> into_string() &&; // moves buffer
  
  size_t size() const noexcept;
  bool empty() const noexcept;
  
  // void make_ascii_lowercase() noexcept;
  // void make_ascii_uppercase() noexcept;
  // 
  // osstring to_ascii_lowercase() const noexcept;
  // osstring to_ascii_uppercase() const noexcept;
  
  // bool is_ascii() const noexcept;
  // bool eq_ignore_ascii_case(const osstring& other) const noexcept;
 
  void append(const osstring&);
  void operator+=(const osstring&);
  osstring operator+(const osstring&) const;
  
  osstring substr(size_t p, size_t len = std::string::npos) const;
  
  // void clear();
  
  operator==
  operator!=
  // comparisons
  // hash

  // Unix only
  const uint8_t* as_bytes() const;
  
  // Windows only
  static windowsosstring from_wide(const wchar_t*); // wchar on Windows
  static windowsosstring from_wide(const uint16_t*);
  const uint16_t* as_wide() const;

*/

template <class T>
class osstringimpl {
  public:
	size_t size() const;
	bool empty() const;
	void append(const T& other);
	void operator+=(const T& other);
	T operator+(const T& other);
	T substr(size_t pos, size_t len = std::string::npos) const;
	bool operator==(const T& other) const;
	bool operator!=(const T& other) const;

  protected:
	osstringimpl() = default;

  private:
	T* t();
	const T* t() const;
};

class unixosstring : public osstringimpl<unixosstring> {
  public:
	friend class osstringimpl<unixosstring>;

	unixosstring();
	unixosstring(const char* s);
	unixosstring(const std::string& s);

	std::string to_string_lossy() const;
	const uint8_t* as_bytes() const;

  private:
	std::basic_string<uint8_t> m_data;
};


class windowsosstring : public osstringimpl<windowsosstring> {
  public:
	friend class osstringimpl<windowsosstring>;

	windowsosstring();
	windowsosstring(const char* s);
	windowsosstring(const std::string& s);

	static windowsosstring from_wide(const uint16_t* s);
	static windowsosstring from_wide(const char16_t* s);
	template <typename T = wchar_t, typename = typename std::enable_if<sizeof(T) == 2>::type>
	static windowsosstring from_wide(const wchar_t* s) {
		return from_wide(reinterpret_cast<const uint16_t*>(s));
	}
	template <typename T = wchar_t, typename = typename std::enable_if<sizeof(T) == 2>::type>
	static windowsosstring from_wide(const std::wstring& s) {
		return from_wide(reinterpret_cast<const uint16_t*>(s.data()));
	}

	std::string to_string_lossy() const;
	const uint16_t* as_wide() const;

  private:
	std::basic_string<uint16_t> m_data;
};

#ifdef _WIN32
using osstring = windowsosstring;
#else
using osstring = unixosstring;
#endif