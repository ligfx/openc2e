#pragma once

#include <array>
#include <stdexcept>

template <typename T, size_t N>
class static_vector {
  public:
	static_vector()
		: _size(0) {}

	void push_back(const T& t) {
		if (_size >= N) {
			throw std::logic_error("Pushed into static_vector that is already at capacity");
		}
		_data[_size++] = t;
	}

	void operator+=(const T& t) {
		push_back(t);
	}

	size_t size() const {
		return _size;
	}

	auto begin() const {
		return _data.begin();
	}
	auto end() const {
		return _data.begin() + _size;
	}

  private:
	size_t _size;
	std::array<T, N> _data;
};