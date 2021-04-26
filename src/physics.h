/*
 *  physics.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue 08 Feb 2005.
 *  Copyright (c) 2005 Alyssa Milburn. All rights reserved.
 *  Copyright (c) 2005 Bryan Donlan. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */
#pragma once

#include "serfwd.h"
#include "utils/optional.h"

#include <cassert>
#include <cmath> // sqrt

// OS X header files use Point and Line, so..
#define Point MyPoint
#define Line MyLine

struct Point {
	float x, y;
	Point() { x = y = 0; }
	Point(float _x, float _y)
		: x(_x), y(_y) {}
	Point(const Point& p)
		: x(p.x), y(p.y) {}

	bool operator==(const Point& p) { return x == p.x && y == p.y; }
	bool operator!=(const Point& p) { return !(*this == p); }
	Point& operator=(const Point& r) {
		x = r.x;
		y = r.y;
		return *this;
	}
};

class Line {
  protected:
	FRIEND_SERIALIZE(Line)
	Point start, end;

  public:
	void dump() const;

	Line() {
		start = Point(0, 0);
		end = Point(1, 1);
	}

	Line(const Line& l) {
		start = l.start;
		end = l.end;
	}

	Line(Point s, Point e);

	Line& operator=(const Line& l) {
		start = l.start;
		end = l.end;
		return *this;
	}

	static optional<Point> intersect(const Line& l1, const Line& l2);

	bool isHorizontal() const { return start.y == end.y; }
	bool isVertical() const { return start.x == end.x; }
	float slope() const {
		assert(!isVertical());
		return (end.y - start.y) / (end.x - start.x);
	}
	const Point& getStart() const { return start; }
	const Point& getEnd() const { return end; }

	Point pointAtX(double x) const {
		assert(!isVertical());
		if (isHorizontal()) {
			return Point(x, start.y);
		} else {
			return Point(x, (x - start.x) * slope() + start.y);
		}
	}
	Point pointAtY(double y) const {
		assert(!isHorizontal());
		if (isVertical()) {
			return Point(start.x, y);
		} else {
			return Point((y - start.y) / slope() + start.x, y);
		}
	}

	bool containsX(double x) const {
		return x >= start.x && x <= end.x;
	}
};
