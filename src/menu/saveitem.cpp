
#include "menu/saveitem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

#include <filesystem>
#include <cstring>

namespace chroma {

    static std::filesystem::path get_home() noexcept
    {
        #ifdef _WIN32
            const char* _home = std::getenv("USERPROFILE");
        #else
            const char* _home = std::getenv("HOME");
        #endif

        if (_home) {
            return std::filesystem::path(_home);
        }
        return "";
    }

    SaveMenuItem::SaveMenuItem() noexcept
    {   
        name = new char[1024];
        directory = new char[4096];

        home = get_home();
        current = home;

        name[0] = '\0';
        std::strcpy(directory, current.c_str());
    }

    SaveMenuItem::~SaveMenuItem() noexcept
    {
        delete[] name;
        delete[] directory;
    }

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
        ImGui::PushOverrideID(33);
        if (ImGui::BeginPopupModal("Save", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            std::filesystem::directory_iterator dir_iter(current);
            
            if (ImGui::ArrowButton("##go_up", ImGuiDir_Up)) {
                current = current.parent_path();
                std::strcpy(directory, current.c_str());
                dir_iter = std::filesystem::directory_iterator(current);
            }
            ImGui::SameLine();
            ImGui::InputText("##directory", directory, 4096);

            ImGui::BeginChild("filesystem", ImVec2(500, 350), ImGuiChildFlags_Borders);
            for (const auto& entry : dir_iter) {
                if (ImGui::Selectable(entry.path().filename().c_str())) {
                    current = entry.path();
                    std::strcpy(directory, current.c_str());
                    dir_iter = std::filesystem::directory_iterator(current);
                    break;
                }
            }
            ImGui::EndChild();

            ImGui::Text("File name: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputText("##filename", name, 1024);
            
            ImGui::Text("File type: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(70);
            if (ImGui::BeginCombo("##filetype", ".bmp")) {
                ImGui::Selectable(".bmp");
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            if (ImGui::Button("Save", ImVec2(140, 0))) {
                // Create new file with specified width and height
                App::get_instance()->emit_signal<const std::string &, const std::string &>("save_canvas_requested", name, ".bmp");
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
