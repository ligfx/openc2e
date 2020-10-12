#pragma once

#include "MainMenu.h"

static int s_last_c1toolbar_height = 0;

int GetC1ToolBarHeight() {
  return s_last_c1toolbar_height;
}

void DrawC1ToolBar() {  
  if (ImGui::BeginMainMenuBar()) {
    DrawMainMenu();
    s_last_c1toolbar_height = ImGui::GetWindowSize().y;
    ImGui::EndMainMenuBar();
  }
}