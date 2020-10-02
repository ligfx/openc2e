// #pragma once
// 
// #include <imgui.h>
// #include <imgui_internal.h>
// 
// #include "fileformats/bmpImage.h"
// #include "fileformats/c16Image.h"
// 
// static bool s_hatchery_open = false;
// static bool s_hatchery_loaded = false;
// 
// static SDL_Texture *s_hatcherybackground = nullptr;
// static SDL_Texture *s_hatcherymask = nullptr;
// static std::vector<Texture> s_omelette = nullptr;
// 
// struct C1EggInfo {
//   std::string name;
//   SDL_Texture *texture;
//   int x;
//   int y;
// };
// 
// std::array<C1EggInfo, 6> s_c1eggs = {{
//     {"hatchery/EGG0.bmp", nullptr, 50, 150},
//     {"hatchery/EGG1.bmp", nullptr, 80, 145},
//     {"hatchery/EGG2.bmp", nullptr, 110, 150},
//     {"hatchery/EGG3.bmp", nullptr, 140, 140},
//     {"hatchery/EGG4.bmp", nullptr, 170, 155},
//     {"hatchery/EGG5.bmp", nullptr, 200, 150},
// }};
// 
// struct C2EggInfo {
//   int frame;
//   int x;
//   int y;
// };
// 
// std::array<C2EggInfo, 10> s_c2eggs;
// 
// int TextureAtlasGetWidth(const TextureAtlasHandle& atlas_, int frame) {
//   SDLTextureAtlas* atlas = dynamic_cast<SDLTextureAtlas*>(atlas_.get());
//   assert(frame < atlas->textures.size());
//   SDL_Texture *texture = atlas->textures[frame];
//   int w;
//   SDL_QueryTexture(texture, nullptr, nullptr, &w, nullptr);
//   return w;
// }
// 
// int TextureAtlasGetHeight(const TextureAtlasHandle& atlas_, int frame) {
//   SDLTextureAtlas* atlas = dynamic_cast<SDLTextureAtlas*>(atlas_.get());
//   assert(frame < atlas->textures.size());
//   SDL_Texture *texture = atlas->textures[frame];
//   int h;
//   SDL_QueryTexture(texture, nullptr, nullptr, nullptr, &h);
//   return h;
// }
// 
// SDL_Texture *CreateTextureFromBmp(bmpImage &bmp) {
//   // TODO: don't cast. Move this into Backend?
//   return ((SDLBackend *)engine.backend.get())
//       ->createTexture(bmp.data(0), bmp.width(0), bmp.height(0), bmp.format(), bmp.getColorKey(0),
//                       bmp.getCustomPalette());
// }
// 
// void LoadHatchery() {
//   if (engine.version == 1) {
//     std::string hatcherybgfile = world.findFile("hatchery/hatchery.bmp");
//     if (hatcherybgfile.size()) {
//       // TODO: make top-left pixel transparent
//       bmpImage bgbmp(hatcherybgfile);
//       s_hatcherybackground = CreateTextureFromBmp(bgbmp);
//     }
// 
//     std::string hatcherymaskfile = world.findFile("hatchery/htchmask.bmp");
//     if (hatcherymaskfile.size()) {
//       bmpImage maskbmp(hatcherymaskfile);
//       s_hatcherymask = CreateTextureFromBmp(maskbmp);
//     }
// 
//     for (auto &e : s_c1eggs) {
//       std::string path = world.findFile(e.name);
//       if (path.size()) {
//         bmpImage bmp(path);
//         e.texture = CreateTextureFromBmp(bmp);
//       }
//     }
//   } else if (engine.version == 2) {
//     std::string hatcherybgfile = world.findFile("Applet Data/Hatchery.bmp");
//     if (hatcherybgfile.size()) {
//       bmpImage bgbmp(hatcherybgfile);
//       s_hatcherybackground = CreateTextureFromBmp(bgbmp);
//     }
// 
//     std::string omelettefile = world.findFile("Applet Data/Omelette.s16");
//     if (omelettefile.size()) {
//       std::ifstream in(omelettefile, std::ios::binary);
//       s16Image omelette(in, "Omelette");
//       for (size_t i = 0; i < omelette.numframes(); ++i) {
//           uint16_t topleft = ((uint16_t*)omelette.data(i))[0];
//           omelette.setColorKey(i, topleft);
//       }
//       s_omelette = engine.backend->createTextureAtlasFromCreaturesImage(omelette);
//     }
// 
//     // make leftside and rightside transparent
//     SDL_SetTextureAlphaMod(
//       ((SDLTextureAtlas*)s_omelette.get())->textures[18],
//       128
//     );
//     SDL_SetTextureAlphaMod(
//       ((SDLTextureAtlas*)s_omelette.get())->textures[19],
//       128
//     );
// 
//     for (size_t i = 0; i < 10; i++) {
//       // Place eggs from 80-240 horizontally, 110-230 vertically
//       int x = 0, y = 0, tries = 0;
//       while (x == 0 && tries < 50) {
//         x = (rand() / (RAND_MAX + 1.0)) * (240 - 80 - TextureAtlasGetWidth(s_omelette, i)) + 80;
//         y = (rand() / (RAND_MAX + 1.0)) * (230 - 110 - TextureAtlasGetHeight(s_omelette, i)) + 110;
//         for (unsigned int j = 0; j < i; j++) {
//           float xdist = x - s_c2eggs[j].x;
//           xdist = xdist * xdist;
//           float ydist = y - s_c2eggs[j].y;
//           ydist = ydist * ydist;
//           if (xdist + ydist < 20 * 20) {
//             // less than 20 pixels apart
//             // try again
//             x = 0;
//           }
//         }
//         tries++; // give up after 50 attempts at placement..
//       }
//       s_c2eggs[i].frame = i;
//       s_c2eggs[i].x = x;
//       s_c2eggs[i].y = y;
//     }
//     std::sort(s_c2eggs.begin(), s_c2eggs.end(), [](const auto &l, const auto &r) { return l.y < r.y; });
//   }
// 
//   s_hatchery_loaded = true;
// }
// 
// void UnloadHatchery() {
//   if (s_hatcherybackground) {
//     SDL_DestroyTexture(s_hatcherybackground);
//   }
//   if (s_hatcherymask) {
//     SDL_DestroyTexture(s_hatcherymask);
//   }
//   s_omelette = {};
//   for (auto &e : s_c1eggs) {
//     if (e.texture) {
//       SDL_DestroyTexture(e.texture);
//     }
//   }
// 
//   s_hatchery_loaded = false;
// }
// 
// bool IsHatcheryEnabled() {
//   return engine.version == 1 || engine.version == 2;
// }
// 
// void DrawHatchery() {
//   if (!(engine.version == 1 || engine.version == 2)) {
//     return;
//   }
// 
//   /*
//     C1 hatchery resources:
//     hatchery.bmp and htchmask.bmp used for the background/foreground
//     SCAN*.BMP and hdsk.wav used for egg disk animation
//     EGG*.BMP and hegg.wav used for egg movement animation
//     FAN*.BMP and hfan.wav used for the fan animation
//     lightoff.bmp and hlgt.wav used for the light flickering
//     GENSPIN.BMP, and hmle.wav/hfml.wav used for male/female animation (male.bmp
//     and female.bmp also present)
// 
//     C2's Omelette.s16:
//     * frames 0-10 are eggs (33px x 52px)
//     * frames 11-16 are C1-style eggs masked with black
//     * frame 17 is an egg shadow
//     * frames 22-37 is a rotating questionmark anim
//     * frames 38-53 (female) and 54-69 are (male) are rotating gender symbol
//     anims
//     *
//     * frame 18 and 19 are sides of the hatchery
//     * frame 20 is the grabber
//     * frame 21 is the middle bit of the side
//   */
// 
//   if (s_hatchery_open && ImGui::Begin("Hatchery", &s_hatchery_open,
//                                       ImGuiWindowFlags_NoScrollbar |
//                                           ImGuiWindowFlags_NoScrollWithMouse |
//                                           ImGuiWindowFlags_NoCollapse)) {
// 
//     if (!s_hatchery_loaded) {
//       LoadHatchery();
//     }
// 
//     if (engine.version == 1) {
//       // background
//       ImVec2 p = ImGui::GetCursorScreenPos();
//       DrawTexture(s_hatcherybackground, p);
//       ImGui::Dummy(ImVec2(320, 240));
// 
//       /* fan animation */
//       for (unsigned int i = 0; i < 4; i++) {
//         // TODO
//       }
// 
//       /* 'off' state for the light */
//       // TODO
// 
//       /* eggs */
//       for (auto &e : s_c1eggs) {
//         if (!e.texture)
//           continue;
//         DrawTexture(e.texture, p + ImVec2(e.x, e.y));
//       }
// 
//       /* mask which goes over the eggs */
//       DrawTexture(s_hatcherymask, p + ImVec2(0, 168));
// 
//       /* gender marker animation */
//       // TODO
//     } else if (engine.version == 2) {
//       // background
//       ImVec2 p = ImGui::GetCursorScreenPos();
//       DrawTexture(s_hatcherybackground, p);
//       ImGui::Dummy(ImVec2(320, 275));
// 
//       // eggs
//       for (int i = 0; i < 10; ++i) {
//         DrawTextureAtlas(s_omelette, s_c2eggs[i].frame, p + ImVec2(s_c2eggs[i].x, s_c2eggs[i].y));
//       }
//       // TODO: gender marker animation      
//       // TODO: place shadows
// 
//       // leftsideimg      
//       DrawTextureAtlas(s_omelette, 18, p + ImVec2(75, 176));
//       // rightsideimg
//       DrawTextureAtlas(s_omelette, 19, p + ImVec2(181, 162));
//       // grabberimg
//       DrawTextureAtlas(s_omelette, 20, p + ImVec2(150, 93));
//       // midsideimg
//       DrawTextureAtlas(s_omelette, 21, p + ImVec2(141, 177));
//     }
// 
//     ImGui::End();
//   }
// }