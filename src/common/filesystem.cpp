#include "filesystem.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <sys/stat.h>
#endif

namespace filesystem {

#ifdef _WIN32
static DWORD GetFileAttributesFollowSymlinksA(const std::string& filename) {
	// TODO: use GetFileAttributesW
	DWORD attr = GetFileAttributesA(filename.c_str());
	if (attr != INVALID_FILE_ATTRIBUTES && attr & FILE_ATTRIBUTE_REPARSE_POINT) {
		// TODO: use CreateFileW
		HANDLE file_handle = CreateFileA(filename.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS);
		if (file_handle == INVALID_HANDLE_VALUE) {
			return INVALID_FILE_ATTRIBUTES;
		}
		BY_HANDLE_FILE_INFORMATION info;
		bool result = GetFileInformationByHandle(file_handle, &info);
		CloseHandle(file_handle); // TODO: handle error?
		if (result == 0) {
			return INVALID_FILE_ATTRIBUTES;
		}
		return info.dwFileAttributes;
	}
	return attr;
}
#endif

bool exists(const std::string& filename) {
#ifdef _WIN32
	// TODO: use GetFileAttributesW
	return GetFileAttributesFollowSymlinksA(filename.c_str()) != INVALID_FILE_ATTRIBUTES;
#else
	struct stat st;
	return stat(filename.c_str(), &st) == 0;
#endif
}

bool is_directory(const std::string& filename) {
#ifdef _WIN32
	// TODO: use GetFileAttributesW
	DWORD result = GetFileAttributesFollowSymlinks(filename.c_str());
	return result != INVALID_FILE_ATTRIBUTES && result & FILE_ATTRIBUTE_DIRECTORY;
#else
	struct stat st;
	return stat(filename.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
#endif
}

} // namespace filesystem