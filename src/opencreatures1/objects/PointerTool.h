#pragma once

#include "ObjectHandle.h"
#include "SimpleObject.h"

#include <stdint.h>
#include <string>

struct SFCLoader;
struct SFCSaver;

namespace sfc {
struct PointerToolV1;
};

struct PointerTool : SimpleObject {
	int32_t relx;
	int32_t rely;
	ObjectHandle bubble;
	std::string text;

	void load(SFCLoader&, const sfc::PointerToolV1*);
	void save(SFCSaver&, sfc::PointerToolV1*) const;
};
