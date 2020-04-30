#include "prayfile/Caos2PrayParser.h"
#include "prayfile/PraySourceParser.h"
#include "prayfile/PrayFileWriter.h"
#include "visit_overloads.h"

#include <iostream>
#include <map>
#include <string>

#include <fstream>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

static bool is_filename_okay(const std::string& filename) {
    auto p = fs::path(filename).lexically_normal();
    if (p.has_root_path() || p.has_root_name() || p.has_root_directory() || p.is_absolute()) {
        std::cerr << "Error: " << p << " absolute paths not allowed\n";
        return false;
    }
    if (*p.begin() == "..") {
        std::cerr << "Error: " << p << " only files in the same directory or subdirectories are allowed\n";
        return false;
    }
    if (!fs::exists(p)) {
        std::cerr << "Error: " << p << " doesn't exist\n";
        return false;
    }
    if (!(fs::is_regular_file(p) || fs::is_symlink(p))) {
        std::cerr << "Error: " << p << " is not a file\n";
        return false;
    }
    return true;
}

int main(int argc, char**argv) {
  {
      if (!(argc == 2 || argc == 3)) {
          printf("USAGE: %s INPUT [OUTPUT]\n", argv[0]);
          return 1;
      }
      
    std::ifstream f(argv[1]);
    std::string str((std::istreambuf_iterator<char>(f)),
                    std::istreambuf_iterator<char>());

    fs::path parent_path = fs::path(argv[1]).parent_path();

    std::vector<PraySourceParser::Event> events;
    if (fs::path(argv[1]).extension() == ".txt") {
        events = PraySourceParser::parse(str);
    } else if (fs::path(argv[1]).extension() == ".cos") {
        events = Caos2PrayParser::parse(str);
    } else {
        std::cout << "Don't know how to handle input file \"" << argv[1] << "\"" << std::endl;
        exit(1);
    }

    std::string output_filename;
    if (argc == 3) {
        output_filename = argv[2];
    } else {
        output_filename = fs::path(argv[1]).stem().string() + ".agents";
    }

    bool seen_error = false;
    for (auto res : events) {
      visit_overloads(
          res,
          [&](PraySourceParser::Error event) {
            std::cout << "Error: " << event.message << "\n";
            seen_error = true;
          },
          [](PraySourceParser::Warning event) {
            std::cerr << "Warning: " << event.message << std::endl;
          },
          [](PraySourceParser::GroupBlockStart) {},
          [](PraySourceParser::GroupBlockEnd) {},
          [](PraySourceParser::StringTag) {},
          [](PraySourceParser::IntegerTag) {},
          [&](PraySourceParser::InlineBlock event) {
              seen_error = !is_filename_okay(event.filename);
          },
          [&](PraySourceParser::StringTagFromFile event) {
              seen_error = !is_filename_okay(event.filename);
          }
      );
    }
    if (seen_error) {
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
    
    for (auto res : events) {
      visit_overloads(
          res, [](PraySourceParser::Error) {
              /* handled already */
          },
          [](PraySourceParser::Warning event) {
            std::cerr << "Warning: " << event.message << std::endl;
          },
          [&](PraySourceParser::GroupBlockStart) {
            string_tags = {};
            int_tags = {};
          },
          [&](PraySourceParser::GroupBlockEnd event) {
            writer.writeBlockTags(event.type, event.name, int_tags, string_tags);
            std::cout << "Tag block " << event.type << " \""
                      << event.name << "\"\n";
          },
          [&](PraySourceParser::InlineBlock event) {
              std::cout << "Inline block " << event.type << " \""
                      << event.name << "\" from file \"" << event.filename
                      << "\"\n";
            
            // TODO: check in same directory
          std::ifstream in((parent_path / event.filename).string());
          if (!in) {
              std::cout << "Couldn't open file \""
                        << (parent_path / event.filename).string() << "\""
                        << std::endl;
              exit(1);
          }
          std::vector<char> data((std::istreambuf_iterator<char>(in)),
                          std::istreambuf_iterator<char>());
                          
            writer.writeBlockRawData(event.type, event.name, data.data(), data.size());
            
          },
          [&](PraySourceParser::StringTag event) {
            string_tags[event.key] = event.value;
          },
          [&](PraySourceParser::StringTagFromFile event) {
            // TODO: check in same directory
            std::ifstream in((parent_path / event.filename).string());
            if (!in) {
                std::cout << "Couldn't open file \""
                          << (parent_path / event.filename).string() << "\""
                          << std::endl;
                exit(1);
            }
            std::string val((std::istreambuf_iterator<char>(in)),
                            std::istreambuf_iterator<char>());
                            
            string_tags[event.key] = val;
          },
          [&](PraySourceParser::IntegerTag event) {
            int_tags[event.key] = event.value;
          });
    }

    std::cout << "Done!" << std::endl;
  }
}
