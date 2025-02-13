#include "s16Image.h"

#include "common/endianlove.h"
#include "common/shared_array.h"

#include <vector>

MultiImage ReadS16File(Reader& in) {
	uint32_t flags = read32le(in);
	bool is_565 = (flags & 0x01);
	imageformat imgformat = is_565 ? if_rgb565 : if_rgb555;
	auto numframes = read16le(in);

	MultiImage images(numframes);
	std::vector<uint32_t> offsets(numframes);

	// first, read the headers.
	for (unsigned int i = 0; i < numframes; i++) {
		offsets[i] = read32le(in);
		images[i].width = read16le(in);
		images[i].height = read16le(in);
		images[i].format = imgformat;
		images[i].colorkey = Color{0, 0, 0, 255};
	}

	for (unsigned int i = 0; i < numframes; i++) {
		if (offsets[i] != in.tell()) {
			// we don't care about offsets but the official engine does
			fmt::print("WARNING: S16 image offset in header was {} but file position is actually {}\n",
				offsets[i], in.tell());
		}
		images[i].data = shared_array<uint8_t>(2 * images[i].width * images[i].height);
		readmany16le(in, (uint16_t*)images[i].data.data(), images[i].width * images[i].height);
	}

	return images;
}
