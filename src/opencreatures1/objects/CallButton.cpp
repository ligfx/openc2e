#include "CallButton.h"

#include "SFCSerialization.h"
#include "fileformats/sfc/CallButton.h"
#include "fileformats/sfc/Lift.h"

void CallButton::load(SFCLoader& ctx, sfc::CallButtonV1* cbtn) {
	lift = ctx.load_object(cbtn->lift);
	floor = cbtn->floor;

	static_cast<SimpleObject*>(this)->load(ctx, static_cast<sfc::SimpleObjectV1*>(cbtn));
}

void CallButton::save(SFCSaver& ctx, sfc::CallButtonV1* cbtn) {
	cbtn->lift = dynamic_cast<sfc::LiftV1*>(ctx.dump_object(lift).get());
	cbtn->floor = floor;

	static_cast<SimpleObject*>(this)->save(ctx, static_cast<sfc::SimpleObjectV1*>(cbtn));
}