#pragma once

#include "physics.h"

namespace physicsUtils {

optional<Point> intersect(const Line& l1, const Line& l2) {
	auto denominator = (l2.getEnd().y - l2.getStart().y) * (l1.getEnd().x - l1.getStart().x) - (l2.getEnd().x - l2.getStart().x) * (l1.getEnd().y - l1.getStart().y);
	if (std::fabs(denominator) < 0.00001) {
		// lines are parallel, e.g. they either don't intersect or they overlap
		return {};
	}

	auto a = (l2.getEnd().x - l2.getStart().x) * (l1.getStart().y - l2.getStart().y) - (l2.getEnd().y - l2.getStart().y) * (l1.getStart().x - l2.getStart().x);
	auto b = (l1.getEnd().x - l1.getStart().x) * (l1.getStart().y - l2.getStart().y) - (l1.getEnd().y - l1.getStart().y) * (l1.getStart().x - l2.getStart().x);

	auto ua = a / denominator;
	auto ub = b / denominator;

	if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1) {
		// intersected!
		auto x = l1.getStart().x + ua * (l1.getEnd().x - l1.getStart().x);
		auto y = l1.getStart().y + ua * (l1.getEnd().y - l1.getStart().y);
		return Point(x, y);
	}
	return {};
}

auto squared_distance(const Point& p1, const Point& p2) {
	double distx = p1.x - p2.x;
	double disty = p1.y - p2.y;
	return distx * distx + disty * disty;
}

Point as_unit_vector(const Point& p) {
	auto magnitude = std::sqrt(p.x * p.x + p.y * p.y);
	return Point(p.x / magnitude, p.y / magnitude);
}

} // namespace physicsUtils