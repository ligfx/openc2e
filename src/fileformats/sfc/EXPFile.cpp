#include "EXPFile.h"

#include "Body.h"
#include "CBiochemistry.h"
#include "CBrain.h"
#include "CGallery.h"
#include "CGenome.h"
#include "CInstinct.h"
#include "Creature.h"
#include "Limb.h"
#include "MFCReader.h"
#include "MFCWriter.h"
#include "SimpleObject.h"

#include <fstream>

namespace sfc {

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