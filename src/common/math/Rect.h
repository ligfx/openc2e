#pragma once

#include "common/NumericCast.h"

#include <fmt/core.h>
#include <stdint.h>

/*

Two different types of rectangles, Rect2i and Rect2f, which store components
as int32_t or float, respectively.

- Why do we have two different types? The Creatures games tend to use integers
  for most things, but there are situations where floats either make things
  easier or are required for accuracy (e.g., graphics rendering).

- Rect2i objects are convertible to Rect2f objects, but not vice versa.

- Both rectangles store x, y, width, and height (not to be confused with e.g.
  Window's RECT struct or SFC serialized data, which store left, top, right, bottom).

*/

struct Rect2i {
	int32_t x = 0;
	int32_t y = 0;
	int32_t width = 0;
	int32_t height = 0;

	int32_t right() const { return x + width; }
	int32_t bottom() const { return y + height; }

	bool has_point(int32_t pointx, int32_t pointy) const {
		return pointx >= x && pointx < right() && pointy >= y && pointy < bottom();
	}

	bool has_point(float pointx, float pointy) const {
		// TODO: there's probably a better way to do this. see e.g.
		// https://stackoverflow.com/questions/58734034/how-to-properly-compare-an-integer-and-a-floating-point-values
		return pointx >= numeric_cast<float>(x) && pointx < numeric_cast<float>(right()) && pointy >= numeric_cast<float>(y) && pointy < numeric_cast<float>(bottom());
	}

	bool operator==(const Rect2i& other) const {
		return x == other.x && y == other.y && width == other.width && height == other.height;
	}
	bool operator!=(const Rect2i& other) const {
		return !(*this == other);
	}
};

struct Rect2f {
	float x = 0;
	float y = 0;
	float width = 0;
	float height = 0;

	Rect2f() {}
	Rect2f(float x_, float y_, float width_, float height_)
		: x(x_), y(y_), width(width_), height(height_) {}
	Rect2f(Rect2i other)
		: x(numeric_cast<float>(other.x)),
		  y(numeric_cast<float>(other.y)),
		  width(numeric_cast<float>(other.width)),
		  height(numeric_cast<float>(other.height)) {}

	float right() const { return x + width; }
	float bottom() const { return y + height; }

	bool has_point(float pointx, float pointy) const {
		return pointx >= x && pointx < right() && pointy >= y && pointy < bottom();
	}

	bool intersects(Rect2f other) const {
		if (x > other.x + other.width) {
			return false;
		}
		if (x + width < other.x) {
			return false;
		}
		if (y > other.y + other.height) {
			return false;
		}
		if (y + height < other.y) {
			return false;
		}
		return true;
	}

	bool operator==(const Rect2f& other) const {
		return x == other.x && y == other.y && width == other.width && height == other.height;
	}
	bool operator!=(const Rect2f& other) const {
		return !(*this == other);
	}
};

inline auto format_as(Rect2f val) {
	return fmt::format("Rect2f({},{} + {},{})", val.x, val.y, val.width, val.height);
}