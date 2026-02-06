
#define IMGUI_DEFINE_MATH_OPERATORS

#include "window/color_picker.hpp"
#include "app.hpp"
#include "cursor.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <functional>

namespace chroma {
    
    ColorPickerWindow::ColorPickerWindow() noexcept
    : Window(
        "Color Pick",
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove
    )
    {}

    void ColorPickerWindow::ready() noexcept {
        std::function<void(const Color &)> a = std::bind(&ColorPickerWindow::_on_main_color_selected, this, std::placeholders::_1);
        App::get_instance()->connect_signal<const Color &>("color_selected", a);
    }

    void ColorPickerWindow::display() noexcept
    {
        ImGui::Begin(label.c_str(), nullptr, flags);

        ImGuiContext& g = *GImGui;
        ImGuiWindow* w = ImGui::GetCurrentWindow();
        ImDrawList* draw_list = w->DrawList;
        ImGuiIO& io = ImGui::GetIO();

        const float width = ImGui::CalcItemWidth();

        ImGui::PushID("ColorPicker##4");
        const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
        if (set_current_color_edit_id)
            g.ColorEditCurrentID = w->IDStack.back();
        ImGui::BeginGroup();

        ImVec2 picker_pos = w->DC.CursorPos;

        float H = main_color[0], S = main_color[1], V = main_color[2];
        float R = main_color[0], G = main_color[1], B = main_color[2];

        ImGui::ColorConvertRGBtoHSV(R, G, B, H, S, V);
        // ImGui::ColorEditRestoreHS(col, &H, &S, &V);

        IM_ASSERT(g.ColorEditCurrentID != 0);
        if (!(g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(main_color[0], main_color[1], main_color[2], 0)))) {
            // When S == 0, H is undefined.
            // When H == 1 it wraps around to 0.
            if (S == 0.0f || (H == 0.0f && g.ColorEditSavedHue == 1))
                H = g.ColorEditSavedHue;

            // When V == 0, S is undefined.
            if (V == 0.0f)
                S = g.ColorEditSavedSat;
        }

        ImVec2 avail = ImGui::GetContentRegionAvail();

        ImVec2 sv_picker_size(avail.x, 150.0f);

