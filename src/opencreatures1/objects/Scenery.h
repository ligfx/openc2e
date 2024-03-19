#pragma once

#include "Object.h"
#include "Renderable.h"

struct SFCLoader;
struct SFCSaver;

namespace sfc {
struct SceneryV1;
};

struct Scenery : Object {
	Renderable part;

	void load(SFCLoader&, const sfc::SceneryV1*);
	void save(SFCSaver&, sfc::SceneryV1*) const;
};
