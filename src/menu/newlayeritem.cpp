
#include "menu/newlayeritem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

namespace chroma {

    void NewLayerMenuItem::menubar() noexcept {
        if (ImGui::MenuItem("New Layer", "Shift+N")) {
            action();
        }
    }

    void NewLayerMenuItem::action() noexcept {
        App::get_instance()->emit_signal("layer_new");
    }

    void NewLayerMenuItem::shortcuts() noexcept {
        if (ImGui::IsKeyChordPressed(ImGuiMod_Shift | ImGuiKey_N)) {
            action();
        }
    }
} // namespace chroma
