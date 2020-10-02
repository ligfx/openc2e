#pragma once

#include <string>
#include <vector>

#include "Image.h"

class CobManager {
public:
	struct CobFileInfo {
		CobFileInfo(std::string name_, std::string filename_) : name(name_), filename(filename_) {}
		std::string name;
		std::string filename;
	};

	std::vector<CobFileInfo> objects;
	
	void update();
	Image getPicture(const CobFileInfo& info);
	void inject(const CobFileInfo& info);
};