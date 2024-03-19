#pragma once

#include "Object.h"
#include "Renderable.h"

#include <array>
#include <stdint.h>

struct SFCLoader;
struct SFCSaver;

namespace sfc {
struct SimpleObjectV1;
};

struct SimpleObject : Object {
	Renderable part;
	int32_t z_order;
	std::array<int8_t, 3> click_bhvr;
	uint8_t touch_bhvr;

	void load(SFCLoader&, const sfc::SimpleObjectV1*);
	void save(SFCSaver&, sfc::SimpleObjectV1*) const;
};
