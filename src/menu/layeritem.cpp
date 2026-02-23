
#include "menu/layeritem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

namespace chroma {

void LayerMenuItem::menubar() noexcept
{
    if (ImGui::MenuItem("New Layer", "Shift+N")) {
        App::get_instance()->emit_signal("layer_new");
    }
    if (ImGui::MenuItem("Delete Layer")) {
        App::get_instance()->emit_signal("layer_delete");
    }
}

void LayerMenuItem::shortcuts() noexcept
{
    if (ImGui::IsKeyChordPressed(ImGuiMod_Shift | ImGuiKey_N)) {
        App::get_instance()->emit_signal("layer_new");
    }
}
} // namespace chroma
