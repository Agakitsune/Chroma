
#define IMGUI_DEFINE_MATH_OPERATORS

#include "window/layer.hpp"
#include "app.hpp"
#include "cursor.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <functional>
#include <utility>

namespace chroma {

    LayerWindow::LayerWindow() noexcept
        : Window("Layer",
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove) {
        // App::get_instance()->add_signal<const Color &>("color_picked");

        // App::get_instance()->add_signal<const Color &>("main_color_changed");
        // App::get_instance()->add_signal<const Color &>(
        //     "secondary_color_changed");
    
        name = new char[1024];
        name[0] = '\0';
    }

    LayerWindow::~LayerWindow() noexcept {
        delete[] name;
    }

    void LayerWindow::ready() noexcept {
        App::get_instance()->connect_signal(
            "canvas_selected", this,
            &LayerWindow::_on_canvas_selected);
    }

    void LayerWindow::display() noexcept {
        char l[64] = {0};

        ImGui::Begin(label.c_str(), nullptr, flags);

        if (canvas) {
            for (uint32_t i = canvas->layers.size(); i > 0; i--) {
                bool s = ((i - 1) == this->selected);
                const char *c = nullptr;

                if (canvas->layers[i - 1].name.empty()) {
                    c = "##";
                } else {
                    c = canvas->layers[i - 1].name.c_str();
                }

                ImGui::SameLine();

                if (ImGui::Selectable(c, &s, ImGuiSelectableFlags_AllowDoubleClick)) {
                    canvas->layer = i - 1;
                    this->selected = i - 1;
                    if (ImGui::IsMouseDoubleClicked(0)) {
                        ImGui::PushOverrideID(60);
                        ImGui::OpenPopup("Layer Properties");
                        open = true;
                        ImGui::PopID();
                        strcpy(name, canvas->layers[this->selected].name.c_str());
                    }
                }

                if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) {
                    int n_next = (i - 1) + (ImGui::GetMouseDragDelta(0).y < 0.f ? 1 : -1);
                    if (n_next >= 0 && n_next < canvas->layers.size()) {
                        Layer c = std::move(canvas->layers[i - 1]);
                        canvas->layers[i - 1] = std::move(canvas->layers[n_next]);
                        canvas->layers[n_next] = std::move(c);
                        ImGui::ResetMouseDragDelta();
                    }
                }
            }
        } else {
            ImGui::Text("Nothing to see here");
        }

        ImGui::End();

        ImGui::PushOverrideID(60);
        if (ImGui::BeginPopupModal("Layer Properties", &open,
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Name:");

            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            if (ImGui::InputText("##name", name, 1024)) {
                canvas->layers[this->selected].name = name;
            }

            ImGui::EndPopup();
        }

        ImGui::PopID();
    }

    void LayerWindow::_on_canvas_selected(Canvas *canvas) noexcept {
        this->canvas = canvas;
    }
}