        // float sv_picker_size = 250.0f;
        bool value_changed_sv = false;
        bool value_changed_h = false;
        bool value_changed = false;

        ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
        // SV rectangle logic
        ImGui::InvisibleButton("sv", sv_picker_size);
        bool right_clicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
        if (ImGui::IsItemHovered())
            CursorManager::set_cursor(Cursor::Picker);
        if (ImGui::IsItemActive() || right_clicked)
        {
            S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size.x - 1));
            V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size.y - 1));
            IM_ASSERT(g.ColorEditCurrentID != 0);
            if (!(g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(main_color[0], main_color[1], main_color[2], 0)))) {
                // return;
                H = g.ColorEditSavedHue;
            }
            // ImGui::ColorEditRestoreH(&main_color.r, &H); // Greatly reduces hue jitter and reset to 0 when hue == 255 and color is rapidly modified using SV square.
            value_changed_sv = true;

            if (right_clicked) {
                ImGui::ColorConvertHSVtoRGB(H, S, V, main_color[0], main_color[1], main_color[2]);
                App::get_instance()->get_window<PaletteWindow>("Palette")->add_color(Color{R, G, B, main_color[3]});
            }
        }

        const float bar_height = 15.0;

        // Hue bar logic
        ImGui::SetCursorScreenPos(ImVec2(picker_pos.x, picker_pos.y + sv_picker_size.y));
        ImGui::InvisibleButton("hue", ImVec2(sv_picker_size.x, bar_height));
        if (ImGui::IsItemHovered())
            CursorManager::set_cursor(Cursor::Picker);
        if (ImGui::IsItemActive())
        {
            H = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size.x - 1));
            value_changed_h = true;
        }

        ImGui::SetCursorScreenPos(ImVec2(picker_pos.x, picker_pos.y + sv_picker_size.y + bar_height));
        ImGui::InvisibleButton("alpha", ImVec2(sv_picker_size.x, bar_height));
        if (ImGui::IsItemHovered())
            CursorManager::set_cursor(Cursor::Picker);
        if (ImGui::IsItemActive())
        {
            main_color[3] = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size.x - 1));
            value_changed = true;
        }
        ImGui::PopItemFlag();

        const ImU32 col_black = IM_COL32(0,0,0,255);
        const ImU32 col_white = IM_COL32(255,255,255,255);
        const ImU32 col_midgrey = IM_COL32(128,128,128,255);
        const ImU32 col_hues[6 + 1] = { IM_COL32(255,0,0,255), IM_COL32(255,255,0,255), IM_COL32(0,255,0,255), IM_COL32(0,255,255,255), IM_COL32(0,0,255,255), IM_COL32(255,0,255,255), IM_COL32(255,0,0,255) };

        ImGui::ColorConvertHSVtoRGB(H, S, V, main_color[0], main_color[1], main_color[2]);
        g.ColorEditSavedHue = H;
        g.ColorEditSavedSat = S;
        g.ColorEditSavedID = g.ColorEditCurrentID;
        g.ColorEditSavedColor = ImGui::ColorConvertFloat4ToU32(ImVec4(main_color[0], main_color[1], main_color[2], 0));

        ImVec4 hue_color_f(1, 1, 1, 1);
        ImGui::ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
        ImU32 hue_color32 = ImGui::ColorConvertFloat4ToU32(hue_color_f);
        ImU32 user_col32_striped_of_alpha = ImGui::ColorConvertFloat4ToU32(ImVec4(R, G, B, 1.0));

        ImVec2 sv_cursor_pos;

        // float wheel_thickness = sv_picker_size * 0.08f;

        // Render SV Square
        draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + sv_picker_size, col_white, hue_color32, hue_color32, col_white);
        draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + sv_picker_size, 0, 0, col_black, col_black);
        // ImGui::RenderFrameBorder(picker_pos, picker_pos + sv_picker_size, 0.0f);
        sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S)     * sv_picker_size.x), picker_pos.x + 2, picker_pos.x + sv_picker_size.x - 2); // Sneakily prevent the circle to stick out too much
        sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * sv_picker_size.y), picker_pos.y + 2, picker_pos.y + sv_picker_size.y - 2);
        
        float sv_cursor_rad = 3.0f;
        int sv_cursor_segments = draw_list->_CalcCircleAutoSegmentCount(sv_cursor_rad); // Lock segment count so the +1 one matches others.

        // Render Hue Bar
        float bar_pos_y = picker_pos.y + sv_picker_size.y;
        for (int i = 0; i < 6; ++i) {
            draw_list->AddRectFilledMultiColor(ImVec2(picker_pos.x + i * (sv_picker_size.x / 6), bar_pos_y), ImVec2(picker_pos.x + (i + 1) * (sv_picker_size.x / 6), bar_pos_y + bar_height), col_hues[i], col_hues[i + 1], col_hues[i + 1], col_hues[i]);
        }
        ImVec2 hue_bar;
        hue_bar.y = bar_pos_y + bar_height / 2.0;
        hue_bar.x = IM_ROUND(picker_pos.x + H * sv_picker_size.x);
        draw_list->AddCircle(hue_bar, sv_cursor_rad + 1, col_midgrey, sv_cursor_segments);
        draw_list->AddCircle(hue_bar, sv_cursor_rad, col_white, sv_cursor_segments);
        // RenderFrameBorder(ImVec2(bar0_pos_x, picker_pos.y), ImVec2(bar0_pos_x + bars_width, picker_pos.y + sv_picker_size), 0.0f);
        // ImGui::RenderArrowsForVerticalBar(draw_list, ImVec2(bar0_pos_x - 1, bar0_line_y), ImVec2(bars_triangles_half_sz + 1, bars_triangles_half_sz), bars_width + 2.0f, style.Alpha);
        
        bar_pos_y = picker_pos.y + sv_picker_size.y + bar_height;
        float alpha = ImSaturate(main_color[3]);
        ImRect bar1_bb(picker_pos.x, bar_pos_y, picker_pos.x + sv_picker_size.x, bar_pos_y + bar_height);
        ImGui::RenderColorRectWithAlphaCheckerboard(draw_list, bar1_bb.Min, bar1_bb.Max, 0, 16.0f, ImVec2(0.0f, -(16.0f - bar_height / 2.0f)));
        draw_list->AddRectFilledMultiColor(bar1_bb.Min, bar1_bb.Max, user_col32_striped_of_alpha & ~IM_COL32_A_MASK, user_col32_striped_of_alpha, user_col32_striped_of_alpha, user_col32_striped_of_alpha & ~IM_COL32_A_MASK);
        // float bar1_line_y = IM_ROUND(picker_pos.y + (1.0f - alpha) * sv_picker_size);

        ImVec2 alpha_bar;
        alpha_bar.y = bar_pos_y + bar_height / 2.0;
        alpha_bar.x = IM_ROUND(picker_pos.x + alpha * sv_picker_size.x);
        draw_list->AddCircle(alpha_bar, sv_cursor_rad + 1, col_midgrey, sv_cursor_segments);
        draw_list->AddCircle(alpha_bar, sv_cursor_rad, col_white, sv_cursor_segments);

        // RenderFrameBorder(bar1_bb.Min, bar1_bb.Max, 0.0f);
        // RenderArrowsForVerticalBar(draw_list, ImVec2(bar1_pos_x - 1, bar1_line_y), ImVec2(bars_triangles_half_sz + 1, bars_triangles_half_sz), bars_width + 2.0f, style.Alpha);

        // draw_list->AddCircleFilled(sv_cursor_pos, sv_cursor_rad, user_col32_striped_of_alpha, sv_cursor_segments);
        draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad + 1, col_midgrey, sv_cursor_segments);
        draw_list->AddCircle(sv_cursor_pos, sv_cursor_rad, col_white, sv_cursor_segments);

        ImGui::EndGroup();

        if (set_current_color_edit_id)
            g.ColorEditCurrentID = 0;
        ImGui::PopID();

        ImGui::End();
    }

    void ColorPickerWindow::_on_main_color_selected(const Color &clr) {
        main_color = clr;
    }
}
