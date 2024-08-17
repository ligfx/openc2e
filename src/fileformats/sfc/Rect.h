#pragma once

#include "common/math/Rect.h"

#include <stdint.h>

namespace sfc {

struct Rect {
	// https://learn.microsoft.com/en-us/windows/win32/api/windef/ns-windef-rect
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;

	Rect() {}
	Rect(Rect2i other)
		: left(other.x), top(other.y), right(other.x + other.width), bottom(other.y + other.height) {}
	operator Rect2i() const {
		return Rect2i{left, top, right - left, bottom - top};
	}
};

} // namespace sfc