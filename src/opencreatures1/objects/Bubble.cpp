#include "Bubble.h"

#include "SFCSerialization.h"
#include "fileformats/sfc/Bubble.h"

void Bubble::load(SFCLoader& ctx, const sfc::BubbleV1* bub) {
	// TODO: implement
	fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type: Bubble\n", family, genus, species);
	static_cast<SimpleObject*>(this)->load(ctx, static_cast<const sfc::SimpleObjectV1*>(bub));
}


void Bubble::save(SFCSaver& ctx, sfc::BubbleV1* bub) const {
	// TODO: implement
	static_cast<const SimpleObject*>(this)->save(ctx, static_cast<sfc::SimpleObjectV1*>(bub));
}