#include "fileformats/sfc/Blackboard.h"
#include "fileformats/sfc/Body.h"
#include "fileformats/sfc/CBiochemistry.h"
#include "fileformats/sfc/CBrain.h"
#include "fileformats/sfc/CGallery.h"
#include "fileformats/sfc/CGenome.h"
#include "fileformats/sfc/CInstinct.h"
#include "fileformats/sfc/CallButton.h"
#include "fileformats/sfc/CompoundObject.h"
#include "fileformats/sfc/Creature.h"
#include "fileformats/sfc/Entity.h"
#include "fileformats/sfc/Lift.h"
#include "fileformats/sfc/Limb.h"
#include "fileformats/sfc/Macro.h"
#include "fileformats/sfc/MapData.h"
#include "fileformats/sfc/PointerTool.h"
#include "fileformats/sfc/SFCFile.h"
#include "fileformats/sfc/Scenery.h"
#include "fileformats/sfc/SimpleObject.h"
#include "fileformats/sfc/Vehicle.h"

#include <fmt/core.h>
#include <fstream>


int main(int argc, char** argv) {
	if (argc != 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
		fmt::print(stderr, "USAGE: {} FILE\n", argv[0]);
		return 1;
	}

	auto input_filename = argv[1];

	std::ifstream in(input_filename, std::ios_base::binary);
	auto sfc = sfc::read_sfc_v1_file(in);

	// check if we read all of it
	in.peek();
	fmt::print("read entire file? {}\n", in.eof());

	fmt::print("number of objects {}\n", sfc.objects.size());
	fmt::print("number of sceneries {}\n", sfc.sceneries.size());
	fmt::print("number of scripts {}\n", sfc.scripts.size());
	fmt::print("number of macros {}\n", sfc.macros.size());
}
