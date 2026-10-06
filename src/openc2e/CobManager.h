#pragma once

#include "common/Image.h"

#include <ghc/filesystem.hpp>
#include <string>
#include <vector>

class CobManager {
  public:
	struct CobFileInfo {
		CobFileInfo(std::string name_, ghc::filesystem::path filename_)
			: name(name_), filename(filename_) {}
		std::string name;
		ghc::filesystem::path filename;
		bool is_removable = false;
	};

	std::vector<CobFileInfo> objects;

	void update();
	Image getPicture(const CobFileInfo& info);
	void inject(const CobFileInfo& info);
	void remove(const CobFileInfo& info);
};