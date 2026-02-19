
#define IMGUI_DEFINE_MATH_OPERATORS

#include "window/layer.hpp"
#include "app.hpp"
#include "cursor.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <functional>

namespace chroma {

    LayerWindow::LayerWindow() noexcept
        : Window("Layer",
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove) {
        // App::get_instance()->add_signal<const Color &>("color_picked");

        // App::get_instance()->add_signal<const Color &>("main_color_changed");
        // App::get_instance()->add_signal<const Color &>(
        //     "secondary_color_changed");
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
            for (uint32_t i = 0; i < canvas->layers.size(); i++) {
                bool s = i == this->selected;
                sprintf(l, "Layer %i", i + 1);

                if (ImGui::Selectable(l, &s)) {
                    canvas->layer = i;
                    this->selected = i;
                }
            }
        } else {
            ImGui::Text("Nothing to see here");
        }

        ImGui::End();
    }

    void LayerWindow::_on_canvas_selected(Canvas *canvas) noexcept {
        this->canvas = canvas;
    }
}
