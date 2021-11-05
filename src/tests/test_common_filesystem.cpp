#include "common/filesystem.h"
#include "common/io/file.h"
#include "common/path.h"

#include <gtest/gtest.h>

TEST(io, filesystem) {
	const std::string tempname = "testfile.txt";
	{ filewriter out(tempname); }
	EXPECT_TRUE(filesystem::exists(tempname));
	EXPECT_FALSE(filesystem::exists("thisshouldnotexist.txt"));

	EXPECT_TRUE(filesystem::is_directory("CMakeFiles"));
	EXPECT_FALSE(filesystem::is_directory(tempname));

	for (auto d : filesystem::directory_iterator(".")) {
		// for (auto it = filesystem::directory_iterator(".");
		// it != filesystem::directory_iterator(); ++it) {
		// auto d = *it;
		// printf("d: %s\n", d.path().c_str());
	}
}

std::ostream& operator<<(std::ostream& out, const unixpath& p) {
	return out << p.to_string_lossy();
}
std::ostream& operator<<(std::ostream& out, const windowspath& p) {
	return out << p.to_string_lossy();
}

TEST(path, prefix) {
	EXPECT_EQ(unixpath().prefix(), "");
	EXPECT_EQ(unixpath("relative").prefix(), "");
	EXPECT_EQ(unixpath("/absolute").prefix(), "");

	// empty
	EXPECT_EQ(windowspath().prefix(), "");
	EXPECT_FALSE(windowspath().has_root());
	EXPECT_TRUE(windowspath().is_relative());
	EXPECT_FALSE(windowspath().is_absolute());
	// drive letters
	EXPECT_EQ(windowspath("c:\\").prefix(), "c:");
	EXPECT_EQ(windowspath("C:").prefix(), "C:");
	EXPECT_TRUE(windowspath("c:\\").has_root());
	EXPECT_TRUE(windowspath("c:\\").is_absolute());
	EXPECT_TRUE(windowspath("c:").is_relative());
	// device namespace
	EXPECT_EQ(windowspath("\\\\.\\COM42\\").prefix(), "\\\\.\\COM42");
	EXPECT_TRUE(windowspath("\\\\.\\COM42\\").has_root());
	EXPECT_TRUE(windowspath("\\\\.\\COM42\\").is_absolute());
	// UNC
	EXPECT_EQ(windowspath("\\\\servernoshare\\").prefix(), "");
	EXPECT_EQ(windowspath("\\\\server\\share\\").prefix(), "\\\\server\\share");
	// verbatim drive letter
	EXPECT_EQ(windowspath("\\\\?\\c:\\").prefix(), "\\\\?\\c:");
	// verbatim UNC
	EXPECT_EQ(windowspath("\\\\?\\UNC\\server\\share\\").prefix(),
		"\\\\?\\UNC\\server\\share");
	// verbatim
	EXPECT_EQ(windowspath("\\\\?\\relative").prefix(), "\\\\?\\");
	EXPECT_TRUE(windowspath("\\\\?\\relative").is_absolute());
}

TEST(path, push) {
	EXPECT_EQ(unixpath().join("b"), "b");
	EXPECT_EQ(unixpath("a").join("b"), "a/b");
	EXPECT_EQ(unixpath("a").join("/b"), "/b");

	EXPECT_EQ(windowspath().join("b"), "b");
	EXPECT_EQ(windowspath("a").join("b"), "a\\b");
	EXPECT_EQ(windowspath("a").join("\\b"), "\\b");
	EXPECT_EQ(windowspath("a").join("c:hi"), "c:hi");
	EXPECT_EQ(windowspath("a").join("\\\\?\\path"), "\\\\?\\path");
}

TEST(path, to_string_lossy) {
	EXPECT_EQ(unixpath("mypath").to_string_lossy(), "mypath");
	EXPECT_EQ(unixpath("tr\xe8s cool").to_string_lossy(), "tr\xef\xbf\xbds cool");
	EXPECT_EQ(windowspath("mypath").to_string_lossy(), "mypath");
	// can't even construct a windowspath with invalid UTF-8, wait until we
	// can construct one with invalid UTF-16
	// EXPECT_EQ(windowspath("tr\xe8s cool").to_string_lossy(),
	//           "tr\xef\xbf\xbds cool");
}

TEST(osstring, to_string_lossy) {
	EXPECT_EQ(unixosstring("mypath").to_string_lossy(), "mypath");
	EXPECT_EQ(unixosstring("tre\u00E8s cool").to_string_lossy(),
		"tre\u00E8s cool");
	EXPECT_EQ(unixosstring("tr\xe8s cool").to_string_lossy(), "tr\uFFFDs cool");

	EXPECT_EQ(windowsosstring("mypath").to_string_lossy(), "mypath");
	EXPECT_EQ(windowsosstring("tre\u00E8s cool").to_string_lossy(),
		"tre\u00E8s cool");
	EXPECT_THROW(windowsosstring("tr\xe8s cool"), std::domain_error);
	EXPECT_EQ(windowsosstring::from_wide(u"\xd801\xdc37").to_string_lossy(),
		"\U00010437");
	EXPECT_EQ(windowsosstring::from_wide(u"\xd801").to_string_lossy(), "\uFFFD");
}