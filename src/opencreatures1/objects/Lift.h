#pragma once

#include "ObjectHandle.h"
#include "Vehicle.h"
#include "common/StaticVector.h"

#include <stdint.h>

struct SFCLoader;
struct SFCSaver;

namespace sfc {
struct LiftV1;
};

struct Lift : Vehicle {
	int32_t next_or_current_floor = -1;
	StaticVector<int32_t, 8> floors;
	StaticSet<ObjectHandle, 8> activated_call_buttons;

	void load(SFCLoader&, const sfc::LiftV1*);
	void save(SFCSaver&, sfc::LiftV1*) const;
};