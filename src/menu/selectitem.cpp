
#include "menu/selectitem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

namespace chroma {

void SelectItem::menubar() noexcept
{
    if (ImGui::MenuItem("Select All", "Ctrl+A")) {
        App::get_instance()->emit_signal("select_all");
    }
}

void SelectItem::shortcuts() noexcept
{
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_A)) {
        App::get_instance()->emit_signal("select_all");
    }
}
} // namespace chroma
