#pragma once

#include "Openc2eImgui.h"
#include <imgui.h>

static bool s_creature_grapher_open = true;

void DrawCreatureGrapher() {
  if (Openc2eImgui::BeginWindow("Creature Grapher", &s_creature_grapher_open)) {
    
    // float values[] = {0, 0.5, -0.5, 1, -1};
  
    // ImGui::PlotLines("##Creature Grapher Graph", values, 5, 0, nullptr, -1, 1, ImVec2(320, 275));
    
    // ImGui::PlotLines("##Creature Grapher Graph 2", values, 5, 2, nullptr, -1, 1, ImVec2(320, 275));
    
    {
      Openc2eImgui::SetNextStretchMinSize(ImVec2(300, 200));
      auto size = Openc2eImgui::GetStretchSize();
      ImDrawList* drawlist = ImGui::GetWindowDrawList();
      ImVec2 p = ImGui::GetCursorScreenPos();
      
      drawlist->AddLine(p + ImVec2(0, 100), p + ImVec2(20, 130), 0xffffffff);
      ImGui::Dummy(size);
    }
  
    float content_width = ImGui::GetWindowContentRegionWidth();
  
    if (ImGui::ListBoxHeader("##Creature Grapher - Chemical Groups", ImVec2(content_width / 2, 0))) {
      // TODO: get chem groups
      ImGui::Selectable("All", true);
      
      ImGui::ListBoxFooter();
    }
    
    ImGui::SameLine();
    
    if (ImGui::ListBoxHeader("##Creature Grapher - Chemicals", ImVec2(content_width / 2, 0))) {
      // TODO: get chemicals
      bool is_selected = false;
      ImGui::Checkbox("Lactate", &is_selected);
      ImGui::Checkbox("Pyruvate", &is_selected);
      ImGui::Checkbox("Glucose", &is_selected);
      ImGui::Checkbox("Glycogen", &is_selected);
      ImGui::Checkbox("Starch", &is_selected);
      ImGui::Checkbox("Fatty Acid", &is_selected);
      ImGui::Checkbox("Cholesterol", &is_selected);
      ImGui::Checkbox("Triglyceride", &is_selected);
      ImGui::Checkbox("Adipose Tissue", &is_selected);
      
      ImGui::ListBoxFooter();
    }
  
    
    Openc2eImgui::EndWindow();
  }
}

// void CreatureGrapher::onCreatureChange() {
// 	graph->wipeGraph();
// }

// void CreatureGrapher::onCreatureTick() {
// Creature *c = parent->getSelectedCreature();
// if (!c) return; // TODO: assert
// 
// // TODO: we should only update on biochem ticks..
// 
// c2eCreature *cc = dynamic_cast<c2eCreature *>(c);
// if (cc) {
//   for (unsigned int i = 1; i < 256; i++)
//     graph->addDataPoint(i, cc->getChemical(i));
// }
// oldCreature *oc = dynamic_cast<oldCreature *>(c);
// if (oc) {
//   for (unsigned int i = 1; i < 256; i++)
//     graph->addDataPoint(i, oc->getChemical(i));
// }
// 
// graph->update();


// void CreatureGrapher::onChemSelectionChange(unsigned int i) {
// 	graph->setDataSetVisible(i, selector->chemSelected(i));
// 
// 	graph->update();
// }