#include "text.h"

#include "common/encoding.h"

#include <ctype.h>

bool getline(reader& in, std::string& s) {
	bool first_read = true;
	while (true) {
		char next_char;
		bool success = in.read_some(reinterpret_cast<uint8_t*>(&next_char), 1);
		if (success == 0) {
			if (first_read) {
				return false;
			} else {
				return true;
			}
		}
		if (next_char == '\r') {
			char second_char = {};
			in.read(reinterpret_cast<uint8_t*>(second_char), 1);
			if (second_char == '\n') {
				return true;
			} else {
				s += next_char;
				s += second_char;
			}
		} else if (next_char == '\n') {
			return true;
		} else {
			s += next_char;
		}
	}
}

static uint8_t read_byte(reader& in) {
	uint8_t c;
	in.read(&c, 1);
	return c;
}

int read_text_int(bufferedreader& in) {
	// [ \t\r\n]* [+-]? [0-9]+
	std::vector<uint8_t> buf;
	uint8_t c;
start:
	c = in.peek();
	if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
		(void)read_byte(in);
		goto start;
	} else if (c == '+' || c == '-') {
		buf.push_back(read_byte(in));
		goto have_sign;
	} else if (std::isdigit(c)) {
		buf.push_back(read_byte(in));
		goto digits;
	} else {
		throw io_error("Expected int to start with digit or sign");
	}
have_sign:
	c = in.peek();
	if (std::isdigit(c)) {
		buf.push_back(read_byte(in));
		goto digits;
	} else {
		throw io_error("Int must have digit following sign");
	}
digits:
	if (!in.has_more_data()) {
		goto finish;
	}
	c = in.peek();
	if (std::isdigit(c)) {
		buf.push_back(read_byte(in));
		goto digits;
	} else {
		goto finish;
	}
finish:
	buf.push_back('\0');
	return std::atof(reinterpret_cast<const char*>(buf.data()));
}

float read_text_float(bufferedreader& in) {
	// [ \t\r\n]* [+-]? [0-9]+ ("." [0-9]*)?
	std::vector<uint8_t> buf;
	uint8_t c;
start:
	c = in.peek();
	if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
		(void)read_byte(in);
		goto start;
	} else if (c == '+' || c == '-') {
		buf.push_back(read_byte(in));
		goto have_sign;
	} else if (std::isdigit(c)) {
		buf.push_back(read_byte(in));
		goto leading_digits;
	} else {
		throw std::domain_error("Expected float to start with digit or sign");
	}
have_sign:
	c = in.peek();
	if (std::isdigit(c)) {
		buf.push_back(read_byte(in));
		goto leading_digits;
	} else {
		throw std::domain_error("Float must have digit following sign");
	}
leading_digits:
	if (!in.has_more_data()) {
		goto finish;
	}
	c = in.peek();
	if (std::isdigit(c)) {
		buf.push_back(read_byte(in));
		goto leading_digits;
	} else if (c == '.') {
		buf.push_back(read_byte(in));
		goto trailing_digits;
	} else {
		// TODO: scientific notation/exponent?
		goto finish;
	}
trailing_digits:
	if (!in.has_more_data()) {
		goto finish;
	}
	c = in.peek();
	if (std::isdigit(c)) {
		buf.push_back(read_byte(in));
		goto trailing_digits;
	} else {
		// TODO: scientific notation/exponent?
		goto finish;
	}
finish:
	buf.push_back('\0');
	return std::atof(reinterpret_cast<const char*>(buf.data()));
}

std::vector<uint8_t> read_until(reader& in, uint8_t delimiter) {
	// TODO: Rust has this as a function on the BufRead trait
	// TODO: given a bufferedreader, this should be able to be much faster

	std::vector<uint8_t> buf;
	while (true) {
		try {
			uint8_t c;
			size_t bytes_read = in.read_some(reinterpret_cast<uint8_t*>(&c), 1);
			if (bytes_read == 0) {
				return buf;
			}
			buf.push_back(c);
			if (c == delimiter) {
				return buf;
			}
		} catch (io_interrupted&) {
			continue;
		}
	}
}

std::string read_ascii_line(reader& in) {
	return ascii_to_utf8(read_until(in, '\n'));
}

std::string read_cp1252_line(reader& in) {
	return cp1252_to_utf8(read_until(in, '\n'));
}

std::string read_ascii_string(reader& in, size_t n) {
	std::string buf(n, '\0');
	in.read(reinterpret_cast<uint8_t*>(&buf[0]), n);
	if (!is_valid_ascii(buf)) {
		// TODO: debug representation?
		throw std::domain_error("Invalid ASCII string");
	}
	return buf;
}

std::string read_cp1252_string(reader& in, size_t n) {
	return cp1252_to_utf8(in.read_vector(n));
}
