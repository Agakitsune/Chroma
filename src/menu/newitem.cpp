
#include "menu/newitem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

namespace chroma {

    void NewMenuItem::menubar() noexcept
    {
        if (ImGui::MenuItem("New", "Ctrl+N")) {
            ImGui::PushOverrideID(32);
            ImGui::OpenPopup("New");
            w = 16;
            h = 16;
            ImGui::PopID();
        }
    }

    void NewMenuItem::display() noexcept
    {
        ImGui::PushOverrideID(32);
        if (ImGui::BeginPopupModal("New", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::SeparatorText("Size");

            ImGui::Text("Width:");

            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::SameLine();
            ImGui::InputScalar("##width", ImGuiDataType_U32, &w, nullptr, nullptr, "%upx"); // Need to store w and h in a better way

            ImGui::Text("Height:");

            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::SameLine();
            ImGui::InputScalar("##height", ImGuiDataType_U32, &h, nullptr, nullptr, "%upx");

            if (ImGui::Button("OK", ImVec2(140, 0))) {
                // Create new file with specified width and height
                App::get_instance()->emit_signal("create_canvas_requested", w, h);
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
