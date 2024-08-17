#include "Scenery.h"

#include "SFCSerialization.h"
#include "fileformats/sfc/Scenery.h"

void Scenery::load(SFCLoader& ctx, const sfc::SceneryV1* scen) {
	part = sfc_load_renderable(scen->part.get());
	static_cast<Object*>(this)->load(ctx, static_cast<const sfc::ObjectV1*>(scen));
}

void Scenery::save(SFCSaver& ctx, sfc::SceneryV1* scen) const {
	scen->part = sfc_dump_renderable(part);
	static_cast<sfc::ObjectV1*>(scen)->gallery = scen->part->gallery;
	static_cast<const Object*>(this)->save(ctx, static_cast<sfc::ObjectV1*>(scen));
}