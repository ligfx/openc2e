#pragma once

#include "common/Image.h"

#include <filesystem>
#include <string>
#include <vector>

class CobManager {
  public:
	struct CobFileInfo {
		CobFileInfo(std::string name_, std::filesystem::path filename_)
			: name(name_), filename(filename_) {}
		std::string name;
		std::filesystem::path filename;
		bool is_removable = false;
	};

	std::vector<CobFileInfo> objects;

	void update();
	Image getPicture(const CobFileInfo& info);
	void inject(const CobFileInfo& info);
	void remove(const CobFileInfo& info);
};