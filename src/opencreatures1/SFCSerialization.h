#pragma once

#include "Renderable.h"
#include "fileformats/NewSFCFile.h"

void sfc_load_everything(const sfc::SFCFile&);
sfc::SFCFile sfc_dump_everything();
Renderable sfc_load_renderable(const sfc::EntityV1* part);
std::shared_ptr<sfc::EntityV1> sfc_dump_renderable(const Renderable& r, const std::shared_ptr<sfc::CGalleryV1>& gallery = {});