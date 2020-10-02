#pragma once

#include <imgui.h>
#include "Texture.h"

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator+(const ImVec2& lhs, double rhs) { return ImVec2(lhs.x + rhs, lhs.y + rhs); }

static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, double rhs) { return ImVec2(lhs.x - rhs, lhs.y - rhs); }

static inline ImVec2 operator*(double lhs, const ImVec2& rhs) { return ImVec2(lhs * rhs.x, lhs * rhs.y); }

static inline ImVec2 operator/(const ImVec2& lhs, double rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }

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

void DisabledButton(const char* text);
bool ImageButton(TextureRect texture, bool enabled = true);

void BeginVerticalLayout(const char *name);
void SetNextStretchMinSize(ImVec2 minimum);
ImVec2 GetStretchSize();
void EndVerticalLayout();

} // namespace Openc2eImgui