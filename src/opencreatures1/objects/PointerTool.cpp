#include "PointerTool.h"

#include "EngineContext.h"
#include "ObjectManager.h"
#include "SFCSerialization.h"
#include "fileformats/NewSFCFile.h"

void PointerTool::load(SFCLoader& ctx, const sfc::PointerToolV1* pntr) {
	relx = pntr->relx;
	rely = pntr->rely;
	bubble = ctx.load_object(pntr->bubble);
	text = pntr->text;
	g_engine_context.pointer->m_pointer_tool = uid;
	static_cast<SimpleObject*>(this)->load(ctx, static_cast<const sfc::SimpleObjectV1*>(pntr));
}

void PointerTool::save(SFCSaver& ctx, sfc::PointerToolV1* pntr) const {
	pntr->relx = relx;
	pntr->rely = rely;
	pntr->bubble = dynamic_cast<sfc::BubbleV1*>(
		ctx.dump_object(g_engine_context.objects->try_get(bubble)).get());
	pntr->text = text;

	static_cast<const SimpleObject*>(this)->save(ctx, static_cast<sfc::SimpleObjectV1*>(pntr));
}