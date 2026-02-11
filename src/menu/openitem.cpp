
#include "menu/openitem.hpp"

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

    OpenMenuItem::OpenMenuItem() noexcept
    {   
        name = new char[1024];
        directory = new char[4096];

        home = get_home();
        current = home;

        name[0] = '\0';
        std::strcpy(directory, current.c_str());
    }

    OpenMenuItem::~OpenMenuItem() noexcept
    {
        delete[] name;
        delete[] directory;
    }

    void OpenMenuItem::query_current_directory() noexcept
    {
        directories.clear();
        files.clear();

        std::filesystem::directory_iterator dir_iter(current);

        for (const auto& entry : dir_iter) {
            std::string name = entry.path().filename();
            if (entry.is_directory()) {
                directories.push_back(entry.path());
            } else if (entry.is_regular_file() && entry.path().has_extension() && is_image(entry.path().extension())) {
                files.push_back(entry.path());
            }
        }

        std::stable_sort(directories.begin(), directories.end(), path_less_compare{});
        std::stable_sort(files.begin(), files.end(), path_less_compare{});
    }

    void OpenMenuItem::menubar() noexcept
    {
        if (ImGui::MenuItem("Open", "Ctrl+O")) {
            action();
        }
    }

    void OpenMenuItem::action() noexcept
    {
        ImGui::PushOverrideID(37);
        ImGui::OpenPopup("Open");
        ImGui::PopID();
        query_current_directory();
    }

    void OpenMenuItem::display() noexcept
    {
        constexpr uint32_t ext_size = sizeof(extensions) / sizeof(extensions[0]);

        ImGui::PushOverrideID(37);
        if (ImGui::BeginPopupModal("Open", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
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

            ImGui::Checkbox("Show hidden", &hidden);

            ImGui::BeginChild("filesystem", ImVec2(600, 350), ImGuiChildFlags_Borders);
            for (const auto &dir : directories) {
                if (!hidden && dir.filename().string().starts_with(".")) {
                    continue;
                }
                if (ImGui::Selectable(dir.filename().c_str())) {
                    current = dir;
                    std::strcpy(directory, current.c_str());
                    query_current_directory();
                    break;
                }
            }
            for (const auto &file : files) {
                if ((selected > 0) && std::strcmp(file.extension().c_str(), extensions[selected]) != 0) {
                    continue;
                }

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
            ImGui::SetNextItemWidth(100);
            if (ImGui::BeginCombo("##filetype", extensions[selected])) {
                for (uint32_t i = 0; i < ext_size; i++) {
                    if (ImGui::Selectable(extensions[i], i == selected)) {
                        selected = i;
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            if (ImGui::Button("Open", ImVec2(140, 0))) {
                // Create new file with specified width and height
                std::filesystem::path file = name;
                FileFormat fmt = formats[selected];

                if (file.empty()) {
                    return;
                }

                if (!file.has_extension()) {
                    if (selected > 0) {
                        file += extensions[selected];
                    } else {
                        file += ".png"; // default to png
                        fmt = FileFormat::PNG;
                    }
                } else {
                    fmt = fetch_format(file.extension());
                }

                if (!std::filesystem::exists(current / file)) {
                    ImGui::CloseCurrentPopup();
                    ImGui::PushOverrideID(37);
                    ImGui::OpenPopup("NotFoundFailure");
                    ImGui::PopID();
                    return;
                }

                if (fmt == FileFormat::All) {
                    ImGui::CloseCurrentPopup();
                    ImGui::PushOverrideID(37);
                    ImGui::OpenPopup("ExtFailure");
                    ImGui::PopID();
                    return;
                }

                App::get_instance()->emit_signal<const std::filesystem::path &,
                    const std::filesystem::path &,
                    FileFormat>
                    ("open_canvas_requested", current, file, fmt);
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

        ImGui::PushOverrideID(37);
        if (ImGui::BeginPopupModal("NotFoundFailure", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            std::filesystem::path full = current / name;
            ImGui::Text("Can't open '%s': File not found", full.c_str());

            if (ImGui::Button("Cancel", ImVec2(140, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        ImGui::PopID();

        ImGui::PushOverrideID(37);
        if (ImGui::BeginPopupModal("ExtFailure", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            std::filesystem::path full = current / name;
            ImGui::Text("Can't open '%s': '%s' isn't supported", full.c_str(), full.extension().c_str());

            if (ImGui::Button("Cancel", ImVec2(140, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        ImGui::PopID();
    }

    void OpenMenuItem::shortcuts() {
        if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_O)) {
            action();
        }
    } 
}
