
#include "menu/saveitem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

namespace chroma {

    void SaveMenuItem::menubar() noexcept
    {
        if (ImGui::MenuItem("Save", "Ctrl+S")) {
            ImGui::PushOverrideID(33);
            ImGui::OpenPopup("Save");
            ImGui::PopID();
        }
    }

    void SaveMenuItem::display() noexcept
    {
        ImGui::PushOverrideID(32);
        if (ImGui::BeginPopupModal("Save", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::InputText("File name", name, 1024);
            if (ImGui::BeginCombo("File type", ".bmp")) {
                ImGui::Selectable(".bmp");
                ImGui::EndCombo();
            }

            std::cout << name << std::endl;

            if (ImGui::Button("Save", ImVec2(140, 0))) {
                // Create new file with specified width and height
                App::get_instance()->emit_signal("save_canvas_requested", name, ".bmp");
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(140, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        ImGui::PopID();
    }
}
