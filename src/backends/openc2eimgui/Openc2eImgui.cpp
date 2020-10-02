#include "Openc2eImgui.h"

#include <fmt/format.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <unordered_map>


void Openc2eImgui::DisabledButton(const char* text) {
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	ImGui::Button(text);
	ImGui::PopItemFlag();
	ImGui::PopStyleVar();	
}

bool Openc2eImgui::ImageButton(TextureRect tex, bool enabled) {
	ImVec4 tint(1, 1, 1, 1);
	if (!enabled) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		tint = ImVec4(1, 1, 1, 0.8);
	}
	
	// ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
	// ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
	ImGui::PushID(fmt::format("{}_{}_{}_{}_{}", (uintptr_t)tex.parent.as<void*>(), tex.x, tex.y, tex.w, tex.h).c_str());
	bool ret = ImGui::ImageButton(
		tex.parent.as<ImTextureID>(),
		ImVec2(tex.w, tex.h),
		ImVec2(tex.x * 1.0 / tex.parent.width, tex.y * 1.0 / tex.parent.height),
		ImVec2((tex.x + tex.w) * 1.0 / tex.parent.width, (tex.y + tex.h) * 1.0 / tex.parent.height),
		-1, // padding
		ImVec4(0, 0, 0, 0), // bgcolor
		tint
	);
	ImGui::PopID();
	// ImGui::PopStyleColor();
	// ImGui::PopStyleColor();
	
	if (!enabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
	
	return ret;
}

// void Openc2eImgui::DisabledImageButton(TextureRect texture) {
// 	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
// 	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
// 	ImGui::ImageButton(
// 		texture,
// 		ImVec2(texture.w, texture.h),
// 		ImVec2(texture.x * 1.0 / texture.w, texture.y * 1.0 / texture.h),
// 		ImVec2(texture.x * 1.0 / texture.w, texture.y * 1.0 / texture.h),,
// 		-1, // padding
// 		ImVec4(0,0,0,0), // bgcolor,
// 		ImVec4(1,1,1,0.5) // tint
// 	);
// 	ImGui::PopItemFlag();
// 	ImGui::PopStyleVar();	
// }

struct VerticalLayoutInfo {
	unsigned int top_y = 0;
	unsigned int last_frame_fixed_height = 0;
	unsigned int this_frame_stretch_height = 0;
	bool has_seen_everything_once = false;
	ImVec2 current_item_size;
};

static std::unordered_map<std::string, VerticalLayoutInfo> s_vertical_layouts;
static VerticalLayoutInfo *s_current_vertical_layout = nullptr;

ImVec2 Openc2eImgui::GetStretchSize() {
	assert(s_current_vertical_layout);
	return s_current_vertical_layout->current_item_size;
}

void Openc2eImgui::BeginVerticalLayout(const char *name) {
	if (!s_vertical_layouts.count(name)) {
		s_vertical_layouts[name] = VerticalLayoutInfo();
	}
	s_current_vertical_layout = &s_vertical_layouts[name];
	
	s_current_vertical_layout->top_y = ImGui::GetCursorPosY();
	s_current_vertical_layout->this_frame_stretch_height = 0;
}

void Openc2eImgui::EndVerticalLayout() {
	assert(s_current_vertical_layout);
	s_current_vertical_layout->last_frame_fixed_height =
		ImGui::GetCursorPosY()
		- s_current_vertical_layout->top_y
		- s_current_vertical_layout->this_frame_stretch_height
	;
	s_current_vertical_layout->has_seen_everything_once = true;
	s_current_vertical_layout = nullptr;
}

void Openc2eImgui::SetNextStretchMinSize(ImVec2 minimum) {
	assert(s_current_vertical_layout);
	float window_content_region_width = ImGui::GetWindowContentRegionWidth();
	float window_content_region_height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;
	
	if (s_current_vertical_layout->has_seen_everything_once) {
		ImVec2 available(
			std::max(minimum.x, window_content_region_width),
			std::max(minimum.y, window_content_region_height - s_current_vertical_layout->last_frame_fixed_height)
		);
		s_current_vertical_layout->this_frame_stretch_height += available.y;
		s_current_vertical_layout->current_item_size = available;
	} else {
		s_current_vertical_layout->this_frame_stretch_height += minimum.y;
		s_current_vertical_layout->current_item_size = minimum;
	}
}

bool Openc2eImgui::BeginWindow(const char *title, bool* is_open) {
  if (!*is_open) {
    return false;
  }
	bool ret = ImGui::Begin(title, is_open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);
  if (ret) {
      BeginVerticalLayout(title);
  }
  return ret;
}
void Openc2eImgui::EndWindow() {
  EndVerticalLayout();
	return ImGui::End();
}