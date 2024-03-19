#pragma once

#include "CompoundObject.h"
#include "common/math/Rect.h"

struct SFCLoader;
struct SFCSaver;

namespace sfc {
struct VehicleV1;
};

struct Vehicle : CompoundObject {
	float xvel;
	float yvel;
	Rect2i cabin;
	uint32_t bump;

	void load(SFCLoader&, const sfc::VehicleV1*);
	void save(SFCSaver&, sfc::VehicleV1*) const;
};