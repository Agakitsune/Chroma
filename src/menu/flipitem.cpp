/**
 * @file flipitem.cpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief Flipping vertically and horizontally whole image
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "menu/flipitem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

namespace chroma {
/**
 * @brief Handle and signal if the Flip horizontally or vertically was called
 *
 */
void FlipMenuItem::menubar() noexcept {
  if (ImGui::MenuItem("Flip Horizontal", "Shift+H")) {
    App::get_instance()->emit_signal("edit_fliph");
  }
  if (ImGui::MenuItem("Flip Vertical", "Shift+V")) {
    App::get_instance()->emit_signal("edit_flipv");
  }
}
/**
 * @brief Handle and signal if the Flip horizontally shortcut Ctrl+H or
 * vertically Ctrl+V was called
 *
 */
void FlipMenuItem::shortcuts() noexcept {
  if (ImGui::IsKeyChordPressed(ImGuiMod_Shift | ImGuiKey_H)) {
    App::get_instance()->emit_signal("edit_fliph");
  }
  if (ImGui::IsKeyChordPressed(ImGuiMod_Shift | ImGuiKey_V)) {
    App::get_instance()->emit_signal("edit_flipv");
  }
}
} // namespace chroma
