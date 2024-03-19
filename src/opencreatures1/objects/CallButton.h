#pragma once

#include "ObjectHandle.h"
#include "SimpleObject.h"

#include <stdint.h>

struct SFCLoader;
struct SFCSaver;

namespace sfc {
struct CallButtonV1;
};

struct CallButton : SimpleObject {
	ObjectHandle lift;
	uint8_t floor;

	void load(SFCLoader&, sfc::CallButtonV1*);
	void save(SFCSaver&, sfc::CallButtonV1*);
};