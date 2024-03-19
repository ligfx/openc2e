#pragma once

#include "Renderable.h"
#include "fileformats/NewSFCFile.h"
#include "objects/ObjectHandle.h"

#include <map>
#include <memory>

class Object;

namespace sfc {
struct SFCFile;
struct ObjectV1;
} // namespace sfc

struct SFCLoader {
	SFCLoader(sfc::SFCFile* sfc_)
		: sfc(sfc_) {}

	void load_everything();
	void load_objects_and_sceneries();

	ObjectHandle load_object(sfc::ObjectV1* p);
	std::shared_ptr<sfc::ObjectV1> dump_object(Object*) {
		throw Exception("Can't call dump_object on a SFCLoader");
	}

	constexpr bool is_loading() const { return true; }
	constexpr bool is_storing() const { return false; }

	sfc::SFCFile* sfc;
	std::map<const sfc::ObjectV1*, ObjectHandle> sfc_object_mapping;
};

struct SFCSaver {
	SFCSaver(sfc::SFCFile* sfc_)
		: sfc(sfc_) {}

	std::shared_ptr<sfc::ObjectV1> dump_object(Object*);
	ObjectHandle load_object(sfc::ObjectV1*) {
		throw Exception("Can't call load_object on a SFCSaver");
	}

	sfc::SFCFile* sfc;
	std::map<Object*, std::shared_ptr<sfc::ObjectV1>> sfc_object_mapping;

	constexpr bool is_loading() const { return false; }
	constexpr bool is_storing() const { return true; }
};

void sfc_load_everything(const sfc::SFCFile&);
sfc::SFCFile sfc_dump_everything();
Renderable sfc_load_renderable(const sfc::EntityV1* part);
std::shared_ptr<sfc::EntityV1> sfc_dump_renderable(const Renderable& r, const std::shared_ptr<sfc::CGalleryV1>& gallery = {});