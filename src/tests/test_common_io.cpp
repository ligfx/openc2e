#include "common/io/file.h"
#include "common/io/spanreader.h"
#include "common/io/vectorwriter.h"
#include "common/mappedfile.h"

#include <gtest/gtest.h>
#include <iostream>

const std::string contents = R"(
      some file contents
  )";

TEST(common_io, file) {
	const std::string tempname = "testfile.txt";
	{
		filewriter out(tempname);
		out.write((const uint8_t*)contents.data(), contents.size());
	}
	{
		filereader in(tempname);
		std::vector<uint8_t> _buf = in.read_to_end();
		std::string buf(_buf.begin(), _buf.end());
		EXPECT_EQ(contents, buf);
	}
}

TEST(common_io, spanreader) {
	spanreader in((const uint8_t*)contents.data(), contents.size());
	std::vector<uint8_t> _buf = in.read_to_end();
	std::string buf(_buf.begin(), _buf.end());
	EXPECT_EQ(contents, buf);
}

TEST(common_io, vectorwriter) {
	vectorwriter v;
	v.write((const uint8_t*)contents.data(), contents.size());

	std::string buf(v.vector().begin(), v.vector().end());
	EXPECT_EQ(contents, buf);
}

TEST(common_io, mmapreader) {
	const std::string tempname = "testfile.txt";
	{
		filewriter out(tempname);
		out.write((const uint8_t*)contents.data(), contents.size());
	}
	{
		mappedfile mm(tempname);
		spanreader in(mm);

		std::vector<uint8_t> _buf = in.read_to_end();
		std::string buf(_buf.begin(), _buf.end());
		EXPECT_EQ(contents, buf);
	}
}
