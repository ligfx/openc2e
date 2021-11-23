#include "common/io/file.h"
#include "common/io/io.h"
#include "common/io/owningstringreader.h"
#include "common/io/spanreader.h"
#include "common/io/text.h"
#include "common/io/vectorwriter.h"
#include "common/mappedfile.h"

#include <gtest/gtest.h>

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

TEST(common_io, filewriter_existing_file) {
	ADD_FAILURE();
}

TEST(common_io, ignore) {
	ADD_FAILURE();
}

TEST(common_io, owningstringreader) {
	ADD_FAILURE();
}

TEST(common_io, read_text_int) {
	{
		owningstringreader s(" +3  ");
		EXPECT_EQ(read_text_int(s), 3);
	}
	{
		owningstringreader s("-37");
		EXPECT_EQ(read_text_int(s), -37);
	}
	{
		owningstringreader s("3hello");
		EXPECT_EQ(read_text_int(s), 3);
		std::string hello = read_ascii_string(s, 5);
		EXPECT_EQ(hello, "hello");
	}
}

TEST(common_io, read_text_float) {
	{
		owningstringreader s(" +3.14  ");
		EXPECT_FLOAT_EQ(read_text_float(s), 3.14);
	}
	{
		owningstringreader s("5.");
		EXPECT_FLOAT_EQ(read_text_float(s), 5);
	}
	{
		owningstringreader s("-37");
		EXPECT_FLOAT_EQ(read_text_float(s), -37);
	}
	{
		owningstringreader s("3hello");
		EXPECT_FLOAT_EQ(read_text_float(s), 3);
		std::string hello = read_ascii_string(s, 5);
		EXPECT_EQ(hello, "hello");
	}
}

TEST(common_io, read_until) {
	owningstringreader s("hi\nthere");
	std::vector<uint8_t> buf = read_until(s, '\n');
	EXPECT_EQ(buf, std::vector<uint8_t>({'h', 'i', '\n'}));
}

TEST(common_io, read_ascii_line) {
	{
		owningstringreader s("hi\nthere");
		EXPECT_EQ(read_ascii_line(s), "hi\n");
	}
	{
		owningstringreader s("tr\xe8s cool"); // CP1252
		EXPECT_THROW(read_ascii_line(s), std::domain_error);
	}
}

TEST(common_io, read_ascii_string) {
	{
		owningstringreader s("hello");
		EXPECT_EQ(read_ascii_string(s, 5), "hello");
	}
	{
		owningstringreader s("tr\xe8s cool"); // CP1252
		EXPECT_THROW(read_ascii_string(s, 5), std::domain_error);
	}
}

TEST(common_io, read_cp1252_line) {
	owningstringreader s("tr\xe8s cool\nhi there\n");
	EXPECT_EQ(read_cp1252_line(s), "tr\xc3\xa8s cool\n");
	// TODO: don't allow control/nonprintable characters, as they're likely not CP1252?
}

TEST(common_io, read_cp1252_string) {
	owningstringreader s("tr\xe8s cool");
	EXPECT_EQ(read_cp1252_string(s, 9), "tr\xc3\xa8s cool");
	// TODO: don't allow control/nonprintable characters, as they're likely not CP1252?
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
