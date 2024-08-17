#include "SimpleObject.h"

#include "SFCSerialization.h"
#include "fileformats/sfc/SimpleObject.h"

void SimpleObject::load(SFCLoader& ctx, const sfc::SimpleObjectV1* simp) {
	part = sfc_load_renderable(simp->part.get());
	z_order = simp->z_order;
	click_bhvr = simp->click_bhvr;
	touch_bhvr = simp->touch_bhvr;

	static_cast<Object*>(this)->load(ctx, static_cast<const sfc::ObjectV1*>(simp));
}

void SimpleObject::save(SFCSaver& ctx, sfc::SimpleObjectV1* simp) const {
	simp->part = sfc_dump_renderable(part);
	static_cast<sfc::ObjectV1*>(simp)->gallery = simp->part->gallery;
	simp->z_order = z_order;
	simp->click_bhvr = click_bhvr;
	simp->touch_bhvr = touch_bhvr;

	static_cast<const Object*>(this)->save(ctx, static_cast<sfc::ObjectV1*>(simp));
}