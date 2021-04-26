/*
 *  physics.cpp
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
#include "physics.h"

#include "caos_assert.h"

#include <cassert>
#include <fmt/core.h>

void Line::dump() const {
	fmt::print("pst = ({}, {}) end=({}, {})\n", start.x, start.y, end.x, end.y);
}

Line::Line(Point s, Point e) {
	if (s.x > e.x)
		std::swap(s, e);
	start = s;
	end = e;
}

bool Line::intersect(const Line& l, Point& where) const {
	auto denominator = (l.end.y - l.start.y) * (end.x - start.x) - (l.end.x - l.start.x) * (end.y - start.y);
	if (std::fabs(denominator) < 0.00001) {
		// lines are parallel, e.g. they either don't intersect or they overlap
		return false;
	}

	auto a = (l.end.x - l.start.x) * (start.y - l.start.y) - (l.end.y - l.start.y) * (start.x - l.start.x);
	auto b = (end.x - start.x) * (start.y - l.start.y) - (end.y - start.y) * (start.x - l.start.x);

	auto ua = a / denominator;
	auto ub = b / denominator;

	if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1) {
		// intersected!
		auto x = start.x + ua * (end.x - start.x);
		auto y = start.y + ua * (end.y - start.y);
		where = Point(x, y);
		return true;
	}
	return false;
}

/* vim: set noet: */
