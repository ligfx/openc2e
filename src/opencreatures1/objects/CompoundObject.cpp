#include "CompoundObject.h"

#include "SFCSerialization.h"
#include "fileformats/sfc/CompoundObject.h"
#include "fileformats/sfc/Entity.h"

void CompoundObject::load(SFCLoader& ctx, const sfc::CompoundObjectV1* comp) {
	for (auto& cp : comp->parts) {
		parts.emplace_back();
		parts.back().renderable = sfc_load_renderable(cp.entity.get());
		parts.back().x = cp.x;
		parts.back().y = cp.y;
	}
	for (size_t i = 0; i < hotspots.size(); ++i) {
		hotspots[i] = comp->hotspots[i];
	}
	functions_to_hotspots = comp->functions_to_hotspots;
	static_cast<Object*>(this)->load(ctx, static_cast<const sfc::ObjectV1*>(comp));
}


void CompoundObject::save(SFCSaver& ctx, sfc::CompoundObjectV1* comp) const {
	for (auto& part : parts) {
		sfc::CompoundPartV1 sfcpart;
		sfcpart.entity = sfc_dump_renderable(part.renderable, comp->gallery);
		if (!comp->gallery) {
			comp->gallery = sfcpart.entity->gallery;
		}
		sfcpart.x = part.x;
		sfcpart.y = part.y;
		comp->parts.push_back(sfcpart);
	}
	for (size_t i = 0; i < comp->hotspots.size(); ++i) {
		comp->hotspots[i] = hotspots[i];
	}
	comp->functions_to_hotspots = functions_to_hotspots;
	static_cast<const Object*>(this)->save(ctx, static_cast<sfc::ObjectV1*>(comp));
}