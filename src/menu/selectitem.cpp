
#include "menu/selectitem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

namespace chroma {

void SelectItem::menubar() noexcept
{
    bool disabled = !App::get_instance()->empty;

    if (ImGui::MenuItem("Select All", "Ctrl+A", nullptr, disabled)) {
        App::get_instance()->emit_signal("select_all");
    }
}

void SelectItem::shortcuts() noexcept
{
    bool disabled = !App::get_instance()->empty;

    if (disabled) {
        return;
    }

    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_A)) {
        App::get_instance()->emit_signal("select_all");
    }
}
} // namespace chroma
