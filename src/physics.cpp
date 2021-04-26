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

optional<Point> Line::intersect(const Line& l1, const Line& l2) {
	auto denominator = (l2.end.y - l2.start.y) * (l1.end.x - l1.start.x) - (l2.end.x - l2.start.x) * (l1.end.y - l1.start.y);
	if (std::fabs(denominator) < 0.00001) {
		// lines are parallel, e.g. they either don't intersect or they overlap
		return {};
	}

	auto a = (l2.end.x - l2.start.x) * (l1.start.y - l2.start.y) - (l2.end.y - l2.start.y) * (l1.start.x - l2.start.x);
	auto b = (l1.end.x - l1.start.x) * (l1.start.y - l2.start.y) - (l1.end.y - l1.start.y) * (l1.start.x - l2.start.x);

	auto ua = a / denominator;
	auto ub = b / denominator;

	if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1) {
		// intersected!
		auto x = l1.start.x + ua * (l1.end.x - l1.start.x);
		auto y = l1.start.y + ua * (l1.end.y - l1.start.y);
		return Point(x, y);
	}
	return {};
}

/* vim: set noet: */
