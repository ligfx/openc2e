#pragma once

#include <iosfwd>
#include <memory>
#include <string>

namespace sfc {

struct CreatureV1;
struct CGenomeV1;

struct EXPFile {
	std::shared_ptr<CreatureV1> creature;
	std::shared_ptr<CGenomeV1> genome;
	std::shared_ptr<CGenomeV1> child_genome;
};

EXPFile read_exp_v1_file(std::istream& in);
EXPFile read_exp_v1_file(const std::string& path);

} // namespace sfc