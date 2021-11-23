#include "io.h"


void reader::read(uint8_t* buf, size_t n) {
	size_t total_bytes_read = 0;
	while (total_bytes_read < n) {
		size_t bytes_read = 0;
		try {
			bytes_read = read_some(buf + total_bytes_read, n - total_bytes_read);
		} catch (const io_interrupted&) {
			continue;
		}
		if (bytes_read == 0) {
			throw io_unexpectedeof();
		}
		total_bytes_read += bytes_read;
	}
}

std::vector<uint8_t> reader::read_to_end() {
	// TODO: this could be way more efficient. e.g. Rust reads in much larger chunks
	std::vector<uint8_t> result;
	while (true) {
		uint8_t b[1];
		try {
			size_t bytes_read = read_some(b, 1);
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

std::vector<uint8_t> reader::read_vector(size_t n) {
	std::vector<uint8_t> buf(n);
	read(buf.data(), n);
	return buf;
}

size_t reader::ignore_some(size_t n) {
	size_t total_bytes_ignored = 0;
	while (total_bytes_ignored < n) {
		size_t bytes_ignored = 0;
		try {
			uint8_t temp;
			bytes_ignored = read_some(&temp, 1);
		} catch (const io_interrupted&) {
			continue;
		}
		if (bytes_ignored == 0) {
			break;
		}
		total_bytes_ignored += bytes_ignored;
	}
	return total_bytes_ignored;
}

void reader::ignore(size_t n) {
	size_t total_bytes_ignored = 0;
	while (total_bytes_ignored < n) {
		size_t bytes_ignored = 0;
		try {
			bytes_ignored = ignore_some(n - total_bytes_ignored);
		} catch (const io_interrupted&) {
			continue;
		}
		if (bytes_ignored == 0) {
			throw io_unexpectedeof();
		}
		total_bytes_ignored += bytes_ignored;
	}
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
	read(result.data(), size);
	// need to try reading more in case size was wrong?
	while (true) {
		uint8_t b[1];
		try {
			size_t bytes_read = read_some(b, 1);
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

size_t seekablereader::ignore_some(size_t n) {
	try {
		seek(n, seek_type::seek_cur);
		return n;
	} catch (const io_notseekable&) {
		return this->reader::ignore_some(n);
	}
}

void writer::write(const uint8_t* buf, size_t n) {
	size_t total_bytes_written = 0;
	while (total_bytes_written < n) {
		size_t bytes_written = 0;
		try {
			bytes_written = write_some(buf + total_bytes_written, n - total_bytes_written);
		} catch (const io_interrupted&) {
			continue;
		}
		if (bytes_written == 0) {
			throw io_error("Unexpected zero bytes written");
		}
		total_bytes_written += bytes_written;
	}
}

void writer::write_str(const char* buf, size_t n) {
	return write(reinterpret_cast<const uint8_t*>(buf), n);
}

void writer::write_str(const std::string& s) {
	return write(reinterpret_cast<const uint8_t*>(s.data()), s.size());
}

void writer::flush() {
}