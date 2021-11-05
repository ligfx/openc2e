#include "io.h"


void reader::read_exact(uint8_t* buf, size_t n) {
	size_t bytes_read = 0;
	while (bytes_read < n) {
		try {
			bytes_read += read(buf + bytes_read, n - bytes_read);
		} catch (const io_interrupted&) {
			continue;
		}
		if (bytes_read == 0) {
			throw io_error("Unexpected end of file");
		}
	}
}

std::vector<uint8_t> reader::read_to_end() {
	std::vector<uint8_t> result;
	while (true) {
		uint8_t b[1];
		try {
			size_t bytes_read = read(b, 1);
			if (bytes_read == 0) {
				break;
			}
			result.emplace_back(static_cast<uint8_t>(b[0]));
		} catch (const io_interrupted&) {
			continue;
		}
	}
	return result;
}

std::vector<uint8_t> seekablereader::read_to_end() {
	// try to seek to end to get size and preallocate buffer
	long original_pos = 0;
	try {
		// TODO: what about EINTR?
		original_pos = tell();
		seek(0, seek_type::seek_end);
	} catch (const io_notseekable&) {
		return reader::read_to_end();
	}
	// TODO: what if we get an error on this tell() ?
	auto size = tell() - original_pos;
	seek(original_pos);
	// TODO: faster to not value initialize?
	std::vector<uint8_t> result(size);
	read_exact(result.data(), size);
	// need to try reading more in case size was wrong?
	while (true) {
		uint8_t b[1];
		try {
			size_t bytes_read = read(b, 1);
			if (bytes_read == 0) {
				break;
			}
			result.emplace_back(b[0]);
		} catch (const io_interrupted&) {
			continue;
		}
	}
	return result;
}

void writer::write_all(const uint8_t* buf, size_t n) {
	size_t bytes_written = 0;
	while (bytes_written < n) {
		try {
			bytes_written += write(buf + bytes_written, n - bytes_written);
		} catch (const io_interrupted&) {
			continue;
		}
		if (bytes_written == 0) {
			throw io_error("Unexpected zero bytes written");
		}
	}
}