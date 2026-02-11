
#include "menu/undoredoitem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

namespace chroma {

    void UndoRedoMenuItem::menubar() noexcept
    {
        if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
            App::get_instance()->emit_signal("edit_undo");
        }
        if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
            App::get_instance()->emit_signal("edit_redo");
        }
    }
}
