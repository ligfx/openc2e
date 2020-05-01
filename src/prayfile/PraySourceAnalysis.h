#pragma once

#include "prayfile/PraySourceParser.h"

namespace PraySourceAnalysis {

void check_filenames(std::vector<PraySourceParser::Event> &events);
void check_files_exist(std::vector<PraySourceParser::Event> &events);

enum IdenticalBlocksStrategy {
    WARN_ON_IDENTICAL_BLOCKS,
    IGNORE_IDENTICAL_BLOCKS,
    ERROR_ON_IDENTICAL_BLOCKS,
};

void check_duplicates(std::vector<PraySourceParser::Event> &events, IdenticalBlocksStrategy identical_blocks_strategy = WARN_ON_IDENTICAL_BLOCKS);

} // namespace PraySourceAnalysis