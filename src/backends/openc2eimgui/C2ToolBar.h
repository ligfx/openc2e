#pragma once

#include "Backend.h"
#include "Engine.h"
#include "fileformats/ImageUtils.h"
#include "fileformats/peFile.h"
#include "Openc2eImgui.h"
#include "World.h"

static int s_last_c2toolbar_height = 0;

int GetC2ToolBarHeight() {
  return s_last_c2toolbar_height;
}

void DrawC2Toolbar() {
  ImGuiIO& io = ImGui::GetIO();
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, 0));
  
  if (ImGui::Begin("MainMenuBar", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
    if (ImGui::BeginMenuBar()) {  
      DrawMainMenu();
      ImGui::EndMenuBar();
    }
  
    static Texture s_stdicons_tex;
    if (!s_stdicons_tex) {
      s_stdicons_tex = GetTextureWithTransparencyFromExeFile(0xe3);
    }
    
    Openc2eImgui::TextureRect icon_next{s_stdicons_tex, 0, 0, 22, 21};
    Openc2eImgui::TextureRect icon_eyeview{s_stdicons_tex, 22, 0, 22, 21};
    Openc2eImgui::TextureRect icon_track{s_stdicons_tex, 44, 0, 22, 21};
    Openc2eImgui::TextureRect icon_halo{s_stdicons_tex, 66, 0, 22, 21};
    Openc2eImgui::TextureRect icon_play{s_stdicons_tex, 88, 0, 22, 21};
    Openc2eImgui::TextureRect icon_pause{s_stdicons_tex, 110, 0, 22, 21};
    Openc2eImgui::TextureRect icon_help{s_stdicons_tex, 132, 0, 22, 21};
    Openc2eImgui::TextureRect icon_web{s_stdicons_tex, 154, 0, 22, 21};
    
    static Texture s_handicons_tex;
    if (!s_handicons_tex) {
      s_handicons_tex = GetTextureWithTransparencyFromExeFile(0xe4);
    }
    
    Openc2eImgui::TextureRect icon_invisible{s_handicons_tex, 0, 0, 22, 21};
    Openc2eImgui::TextureRect icon_teach{s_handicons_tex, 22, 0, 22, 21};
    Openc2eImgui::TextureRect icon_push{s_handicons_tex, 44, 0, 22, 21};
    
    ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Horizontal;
    
    Openc2eImgui::ImageButton(icon_next, false);
    Openc2eImgui::ImageButton(icon_eyeview, false);
    Openc2eImgui::ImageButton(icon_track, false);
    Openc2eImgui::ImageButton(icon_halo, false);
    ImGui::Separator();
    if (Openc2eImgui::ImageButton(icon_play, world.paused)) {
      world.paused = false;
    };
    if (Openc2eImgui::ImageButton(icon_pause, !world.paused)) {
      world.paused = true;
    }
    ImGui::Separator();
    Openc2eImgui::ImageButton(icon_help, false);
    Openc2eImgui::ImageButton(icon_web, false);
    ImGui::Separator();
    Openc2eImgui::ImageButton(icon_invisible, false);
    Openc2eImgui::ImageButton(icon_teach, false);
    Openc2eImgui::ImageButton(icon_push, false);
    
    ImGui::Separator();
    
    static Texture s_appleticons_tex;
    if (!s_appleticons_tex) {
      s_appleticons_tex = GetTextureWithTransparencyFromExeFile(0xe6);
    }

    Openc2eImgui::TextureRect icon_hatchery{s_appleticons_tex, 0, 0, 22, 21};
    Openc2eImgui::TextureRect icon_unknown_maybe_ecologykit{s_appleticons_tex, 22, 0, 22, 21};
    Openc2eImgui::TextureRect icon_ownerskit{s_appleticons_tex, 44, 0, 22, 21};
    Openc2eImgui::TextureRect icon_healthkit{s_appleticons_tex, 66, 0, 22, 21};
    Openc2eImgui::TextureRect icon_unknown_maybe_sciencekit{s_appleticons_tex, 88, 0, 22, 21};
    Openc2eImgui::TextureRect icon_unknown_maybe_breederskit{s_appleticons_tex, 110, 0, 22, 21};
    Openc2eImgui::TextureRect icon_unknown_maybe_observationkit{s_appleticons_tex, 132, 0, 22, 21};
    Openc2eImgui::TextureRect icon_agent_injector{s_appleticons_tex, 154, 0, 22, 21};
    Openc2eImgui::TextureRect icon_unknown_maybe_historykit{s_appleticons_tex, 176, 0, 22, 21};
    Openc2eImgui::TextureRect icon_graveyard{s_appleticons_tex, 198, 0, 22, 21};
    Openc2eImgui::TextureRect icon_unknown_globe{s_appleticons_tex, 220, 0, 22, 21};
    Openc2eImgui::TextureRect icon_unknown_magnifyingglass{s_appleticons_tex, 242, 0, 22, 21};
    Openc2eImgui::TextureRect icon_unknown_maybe_neurosciencekit{s_appleticons_tex, 264, 0, 22, 21};
    
    Openc2eImgui::ImageButton(icon_hatchery);
    Openc2eImgui::ImageButton(icon_ownerskit, false);
    Openc2eImgui::ImageButton(icon_healthkit, false);
    Openc2eImgui::ImageButton(icon_graveyard, false);
    ImGui::Separator();
    Openc2eImgui::ImageButton(icon_unknown_maybe_breederskit, false);
    Openc2eImgui::ImageButton(icon_unknown_maybe_sciencekit, false);
    Openc2eImgui::ImageButton(icon_unknown_maybe_neurosciencekit, false);
    Openc2eImgui::ImageButton(icon_unknown_maybe_observationkit, false);
    ImGui::Separator();
    if (Openc2eImgui::ImageButton(icon_agent_injector, IsAgentInjectorEnabled())) {
      SetAgentInjectorOpen(true);
    }
    Openc2eImgui::ImageButton(icon_unknown_maybe_historykit, false);
    Openc2eImgui::ImageButton(icon_unknown_maybe_ecologykit, false);
    
    ImGui::NewLine();
    
    static Texture s_favtoolbaricons_tex;
    if (!s_favtoolbaricons_tex) {
      s_favtoolbaricons_tex = GetTextureWithTransparencyFromExeFile(0xe5);
    }
    Openc2eImgui::TextureRect icon_say{s_favtoolbaricons_tex, 0, 0, 22, 21};
    Openc2eImgui::TextureRect icon_go{s_favtoolbaricons_tex, 22, 0, 22, 21};
    
    // ImGui::GetCurrentWindow()->DC.LayoutType = ImGuiLayoutType_Vertical;
    ImGui::Text("Speech History:");
    ImGui::SetNextItemWidth(150);
    if (ImGui::BeginCombo("##Speech History:", "")) {
      ImGui::EndCombo();
    }
    Openc2eImgui::ImageButton(icon_say);
    ImGui::Separator();
    
    ImGui::Text("Places:");
    ImGui::SetNextItemWidth(150);
    if (ImGui::BeginCombo("##Places:", "The Incubator")) {
      ImGui::EndCombo();
    }
    Openc2eImgui::ImageButton(icon_go);

    s_last_c2toolbar_height = ImGui::GetWindowSize().y;
    ImGui::End();
  }
}