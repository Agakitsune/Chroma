
#include "menu/saveitem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

#include <filesystem>
#include <cstring>
#include <algorithm>


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

namespace chroma {

    struct path_less_compare {
        char upper(char c) const noexcept {
            if (c >= 96 && c <= 122) {
                return c - 32;
            }
            return c;
        }

        bool case_cmp(const char *a, const char *b) const noexcept
        {
            uint64_t i = 0;
            while (a[i]) {
                if (!b[i]) {
                    return false; // A has more character than B
                }
                if (upper(a[i]) != upper(b[i])) {
                    return upper(a[i]) < upper(b[i]);
                }
                i++;
            }
            return true;
        }

        bool operator()(const std::filesystem::path &a, const std::filesystem::path &b) const noexcept
        {
            return case_cmp(a.filename().c_str(), b.filename().c_str());
        }
    };

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

    void SaveMenuItem::query_current_directory() noexcept
    {
        directories.clear();
        files.clear();

        std::filesystem::directory_iterator dir_iter(current);

        for (const auto& entry : dir_iter) {
            std::string name = entry.path().filename();
            if (name.starts_with(".")) {
                continue;
            }
            if (entry.is_directory()) {
                directories.push_back(entry.path());
            } else {
                files.push_back(entry.path());
            }
        }

        std::stable_sort(directories.begin(), directories.end(), path_less_compare{});
        std::stable_sort(files.begin(), files.end(), path_less_compare{});
    }

    void SaveMenuItem::menubar() noexcept
    {
        if (ImGui::MenuItem("Save", "Ctrl+S")) {
            ImGui::PushOverrideID(33);
            ImGui::OpenPopup("Save");
            ImGui::PopID();
            query_current_directory();
        }
    }

    void SaveMenuItem::display() noexcept
    {
        ImGui::PushOverrideID(33);
        if (ImGui::BeginPopupModal("Save", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            // std::filesystem::directory_iterator dir_iter(current);
            
            if (ImGui::ArrowButton("##go_up", ImGuiDir_Up)) {
                current = current.parent_path();
                std::strcpy(directory, current.c_str());
                query_current_directory();
                // dir_iter = std::filesystem::directory_iterator(current);
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputText("##directory", directory, 4096);

            ImGui::BeginChild("filesystem", ImVec2(500, 350), ImGuiChildFlags_Borders);
            for (const auto &dir : directories) {
                if (ImGui::Selectable(dir.filename().c_str())) {
                    current = dir;
                    std::strcpy(directory, current.c_str());
                    query_current_directory();
                    break;
                }
            }
            for (const auto &file : files) {
                if (ImGui::Selectable(file.filename().c_str())) {
                    std::strcpy(name, file.filename().c_str());
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
