#pragma once

#include "Object.h"
#include "Rect.h"
#include "common/Exception.h"

#include <array>
#include <memory>
#include <stdint.h>
#include <vector>

namespace sfc {

struct EntityV1;

struct CompoundPartV1 {
	// not CArchive serialized
	std::shared_ptr<EntityV1> entity;
	int32_t x;
	int32_t y;
};

struct CompoundObjectV1 : ObjectV1 {
	std::vector<CompoundPartV1> parts;
	std::array<Rect, 6> hotspots;
	std::array<int32_t, 6> functions_to_hotspots;

	template <typename Archive>
	void serialize(Archive& ar) {
		ObjectV1::serialize(ar);

		ar.size_u32(parts);
		for (auto& part : parts) {
			ar(part.entity);
			if (part.entity == nullptr) {
				throw Exception("whoops, entity is null");
			}
			ar(part.x);
			ar(part.y);
		}
		for (auto& h : hotspots) {
			ar(h.left);
			ar(h.top);
			ar(h.right);
			ar(h.bottom);
		}
		for (auto& f : functions_to_hotspots) {
			ar(f);
		}
	}
};

} // namespace sfc