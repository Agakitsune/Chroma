/**
 * @file newitem.cpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief Create empty canva to draw on
 * @version 0.1
 * @date 2026-02-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "menu/newitem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

namespace chroma {
/**
 * @brief Handle and signal if a new file has been called
 * 
 */
    void NewMenuItem::menubar() noexcept
    {
        if (ImGui::MenuItem("New", "Ctrl+N")) {
            action();
        }
    }
/**
 * @brief Create a popup to create a new canva
 * 
 */
    void NewMenuItem::action() noexcept
    {
        ImGui::PushOverrideID(32);
        ImGui::OpenPopup("New");
        w = 16;
        h = 16;
        ImGui::PopID();
    }
/**
 * @brief Handle the popup new to ask if user confirm wanting to create new canva and specify dimension, 16x16 by default
 * 
 */
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
                App::get_instance()->emit_signal<uint32_t, uint32_t>("create_canvas_requested", w, h);
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
/**
 * @brief if Ctrl+N pressed, call new item creation function
 * 
 */
    void NewMenuItem::shortcuts() noexcept {
        if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_N)) {
            action();
        }
    } 
}
