
#include "menu/layeritem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

namespace chroma {

void LayerMenuItem::menubar() noexcept
{
    bool disabled = !App::get_instance()->empty;

    if (ImGui::MenuItem("New Layer", "Shift+N", nullptr, disabled)) {
        App::get_instance()->emit_signal("layer_new");
    }
    if (ImGui::MenuItem("Delete Layer", nullptr, nullptr, disabled)) {
        App::get_instance()->emit_signal("layer_delete");
    }
}

void LayerMenuItem::shortcuts() noexcept
{
    bool disabled = !App::get_instance()->empty;

    if (disabled) {
        return;
    }

    if (ImGui::IsKeyChordPressed(ImGuiMod_Shift | ImGuiKey_N)) {
        App::get_instance()->emit_signal("layer_new");
    }
}
} // namespace chroma
