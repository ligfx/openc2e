#pragma once

#include <imgui.h>
#include <imgui_internal.h>

#include "audiobackend/AudioBackend.h"
#include "Engine.h"
#include "AgentInjector.h"
#include "Backend.h"
#include "BrainViewer.h"
#include "CreatureGrapher.h"
#include "Hatchery.h"
#include "World.h"
#include "fileformats/peFile.h"
#include "fileformats/ImageUtils.h"

int GetMainMenuBarHeightThisFrame() {
  int height = 0;
  if (ImGui::BeginMainMenuBar()) {
    height = ImGui::GetWindowSize().y;
    ImGui::EndMainMenuBar();
  }
  return height;
}

void DrawMainMenuBar() {
	// if (ImGui::BeginMainMenuBar()) {
	// 	if (ImGui::BeginMenu("File")) {
	// 		if (ImGui::MenuItem("Quit")) {
	// 			world.quitting = true;
	// 		}
	// 		ImGui::EndMenu();
	// 	}
	// 	if (ImGui::BeginMenu("View")) {
	// 		ImGui::MenuItem("Show Scrollbars", nullptr, false, false);
	// 		ImGui::EndMenu();
	// 	}
	// 	if (ImGui::BeginMenu("Control")) {
	// 		if (ImGui::MenuItem("Pause", nullptr, world.paused)) {
	// 			world.paused = !world.paused;
	// 		}
	// 		if (ImGui::MenuItem("Mute", nullptr, engine.audio->isMuted())) {
	// 			engine.audio->setMute(!engine.audio->isMuted());
	// 		}
	// 		ImGui::Separator();
	// 		if (ImGui::MenuItem("Fast speed", nullptr, engine.fastticks)) {
	// 			engine.fastticks = !engine.fastticks;
	// 		};
	// 		ImGui::MenuItem("Slow display updates", nullptr, false, false);
	// 		if (ImGui::MenuItem("Autokill", nullptr, world.autokill)) {
	// 			world.autokill = !world.autokill;
	// 		}
	// 		ImGui::EndMenu();
	// 	}
	// 	if (ImGui::BeginMenu("Debug")) {
	// 		if (ImGui::MenuItem("Show Map", nullptr, world.showrooms)) {
	// 			world.showrooms = !world.showrooms;
	// 		}
	// 		ImGui::MenuItem("Create a new (debug) Norn");
	// 		ImGui::MenuItem("Create a random egg");
	// 		ImGui::EndMenu();
	// 	}
	// 	if (ImGui::BeginMenu("Tools")) {
	// 		if (ImGui::MenuItem("Hatchery", nullptr, false, IsHatcheryEnabled())) {
  //       s_hatchery_open = true;
  //     }
	// 		if (ImGui::MenuItem("Agent Injector", nullptr, false, IsAgentInjectorEnabled())) {
	// 			s_agent_injector_open = true;
	// 		}
	// 		if (ImGui::MenuItem("Brain Viewer")) {
  //       s_brain_viewer_open = true;
  //     }
	// 		if (ImGui::MenuItem("Creature Grapher")) {
  //       s_creature_grapher_open = true;
  //     };
	// 		ImGui::EndMenu();
	// 	}
  // 
  //   ImGui::Text("Hello");
  //   ImGui::NewLine();
  //   ImGui::Text("Hello");
  // 
  //   ImGui::SetWindowSize(ImVec2(0, 200));
  // 
	// 	ImGui::EndMainMenuBar();
	// }
  
  // auto& style = ImGui::GetStyle();
  // style.WindowMinSize.y = 0;
  // style.WindowPadding.y = 0;
  
  ImGuiIO& io = ImGui::GetIO();
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, 0));
  if (ImGui::Begin("Top Window", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
  			if (ImGui::MenuItem("Quit")) {
  				world.quitting = true;
  			}
  			ImGui::EndMenu();
  		}
  		if (ImGui::BeginMenu("View")) {
  			ImGui::MenuItem("Show Scrollbars", nullptr, false, false);
  			ImGui::EndMenu();
  		}
  		if (ImGui::BeginMenu("Control")) {
  			if (ImGui::MenuItem("Pause", nullptr, world.paused)) {
  				world.paused = !world.paused;
  			}
  			if (ImGui::MenuItem("Mute", nullptr, engine.audio->isMuted())) {
  				engine.audio->setMute(!engine.audio->isMuted());
  			}
  			ImGui::Separator();
  			if (ImGui::MenuItem("Fast speed", nullptr, engine.fastticks)) {
  				engine.fastticks = !engine.fastticks;
  			};
  			ImGui::MenuItem("Slow display updates", nullptr, false, false);
  			if (ImGui::MenuItem("Autokill", nullptr, world.autokill)) {
  				world.autokill = !world.autokill;
  			}
  			ImGui::EndMenu();
  		}
  		if (ImGui::BeginMenu("Debug")) {
  			if (ImGui::MenuItem("Show Map", nullptr, world.showrooms)) {
  				world.showrooms = !world.showrooms;
  			}
  			ImGui::MenuItem("Create a new (debug) Norn");
  			ImGui::MenuItem("Create a random egg");
  			ImGui::EndMenu();
  		}
  		if (ImGui::BeginMenu("Tools")) {
  			// if (ImGui::MenuItem("Hatchery", nullptr, false, IsHatcheryEnabled())) {
        //   s_hatchery_open = true;
        // }
  			if (ImGui::MenuItem("Agent Injector", nullptr, false, IsAgentInjectorEnabled())) {
          SetAgentInjectorOpen(true);
  			}
  			if (ImGui::MenuItem("Brain Viewer")) {
          s_brain_viewer_open = true;
        }
  			if (ImGui::MenuItem("Creature Grapher")) {
          s_creature_grapher_open = true;
        };
  			ImGui::EndMenu();
  		}
      ImGui::EndMenuBar();
    }
    
    static Image s_stdicons;
    static Texture s_stdicons_tex;
    if (!s_stdicons.data) {
      s_stdicons = engine.getExeFile()->getBitmap(0xe3);
      s_stdicons.transparent_color = ImageUtils::GetPixelColor(s_stdicons, 0, 0);
      s_stdicons_tex = engine.backend->createTexture(s_stdicons);
    }
    
    Openc2eImgui::TextureRect icon_next{s_stdicons_tex, 0, 0, 22, 21};
    Openc2eImgui::TextureRect icon_eyeview{s_stdicons_tex, 22, 0, 22, 21};
    Openc2eImgui::TextureRect icon_track{s_stdicons_tex, 44, 0, 22, 21};
    Openc2eImgui::TextureRect icon_halo{s_stdicons_tex, 66, 0, 22, 21};
    Openc2eImgui::TextureRect icon_play{s_stdicons_tex, 88, 0, 22, 21};
    Openc2eImgui::TextureRect icon_pause{s_stdicons_tex, 110, 0, 22, 21};
    Openc2eImgui::TextureRect icon_help{s_stdicons_tex, 132, 0, 22, 21};
    Openc2eImgui::TextureRect icon_web{s_stdicons_tex, 154, 0, 22, 21};
    
    static Image s_handicons;
    static Texture s_handicons_tex;
    if (!s_handicons.data) {
      s_handicons = engine.getExeFile()->getBitmap(0xe4);
      s_handicons.transparent_color = ImageUtils::GetPixelColor(s_handicons, 0, 0);
      s_handicons_tex = engine.backend->createTexture(s_handicons);
    }
    
    Openc2eImgui::TextureRect icon_invisible{s_handicons_tex, 0, 0, 22, 21};
    Openc2eImgui::TextureRect icon_teach{s_handicons_tex, 22, 0, 22, 21};
    Openc2eImgui::TextureRect icon_push{s_handicons_tex, 44, 0, 22, 21};
    
    Openc2eImgui::ImageButton(icon_next, false);
    ImGui::SameLine();
    Openc2eImgui::ImageButton(icon_eyeview, false);
    ImGui::SameLine();
    Openc2eImgui::ImageButton(icon_track, false);
    ImGui::SameLine();
    Openc2eImgui::ImageButton(icon_halo, false);
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();
    if (Openc2eImgui::ImageButton(icon_play, world.paused)) {
      world.paused = false;
    };
    ImGui::SameLine();
    if (Openc2eImgui::ImageButton(icon_pause, !world.paused)) {
      world.paused = true;
    }
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();
    Openc2eImgui::ImageButton(icon_help, false);
    ImGui::SameLine();
    Openc2eImgui::ImageButton(icon_web, false);
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();
    Openc2eImgui::ImageButton(icon_invisible, false);
    ImGui::SameLine();
    Openc2eImgui::ImageButton(icon_teach, false);
    ImGui::SameLine();
    Openc2eImgui::ImageButton(icon_push);
    
    // 
    // 
    // for (int i = 0; i < s_stdicons_subs.size(); ++i) {
    //     if (i > 0) ImGui::SameLine();
    //     Openc2eImgui::ImageButton(s_stdicons_subs[i]);
    // }
    // 
    // for (int i = 0; i < s_stdicons.width / 22; ++i) {
    //   float ustart = 22.0 * i / s_stdicons.width;
    //   float uend = 22.0 * (i + 1) / s_stdicons.width;
    //   if (i > 0) ImGui::SameLine();
    //   Openc2eImgui::DisabledImageButton(s_stdicons_tex.as<ImTextureID>(), ImVec2(22, s_stdicons.height), ImVec2(ustart, 0), ImVec2(uend, 1.0));
    // }
    
    // ImGui::SameLine();
    // ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    // 
    // for (int i = 0; i < s_handicons.width / 22; ++i) {
    //   float ustart = 22.0 * i / s_handicons.width;
    //   float uend = 22.0 * (i + 1) / s_handicons.width;
    //   ImGui::SameLine();
    //   ImGui::ImageButton(s_handicons_tex.as<ImTextureID>(), ImVec2(22, s_handicons.height), ImVec2(ustart, 0), ImVec2(uend, 1.0));
    // }
    
    // 	toolbarnextaction = stdtoolbar->addAction(iconFromImageList(standardicons, 0), "Next");
    // 	toolbareyeviewaction = stdtoolbar->addAction(iconFromImageList(standardicons, 1), "Eye View");
    // 	toolbareyeviewaction->setCheckable(true);
    // 	toolbartrackaction = stdtoolbar->addAction(iconFromImageList(standardicons, 2), "Track");
    // 	toolbartrackaction->setCheckable(true);
    // 	toolbarhaloaction = stdtoolbar->addAction(iconFromImageList(standardicons, 3), "Halo");
    // 	toolbarhaloaction->setCheckable(true);
    
  // 	handicons = imageFromExeResource(0xe4);
  // 	favtoolbaricons = imageFromExeResource(0xe5);
  // 	appleticons = imageFromExeResource(0xe6);
  // 
  // 	for (unsigned int i = 0; i < 4; i++)
  // 		seasonicon[i] = imageFromExeResource(0x98 + i, false);
  // 
  // 	timeofdayicon[0] = imageFromExeResource(0xa3, false);
  // 	timeofdayicon[1] = imageFromExeResource(0xc4, false);
  // 	timeofdayicon[2] = imageFromExeResource(0xc2, false);
  // 	timeofdayicon[3] = imageFromExeResource(0xc3, false);
  // 	timeofdayicon[4] = imageFromExeResource(0xc5, false);
  // 
  // 	for (unsigned int i = 0; i < 5; i++)
  // 		temperatureicon[i] = imageFromExeResource(0xa4 + i, false);
  // 
  // 	healthicon[0] = imageFromExeResource(0xe8); // disabled (gray)
  // 	healthicon[1] = imageFromExeResource(0xc7); // 0/4
  // 	healthicon[2] = imageFromExeResource(0xac); // 1/4
  // 	healthicon[3] = imageFromExeResource(0xad); // 2/4
  // 	healthicon[4] = imageFromExeResource(0xab); // 3/4
  // 	healthicon[5] = imageFromExeResource(0xaa); // 4/4
  // 
  // 	hearticon[0] = imageFromExeResource(0xe9); // disabled (gray)
  // 	hearticon[1] = imageFromExeResource(0xae); // large
  // 	hearticon[2] = imageFromExeResource(0xaf); // medium
  // 	hearticon[3] = imageFromExeResource(0xb0); // small
  // 	hearticon[4] = imageFromExeResource(0xe7); // dead (blue)
  // }
  // 
  // void QtOpenc2e::createC2Toolbars() {
  // 	QToolBar *stdtoolbar = new QToolBar("Standard", this);
  // 	stdtoolbar->setIconSize(QSize(22, 21));
  // 
  // 	toolbarnextaction = stdtoolbar->addAction(iconFromImageList(standardicons, 0), "Next");
  // 	toolbareyeviewaction = stdtoolbar->addAction(iconFromImageList(standardicons, 1), "Eye View");
  // 	toolbareyeviewaction->setCheckable(true);
  // 	toolbartrackaction = stdtoolbar->addAction(iconFromImageList(standardicons, 2), "Track");
  // 	toolbartrackaction->setCheckable(true);
  // 	toolbarhaloaction = stdtoolbar->addAction(iconFromImageList(standardicons, 3), "Halo");
  // 	toolbarhaloaction->setCheckable(true);
  // 
  // 	toolbarplayaction = stdtoolbar->addAction(iconFromImageList(standardicons, 4), "Play");
  // 	stdtoolbar->insertSeparator(toolbarplayaction);
  // 	toolbarpauseaction = stdtoolbar->addAction(iconFromImageList(standardicons, 5), "Pause");
  // 	QActionGroup *playpausegroup = new QActionGroup(this);
  // 	playpausegroup->addAction(toolbarplayaction);
  // 	playpausegroup->addAction(toolbarpauseaction);
  // 	toolbarplayaction->setCheckable(true);
  // 	toolbarpauseaction->setCheckable(true);
  // 	toolbarplayaction->setChecked(true);
  // 
  // 	toolbarhelpaction = stdtoolbar->addAction(iconFromImageList(standardicons, 6), "Help");
  // 	stdtoolbar->insertSeparator(toolbarhelpaction);
  // 	toolbarwebaction = stdtoolbar->addAction(iconFromImageList(standardicons, 7), "Web");
  // 
  // 	addToolBar(stdtoolbar);
    
    // 
    // static SDL_Texture *s_nornbutton = nullptr;
    // if (!s_nornbutton) {
    //   bmpImage bmp("nornbutton.bmp");
    //   s_nornbutton = CreateTextureFromBmp(bmp);
    // }
    // 
    // ImGui::ImageButton(s_nornbutton, ImVec2(29, 29));
    // 
    // ImGui::Text("Hello");
  
    ImGui::End();
  }
}
