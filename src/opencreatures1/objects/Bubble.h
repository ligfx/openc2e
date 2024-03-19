#pragma once

#include "SimpleObject.h"

struct SFCLoader;
struct SFCSaver;

namespace sfc {
struct BubbleV1;
}

struct Bubble : SimpleObject {
	// TODO: implement me
	void load(SFCLoader&, const sfc::BubbleV1*);
	void save(SFCSaver&, sfc::BubbleV1*) const;
};