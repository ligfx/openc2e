/*
 *  mmapifstream.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat Jul 24 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#pragma once

#include "spanstream.h"

#include <string>

class mmapifstream : public spanstream {
  public:
	mmapifstream();
	mmapifstream(const std::string& filename);
	~mmapifstream();
	bool is_open() { return map != nullptr; }
	char* map = nullptr;

  private:
	size_t filesize = 0;
};

/* vim: set noet: */