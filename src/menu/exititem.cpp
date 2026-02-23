/**
 * @file exititem.cpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief Exit Chroma software
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "menu/exititem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

namespace chroma {

void ExitMenuItem::menubar() noexcept {
  if (ImGui::MenuItem("Exit", "Ctrl+Q")) {
    SDL_Event event;
    event.type = SDL_EVENT_QUIT;
    SDL_PushEvent(&event);
  }
}

void ExitMenuItem::shortcuts() noexcept {
  if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Q)) {
    SDL_Event event;
    event.type = SDL_EVENT_QUIT;
    SDL_PushEvent(&event);
  }
}
} // namespace chroma
