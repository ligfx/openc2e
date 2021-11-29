/*
 *  endianlove.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat May 22 2004.
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

#include "common/io/io.h"

#include <stdint.h>
#include <string.h>
#ifdef _WIN32
#include <stdlib.h>
#endif

/*

This file used to handle integer encodings in a platform-neutral way using byte
shifts and ors, in the same vein as Rob Pike's article "The Byte Order Fallacy":

e.g. a little-endian uint32 would be read as (buf[0] << 0) | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

Unfortunately, this is not optimized out on all platforms (*cough*MSVC*cough*),
and even when it is optimized for the singular case it does not optimize well
for the "readmany" case.

So, we do platform endianness detection and byte swapping.

*/

static inline bool is_little_endian() {
	// gets optimized to a constant, inlined, and used to eliminate dead code on
	// mainstream compilers
	const uint32_t value = 0x04030201;
	return *static_cast<const uint8_t*>(static_cast<const void*>(&value)) == 0x01;
}

static inline uint16_t byte_swap_16(uint16_t val) {
#ifdef _WIN32
	return _byteswap_ushort(val);
#else
	// this is optimized down to a rotation on Clang and GCC
	return ((val & 0xff00) >> 8) | ((val & 0xff) << 8);
#endif
}

static inline uint32_t byte_swap_32(uint32_t val) {
	// this is optimized down to a bswap on Clang, GCC, and MSVC
	return ((val & 0xff000000) >> 24) | ((val & 0xff0000) >> 8) | ((val & 0xff00) << 8) | ((val & 0xff) << 24);
}

static inline uint8_t read8(reader& s) {
	uint8_t t;
	s.read(reinterpret_cast<uint8_t*>(&t), 1);
	return t;
}

static inline void write8(writer& s, uint8_t v) {
	s.write(reinterpret_cast<uint8_t*>(&v), 1);
}

static inline uint16_t read16le(const uint8_t* buf) {
	uint16_t val;
	memcpy(&val, buf, 2);
	return is_little_endian() ? val : byte_swap_16(val);
}

static inline uint16_t read16le(reader& s) {
	uint16_t val;
	s.read(reinterpret_cast<uint8_t*>(&val), 2);
	return is_little_endian() ? val : byte_swap_16(val);
}

static inline void write16le(writer& s, uint16_t v) {
	uint16_t t = is_little_endian() ? v : byte_swap_16(v);
	s.write(reinterpret_cast<uint8_t*>(&t), 2);
}

static inline void readmany16le(reader& s, uint16_t* out, size_t n) {
	// this needs to be fast! it's used in the sprite file reading functions
	s.read(reinterpret_cast<uint8_t*>(out), n * 2);
	if (!is_little_endian()) {
		for (size_t i = 0; i < n; ++i) {
			out[i] = byte_swap_16(out[i]);
		}
	}
}

static inline uint32_t read32le(reader& s) {
	uint32_t t;
	s.read(reinterpret_cast<uint8_t*>(&t), 4);
	return is_little_endian() ? t : byte_swap_32(t);
}

static inline void write32le(writer& s, uint32_t v) {
	uint32_t t = is_little_endian() ? v : byte_swap_32(v);
	s.write(reinterpret_cast<uint8_t*>(&t), 4);
}

static inline void write32le(uint8_t* b, uint32_t v) {
	uint32_t t = is_little_endian() ? v : byte_swap_32(v);
	memcpy(b, &t, 4);
}

// big-endian integers are used in two places: c2e genome files (yes, for some
// crazy reason!), and macOS versions of spritefiles (.m16, .n16, and .blk)

static inline uint16_t read16be(reader& s) {
	uint16_t t;
	s.read(reinterpret_cast<uint8_t*>(&t), 2);
	return is_little_endian() ? byte_swap_16(t) : t;
}

static inline void write16be(writer& s, uint16_t v) {
	uint16_t t = is_little_endian() ? byte_swap_16(v) : v;
	s.write(reinterpret_cast<uint8_t*>(&t), 2);
}

static inline void readmany16be(reader& s, uint16_t* out, size_t n) {
	s.read(reinterpret_cast<uint8_t*>(out), n * 2);
	if (is_little_endian()) {
		for (size_t i = 0; i < n; ++i) {
			// gets optimized into fast SIMD instructions on Clang and MSVC at
			// -O2, and on GCC at -O3
			out[i] = byte_swap_16(out[i]);
		}
	}
}

static inline uint32_t read32be(reader& s) {
	uint32_t t;
	s.read(reinterpret_cast<uint8_t*>(&t), 4);
	return is_little_endian() ? byte_swap_32(t) : t;
}