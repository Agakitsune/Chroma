
#include "menu/flipitem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

namespace chroma {

    void FlipMenuItem::menubar() noexcept
    {
        if (ImGui::MenuItem("Flip Horizontal", "Shift+H")) {
            App::get_instance()->emit_signal("edit_fliph");
        }
        if (ImGui::MenuItem("Flip Vertical", "Shift+V")) {
            App::get_instance()->emit_signal("edit_flipv");
        }
    }

    void FlipMenuItem::shortcuts() noexcept
    {
        if (ImGui::IsKeyChordPressed(ImGuiMod_Shift | ImGuiKey_H)) {
            App::get_instance()->emit_signal("edit_fliph");
        }
        if (ImGui::IsKeyChordPressed(ImGuiMod_Shift | ImGuiKey_V)) {
            App::get_instance()->emit_signal("edit_flipv");
        }
    }
}
