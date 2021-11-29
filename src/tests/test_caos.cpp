#include "common/readfile.h"
#include "openc2e-core/PathResolver.h"
#include "openc2e-core/World.h"
#include "openc2e-core/caosScript.h"
#include "openc2e-core/caosVM.h"
#include "openc2e-core/creaturesImage.h"
#include "openc2e-core/imageManager.h"

#include <gtest/gtest.h>

class Openc2eTestHelper {
  public:
	static std::shared_ptr<creaturesImage> addBlnkSprite() {
		shared_array<uint8_t> pureblack(2 * 41 * 18);
		std::vector<Image> images(2);
		for (size_t i = 0; i < images.size(); ++i) {
			images[i].width = 41;
			images[i].height = 18;
			images[i].format = if_rgb565;
			images[i].data = pureblack;
		}

		std::shared_ptr<creaturesImage> img(new creaturesImage("blnk"));
		img->images = images;

		world.gallery->addImage(img);
		return img;
	}
};

static void run_script(const std::string& dialect, const std::string& s) {
	std::stringstream stream(s);
	caosScript script(dialect, "");
	script.parse(stream);

	caosVM vm(nullptr);
	vm.runEntirely(script.installer);
}

TEST(caos, unknown_dialect) {
	ASSERT_ANY_THROW(run_script("unknown dialect", ""));
}

TEST(caos, assert) {
	for (auto dialect : getDialectNames()) {
		run_script(dialect, "dbg: asrt 1 eq 1");
		ASSERT_ANY_THROW(run_script(dialect, "dbg: asrt 1 eq 2"));

		run_script(dialect, "dbg: asrf 1 eq 2");
		ASSERT_ANY_THROW(run_script(dialect, "dbg: asrf 1 eq 1"));

		ASSERT_ANY_THROW(run_script(dialect, "dbg: fail"));
	}
}

TEST(caos, file) {
	struct auto_data_directories {
		decltype(data_directories) original_data_directories;
		auto_data_directories() {
			original_data_directories = data_directories;
			data_directories = {DataDirectory(".")};
		}
		~auto_data_directories() { data_directories = original_data_directories; }
	} auto_directories;
	run_script("c3", readfile("../src/tests/file.cos"));
}

TEST(caos, flow) {
	run_script("c3", readfile("../src/tests/flow.cos"));
}

TEST(caos, ifblocks) {
	run_script("c3", readfile("../src/tests/ifblocks.cos"));
}

TEST(caos, parse_comment_end) {
	run_script("c3", readfile("../src/tests/parse-comment-end.cos"));
}

TEST(caos, parsing) {
	run_script("c3", readfile("../src/tests/parsing.cos"));
}

TEST(caos, simpleagent) {
	auto sprite = Openc2eTestHelper::addBlnkSprite();
	run_script("c3", readfile("../src/tests/simpleagent.cos"));
}

TEST(caos, special_lexing) {
	run_script("c1", readfile("../src/tests/special-lexing.cos"));
	run_script("c2", readfile("../src/tests/special-lexing.cos"));
}

TEST(caos, strings) {
	run_script("c3", readfile("../src/tests/strings.cos"));
}

TEST(caos, timeslice) {
	run_script("c1", readfile("../src/tests/timeslice.cos"));
	run_script("c2", readfile("../src/tests/timeslice.cos"));
}

TEST(caos, variables) {
	run_script("c3", readfile("../src/tests/variables.cos"));
}

TEST(caos, vector) {
	run_script("c3", readfile("../src/tests/vector.cos"));
}
