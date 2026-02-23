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

void UndoRedoMenuItem::menubar() noexcept
{
    bool disabled = !App::get_instance()->empty;

    if (ImGui::MenuItem("Undo", "Ctrl+Z", nullptr, disabled)) {
        App::get_instance()->emit_signal("edit_undo");
    }
    if (ImGui::MenuItem("Redo", "Ctrl+Y", nullptr, disabled)) {
        App::get_instance()->emit_signal("edit_redo");
    }
}

void UndoRedoMenuItem::shortcuts() noexcept
{
    bool disabled = !App::get_instance()->empty;

    if (disabled) {
        return;
    }

    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Z)) {
        App::get_instance()->emit_signal("edit_undo");
    }
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Y)) {
        App::get_instance()->emit_signal("edit_redo");
    }
}
} // namespace chroma
