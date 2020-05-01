#include "prayfile/PraySourceAnalysis.h"
#include "visit_overloads.h"

#include <fstream>
#include <ghc/filesystem.hpp>
#include <map>
#include <set>
#include <stdexcept>

using namespace PraySourceParser;
namespace fs = ghc::filesystem;

namespace PraySourceAnalysis {

template <typename T> // prevent automatic conversions to Event
Event make_error_from(const T &e, std::string message) {
    Event new_event = Error{message};
    new_event.lineno = e.lineno;
    new_event.source_filename = e.source_filename;
    return new_event;
}

template <typename T> // prevent automatic conversions to Event
Event make_warning_from(const T &e, std::string message) {
    Event new_event = Warning{message};
    new_event.lineno = e.lineno;
    new_event.source_filename = e.source_filename;
    return new_event;
}

void check_filenames(std::vector<Event> &events) {
    for (auto it = events.begin(); it != events.end(); ++it) {
        auto check_filename = [&](fs::path filename) {
            if (filename.has_root_path() || filename.has_root_name() ||
                filename.has_root_directory() || filename.is_absolute() ||
                !filename.is_relative()) {
                *it = make_error_from(*it, "\"" + filename.string() +
                            "\" absolute paths not allowed");
            } else if (*filename.lexically_normal().begin() == "..") {
                *it = make_error_from(*it, "\"" + filename.string() +
                            "\" only files in the same directory or "
                            "subdirectories are allowed");
            } else if (filename == "") {
                *it = make_error_from(*it, "\"\" is not a valid filename");
            }
        };
        visit_overloads(
            *it,
            [&](InlineBlock e) { check_filename(e.filename); },
            [&](StringTagFromFile e) { check_filename(e.filename); },
            [](Error) {},
            [](Warning) {},
            [](GroupBlockStart) {},
            [](GroupBlockEnd) {},
            [](IntegerTag) {},
            [](StringTag) {}
        );
    }
}


void check_files_exist(std::vector<Event> &events) {
    for (auto it = events.begin(); it != events.end(); ++it) {
        auto check_file = [&](fs::path filename) {
            if (it->source_filename.empty()) {
                throw std::runtime_error("Can't check event with source_filename = \"\"");
            }
            auto resolved_filename = fs::path(it->source_filename).parent_path() / filename;
            if (!fs::exists(resolved_filename)) {
                *it = make_error_from(*it, "\"" + filename.string() + "\" doesn't exist");
                return;
            }
            else if (!(fs::is_regular_file(resolved_filename) || fs::is_symlink(resolved_filename))) {
                *it = make_error_from(*it, "\"" + filename.string() + "\" is not a file");
            }
        };
        visit_overloads(
            *it,
            [](Error) {},
            [](Warning) {},
            [](GroupBlockStart) {},
            [](GroupBlockEnd) {},
            [](IntegerTag) {},
            [](StringTag) {},
            [&](InlineBlock e) { check_file(e.filename); },
            [&](StringTagFromFile e) { check_file(e.filename); }
        );
    }
}

void check_duplicates(std::vector<Event> &events, IdenticalBlocksStrategy identical_blocks_strategy) {
    std::set<std::string> block_names;
    std::map<std::string, InlineBlock> inline_blocks;
    
    std::set<std::string> integer_tag_names;
    std::set<std::string> string_tag_names;
    
    for (auto it = events.begin(); it != events.end(); ++it) {
        visit_overloads(
            *it,
            [](Error) { },
            [](Warning) { },
            [&](GroupBlockStart event) {
                integer_tag_names = {};
                string_tag_names = {};
                if (!block_names.insert(event.name).second) {
                    *it = make_error_from(*it, "Duplicate block \"" + event.name + "\"");
                }
            },
            [](GroupBlockEnd) {},
            [&](InlineBlock event) {
                if (inline_blocks.count(event.name) && inline_blocks[event.name].type == event.type && inline_blocks[event.name].filename == event.filename) {
                    switch (identical_blocks_strategy) {
                        case WARN_ON_IDENTICAL_BLOCKS:
                            *it = make_warning_from(*it, "Duplicate block \"" + event.name + "\" with identical contents");
                            return;
                        case IGNORE_IDENTICAL_BLOCKS:
                            it = events.erase(it) - 1;
                            return;
                        case ERROR_ON_IDENTICAL_BLOCKS:
                            *it = make_error_from(*it, "Duplicate block \"" + event.name + "\"");
                            return;
                    }
                }
                inline_blocks[event.name] = event;
                if (!block_names.insert(event.name).second) {
                    *it = make_error_from(*it, "Duplicate block \"" + event.name + "\"");
                }
            },
            [&](IntegerTag event) {
                if (!integer_tag_names.insert(event.key).second) {
                    *it = make_error_from(*it, "Duplicate integer tag \"" + event.key + "\"");
                }
            },
            [&](StringTag event) {
                if (!string_tag_names.insert(event.key).second) {
                    *it = make_error_from(*it, "Duplicate string tag \"" + event.key + "\"");
                }
            },
            [&](StringTagFromFile event) {
                if (!string_tag_names.insert(event.key).second) {
                    *it = make_error_from(*it, "Duplicate string tag \"" + event.key + "\"");
                }
            }
        );
    }
}

} // namespace PraySourceAnalysis