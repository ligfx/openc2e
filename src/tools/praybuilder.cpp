#include "prayfile/Caos2PrayParser.h"
#include "prayfile/PraySourceAnalysis.h"
#include "prayfile/PraySourceParser.h"
#include "prayfile/PrayFileWriter.h"
#include "util.h"
#include "visit_overloads.h"

#define CXXOPTS_VECTOR_DELIMITER '\0'
#include <cxxopts.hpp>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <iostream>
#include <string>

namespace fs = ghc::filesystem;

int main(int argc, char**argv) {
    std::string output_filename;
    std::vector<std::string> input_filenames;
    
    cxxopts::Options options(argv[0], "");
    options.add_options()
        ("h,help", "Display help on command-line options")
        ("o,output-filename", "Write output to <arg>", cxxopts::value<std::string>(output_filename))
        ("input_filenames", "FILE...", cxxopts::value<std::vector<std::string>>(input_filenames))
    ;
    options.parse_positional({"input_filenames"});
    options.positional_help("FILE...");
    auto args = options.parse(argc, argv);
    
    if (args.count("help")) {
        std::cerr << options.help() << std::endl;
        exit(0);
    }
    if (input_filenames.empty()) {
        std::cerr << "Error: no input filenames" << std::endl;
        exit(1);
    }
    
    if (output_filename.empty()) {
        if (input_filenames.size() == 1) {
            // TODO: read CAOS2PRAY
            output_filename = fs::path(input_filenames[0]).stem().string() + ".agents";
        } else {
            std::cout << "Error: must specify output filename with multiple input files" << std::endl;
            exit(1);
        }
    }
    
    std::vector<PraySourceParser::Event> all_events;
    
    for (auto input_filename : input_filenames) {
        std::string str = readfile(input_filename);
        fs::path parent_path = fs::path(input_filename).parent_path();
        
        std::vector<PraySourceParser::Event> events;
        if (fs::path(input_filename).extension() == ".txt") {
            events = PraySourceParser::parse(str);
        } else if (fs::path(input_filename).extension() == ".cos") {
            events = Caos2PrayParser::parse(str);
        } else {
            std::cout << "Don't know how to handle input file \"" << input_filename << "\"" << std::endl;
            exit(1);
        }
        
        PraySourceAnalysis::check_duplicates(events, PraySourceAnalysis::WARN_ON_IDENTICAL_BLOCKS);
        
        for (auto &e : events) {
            e.source_filename = input_filename;
        }
        all_events.insert(all_events.end(), events.begin(), events.end());
    }
    
    PraySourceAnalysis::check_filenames(all_events);
    PraySourceAnalysis::check_files_exist(all_events);
    PraySourceAnalysis::check_duplicates(all_events, PraySourceAnalysis::IGNORE_IDENTICAL_BLOCKS);
    
    bool seen_error = false;
    for (auto e : all_events) {
        seen_error |= mpark::holds_alternative<PraySourceParser::Error>(e);
    }
    if (seen_error) {
        // since caos2pray isn't linear, our errors might not be in the right place
        std::stable_sort(all_events.begin(), all_events.end(), [](PraySourceParser::Event l, PraySourceParser::Event r) {
            return l.lineno < r.lineno;
        });
        
        for (auto event : all_events) {
            visit_overloads(
                event,
                [&](PraySourceParser::Error e) { std::cerr << event.source_filename << ":" << event.lineno << ": error: " << e.message << std::endl; },
                [&](PraySourceParser::Warning e) { std::cerr << event.source_filename << ":" << event.lineno << ": warning: " << e.message << std::endl; },
                [](PraySourceParser::GroupBlockStart) {},
                [](PraySourceParser::GroupBlockEnd) {},
                [](PraySourceParser::StringTag) {},
                [](PraySourceParser::IntegerTag) {},
                [](PraySourceParser::InlineBlock) {},
                [](PraySourceParser::StringTagFromFile) {}
            );
        }
        exit(1);
    }

    std::cout << "Writing output to \"" << output_filename << "\"" << std::endl;
    std::ofstream out(output_filename, std::ios::binary);
    if (!out) {
        std::cerr << "Couldn't open \"" << output_filename << "\"" << std::endl;
        exit(1);
    }
    PrayFileWriter writer(out);

    std::map<std::string, std::string> string_tags;
    std::map<std::string, int> int_tags;
    
    try {
        for (auto event : all_events) {
          visit_overloads(
              event, [](PraySourceParser::Error) {
                  /* handled already */
              },
              [&](PraySourceParser::Warning e) {
                std::cerr << event.source_filename << ":" << event.lineno << ": warning: " << e.message << std::endl;
              },
              [&](PraySourceParser::GroupBlockStart) {
                string_tags = {};
                int_tags = {};
              },
              [&](PraySourceParser::GroupBlockEnd e) {
                writer.writeBlockTags(e.type, e.name, int_tags, string_tags);
                std::cout << "Tag block " << e.type << " \""
                          << e.name << "\"\n";
              },
              [&](PraySourceParser::InlineBlock e) {
                std::cout << "Inline block " << e.type << " \""
                          << e.name << "\" from file \"" << e.filename
                          << "\"\n";

                auto data = readfile_binary(fs::path(event.source_filename).parent_path() / e.filename);
                writer.writeBlockRawData(e.type, e.name, data.data(), data.size());
              },
              [&](PraySourceParser::StringTag e) {
                string_tags[e.key] = e.value;
              },
              [&](PraySourceParser::StringTagFromFile e) {
                string_tags[e.key] = readfile(fs::path(event.source_filename).parent_path() / e.filename);
              },
              [&](PraySourceParser::IntegerTag e) {
                int_tags[e.key] = e.value;
              });
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(1);
    }

    std::cout << "Done!" << std::endl;
}
