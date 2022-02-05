#pragma once

#include "common/Exception.h"

#include <iosfwd>
#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

// Windows MFC serialized data / CArchive serialized data

class MFCReader;

class MFCObject {
  public:
	virtual ~MFCObject() = default;
	virtual void read_from(MFCReader&) = 0;
};

class MFCReader {
  public:
	MFCReader(std::istream& in);
	~MFCReader();

	const std::vector<std::unique_ptr<MFCObject>>& objects() const;
	std::vector<std::unique_ptr<MFCObject>>&& release_objects();

	std::string read_ascii(size_t n);
	std::string read_ascii_nullterminated(size_t n);
	std::string read_ascii_mfcstring();
	uint8_t read8();
	int8_t reads8();
	uint16_t read16le();
	int16_t reads16le();
	uint32_t read32le();
	int32_t reads32le();
	template <typename T>
	T* read_type() {
		MFCObject* base_object = read_object();
		if (base_object == nullptr) {
			return nullptr;
		}
		T* object = dynamic_cast<T*>(base_object);
		if (object == nullptr) {
			// TODO: name of type we got, name of type we expected
			throw Exception("Wrong object type");
		}
		return object;
	}

	template <typename T>
	void register_class(const std::string& name, int schema_number) {
		ClassInfo info;
		info.newfunc = [] { return static_cast<MFCObject*>(new T()); };
		m_classregistry[std::make_pair(name, schema_number)] = info;
	}

  private:
	struct ClassInfo {
		MFCObject* (*newfunc)();
	};

	MFCObject* read_object();

	std::istream& m_in;

	std::map<std::pair<std::string, int>, ClassInfo> m_classregistry;
	std::map<uint16_t, ClassInfo> m_classids;
	std::vector<std::unique_ptr<MFCObject>> m_objects;
};
