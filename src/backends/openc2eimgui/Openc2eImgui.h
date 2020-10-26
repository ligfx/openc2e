#pragma once

#include <imgui.h>
#include "Backend.h"
#include "Engine.h"
#include "fileformats/peFile.h"
#include "fileformats/ImageUtils.h"
#include "Texture.h"

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator+(const ImVec2& lhs, double rhs) { return ImVec2(lhs.x + rhs, lhs.y + rhs); }

static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, double rhs) { return ImVec2(lhs.x - rhs, lhs.y - rhs); }

static inline ImVec2 operator*(double lhs, const ImVec2& rhs) { return ImVec2(lhs * rhs.x, lhs * rhs.y); }

static inline ImVec2 operator/(const ImVec2& lhs, double rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }

inline Texture GetTextureFromExeFile(uint32_t resource) {
  return engine.backend->createTexture(engine.getExeFile()->getBitmap(resource));
}

inline Texture GetTextureWithTransparencyFromExeFile(uint32_t resource) {
  auto image = engine.getExeFile()->getBitmap(resource);
  // TODO: don't make all pixels of this color transparent, only pixels of this
  // color that are connected to the edges of the image
  return engine.backend->createTextureWithTransparentColor(image, ImageUtils::GetPixelColor(image, 0, 0));
}

namespace Openc2eImgui {

bool BeginWindow(const char *title, bool* is_open);
void EndWindow();

class TextureRect {
public:
  Texture parent;
  unsigned int x;
  unsigned int y;
  unsigned int w;
  unsigned int h;
};

void Image(Texture texture);

void DisabledButton(const char* text);
bool ImageButton(Texture texture, bool enabled = true);
bool ImageButton(TextureRect texture, bool enabled = true);

void BeginVerticalLayout(const char *name);
void SetNextStretchMinSize(ImVec2 minimum);
ImVec2 GetStretchSize();
void EndVerticalLayout();

} // namespace Openc2eImgui