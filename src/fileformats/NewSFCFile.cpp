#include "NewSFCFile.h"

#include "MFCReader.h"
#include "MFCWriter.h"

#include <fstream>

namespace sfc {

SFCFile read_sfc_v1_file(const std::string& path) {
	std::ifstream in(path, std::ios_base::binary);
	return read_sfc_v1_file(in);
}

SFCFile read_sfc_v1_file(std::istream& in) {
	// set up types
	MFCReader reader(in);
	reader.register_class<MapDataV1>("MapData", 1);
	reader.register_class<CGalleryV1>("CGallery", 1);
	reader.register_class<PointerToolV1>("PointerTool", 1);
	reader.register_class<EntityV1>("Entity", 1);
	reader.register_class<CompoundObjectV1>("CompoundObject", 1);
	reader.register_class<SimpleObjectV1>("SimpleObject", 1);
	reader.register_class<VehicleV1>("Vehicle", 1);
	reader.register_class<LiftV1>("Lift", 1);
	reader.register_class<SceneryV1>("Scenery", 1);
	reader.register_class<MacroV1>("Macro", 1);
	reader.register_class<BlackboardV1>("Blackboard", 1);
	reader.register_class<CallButtonV1>("CallButton", 1);

	reader.register_class<CreatureV1>("Creature", 1);
	reader.register_class<BodyV1>("Body", 1);
	reader.register_class<LimbV1>("Limb", 1);
	reader.register_class<CBrainV1>("CBrain", 1);
	reader.register_class<CBiochemistryV1>("CBiochemistry", 1);
	reader.register_class<CInstinctV1>("CInstinct", 1);
	reader.register_class<CGenomeV1>("CGenome", 1);

	// read file
	SFCFile sfc;
	sfc.serialize(reader);
	return sfc;
}

void write_sfc_v1_file(const std::string& path, SFCFile& sfc) {
	std::ofstream out(path, std::ios_base::binary);
	return write_sfc_v1_file(out, sfc);
}

void write_sfc_v1_file(std::ostream& out, SFCFile& sfc) {
	MFCWriter writer(out);
	writer.register_class<sfc::MapDataV1>("MapData", 1);
	writer.register_class<sfc::CGalleryV1>("CGallery", 1);
	writer.register_class<sfc::PointerToolV1>("PointerTool", 1);
	writer.register_class<sfc::EntityV1>("Entity", 1);
	writer.register_class<sfc::CompoundObjectV1>("CompoundObject", 1);
	writer.register_class<sfc::SimpleObjectV1>("SimpleObject", 1);
	writer.register_class<sfc::VehicleV1>("Vehicle", 1);
	writer.register_class<sfc::LiftV1>("Lift", 1);
	writer.register_class<sfc::SceneryV1>("Scenery", 1);
	writer.register_class<sfc::MacroV1>("Macro", 1);
	writer.register_class<sfc::BlackboardV1>("Blackboard", 1);
	writer.register_class<sfc::CallButtonV1>("CallButton", 1);

	writer.register_class<CreatureV1>("Creature", 1);
	writer.register_class<BodyV1>("Body", 1);
	writer.register_class<LimbV1>("Limb", 1);
	writer.register_class<CBrainV1>("CBrain", 1);
	writer.register_class<CBiochemistryV1>("CBiochemistry", 1);
	writer.register_class<CInstinctV1>("CInstinct", 1);
	writer.register_class<CGenomeV1>("CGenome", 1);

	// write file
	sfc.serialize(writer);
}

EXPFile read_exp_v1_file(const std::string& path) {
	std::ifstream in(path, std::ios_base::binary);
	return read_exp_v1_file(in);
}

EXPFile read_exp_v1_file(std::istream& in) {
	// set up types
	MFCReader reader(in);
	reader.register_class<CGalleryV1>("CGallery", 1);
	reader.register_class<CreatureV1>("Creature", 1);
	reader.register_class<BodyV1>("Body", 1);
	reader.register_class<LimbV1>("Limb", 1);
	reader.register_class<CBrainV1>("CBrain", 1);
	reader.register_class<CBiochemistryV1>("CBiochemistry", 1);
	reader.register_class<CInstinctV1>("CInstinct", 1);
	reader.register_class<CGenomeV1>("CGenome", 1);

	// read file
	EXPFile exp;
	reader(exp.creature);
	if (in.peek(), in.eof()) {
		// we do this stupid conditional because some EXP files floating around
		// just totally skip the ending genome. in that case, just treat it as
		// null I guess?
		exp.genome = nullptr;
		exp.child_genome = nullptr;
	} else {
		reader(exp.genome);
		if (exp.creature->zygote.size()) {
			reader(exp.child_genome);
		}
	}
	return exp;
}

} // namespace sfc