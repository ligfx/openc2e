#pragma once

#include <imgui.h>

static bool s_brain_viewer_open = false;

void DrawBrainViewer() {
  if (s_brain_viewer_open && ImGui::Begin("Brain Viewer", &s_brain_viewer_open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse)) {
  
    // TODO: fix old braininavat code
    ImGui::Dummy(ImVec2(320, 275));
  
    ImGui::End();
  }
}