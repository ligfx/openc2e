#include "hedfile.h"

#include "common/endianlove.h"
#include "common/io/file.h"
#include "common/throw_ifnot.h"

hedfile read_hedfile(const std::string& path) {
	filereader in(path);
	return read_hedfile(in);
}

hedfile read_hedfile(bufferedreader& in) {
	hedfile hed;

	hed.frame_width = read32le(in);
	hed.frame_height = read32le(in);
	hed.numframes = read32le(in);

	THROW_IFNOT(!in.has_more_data());

	return hed;
}