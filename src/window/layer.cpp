
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

  name = new char[1024];
  name[0] = '\0';
}

LayerWindow::~LayerWindow() noexcept { delete[] name; }

void LayerWindow::ready() noexcept {
  App::get_instance()->connect_signal("canvas_selected", this,
                                      &LayerWindow::_on_canvas_selected);

  App::get_instance()->connect_signal("layer_delete", this,
                                      &LayerWindow::_on_layer_delete);
}

void LayerWindow::display() noexcept {
  char l[64] = {0};

  ImGui::Begin(label.c_str(), nullptr, flags);

  if (canvas) {
    for (uint32_t i = canvas->layers.size(); i > 0; i--) {
      bool s = ((i - 1) == this->selected);
      Layer &layer = canvas->layers[i - 1];
      const char *c = nullptr;

      if (layer.name.empty()) {
        c = "##";
      } else {
        c = layer.name.c_str();
      }

      ImGui::PushID(i);

      ImU32 check_col = ImGui::GetColorU32(ImGuiCol_CheckMark);
      ImGuiWindow *window = ImGui::GetCurrentWindow();
      ImGui::SetNextItemAllowOverlap();
      ImVec2 label_size = ImGui::CalcTextSize("##visible", NULL, true);
      ImGuiContext &g = *GImGui;
      const ImGuiStyle &style = g.Style;
      ImVec2 pos = window->DC.CursorPos;
      bool e = false;
      float sz = label_size.y - style.ItemSpacing.x * 0.5;

      if (ImGui::Selectable("##visible", &e, 0, ImVec2(sz, 0.0f))) {
        layer.visible = !layer.visible;
      }
      if (layer.visible) {

        ImGui::RenderCheckMark(window->DrawList, pos, check_col, sz);
      }

      ImGui::SameLine();
      ImGui::PopID();

      ImGui::PushID(layer.surface);
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
      ImGui::PopID();

      if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) {
        int n_next = (i - 1) + (ImGui::GetMouseDragDelta(0).y < 0.f ? 1 : -1);
        if (n_next >= 0 && n_next < canvas->layers.size()) {
          std::swap(canvas->layers[n_next], canvas->layers[i - 1]);
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

void LayerWindow::_on_layer_delete() noexcept {
  if (canvas) {
    selected = canvas->layer;
  }
}

} // namespace chroma
