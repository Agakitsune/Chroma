/**
 * @file undoredoitem.cpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief Undoing (Ctrl+Z) and restore deleted content (Ctrl+Y)
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "menu/undoredoitem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

namespace chroma {
/**
 * @brief Upon pressing the Save function, run the save action
 *
 */
void UndoRedoMenuItem::menubar() noexcept {
  if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
    App::get_instance()->emit_signal("edit_undo");
  }
  if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
    App::get_instance()->emit_signal("edit_redo");
  }
}
/**
 * @brief if Ctrl+Z pressed, run undo signal, if Ctrl+Y pressed, run redo signal
 *
 */
void UndoRedoMenuItem::shortcuts() noexcept {
  if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Z)) {
    App::get_instance()->emit_signal("edit_undo");
  }
  if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Y)) {
    App::get_instance()->emit_signal("edit_redo");
  }
}
} // namespace chroma
