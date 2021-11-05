#include "spanreader.h"

#include <string.h>


size_t spanreader::read_some(uint8_t* buf, size_t n) {
	if (m_buffer == nullptr || m_position >= m_buffer_size || n == 0) {
		return 0;
	}
	if (buf == nullptr) {
		throw io_error("Attempted reading into a null buffer");
	}
	if (n > m_buffer_size - m_position) {
		n = m_buffer_size - m_position;
	}
	memcpy(buf, m_buffer + m_position, n);
	m_position += n;
	return n;
}

void spanreader::seek(int64_t offset, seek_type whence) {
	// TODO: disallow seeking past end?
	switch (whence) {
		case seek_type::seek_set:
			if (offset < 0) {
				throw io_error("Attemped seeking before beginning of spanreader");
			} else {
				m_position = offset;
			}
			break;
		case seek_type::seek_cur:
			if (offset < 0 && static_cast<size_t>(-offset) > m_position) {
				throw io_error("Attemped seeking before beginning of spanreader");
			} else {
				// TODO: overflow?
				m_position += offset;
			}
			break;
		case seek_type::seek_end:
			if (offset < 0 && static_cast<size_t>(-offset) > m_buffer_size) {
				throw io_error("Attemped seeking before beginning of spanreader");
			} else {
				// TODO: overflow?
				m_position = m_buffer_size + offset;
			}
	}
}

size_t spanreader::tell() {
	return m_position;
}
