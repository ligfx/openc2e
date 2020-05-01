#pragma once

#include <mpark/variant.hpp>
#include <string>
#include <vector>

namespace PraySourceParser {
    struct Error {
      std::string message;
    };
    struct Warning {
      std::string message;
    };
    struct GroupBlockStart {
        std::string type;
        std::string name;
    };
    struct GroupBlockEnd {
      std::string type;
      std::string name;
    };
    struct InlineBlock {
      std::string type;
      std::string name;
      std::string filename;
    };
    struct StringTag {
      std::string key;
      std::string value;
    };
    struct StringTagFromFile {
      std::string key;
      std::string filename;
    };
    struct IntegerTag {
      std::string key;
      int value;
    };
    struct Event : mpark::variant<Error, Warning, GroupBlockStart, GroupBlockEnd,
                        InlineBlock, StringTag, StringTagFromFile, IntegerTag>
    {
        using mpark::variant<Error, Warning, GroupBlockStart, GroupBlockEnd,
                            InlineBlock, StringTag, StringTagFromFile, IntegerTag>::variant;
        // lineno is set by the parser
        int lineno = 0;
        // source_filename is not set by the parser, but is needed by praybuilder
        // and by some of the analysis checks
        std::string source_filename;
    };

    int lineno(const Event&);

    std::vector<Event> parse(const std::string&);
    std::string eventToString(const Event&);
}
