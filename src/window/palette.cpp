
#define IMGUI_DEFINE_MATH_OPERATORS

#include "window/palette.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"
#include "cursor.hpp"

namespace chroma {

    PaletteWindow::PaletteWindow() noexcept
    : Window(
        "Palette",
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove
    )
    {
        palette.resize(25);

        palette[0] =  Color(1.00f, 0.50f, 0.00f);   // Orange
        palette[1] =  Color(0.20f, 0.15f, 0.25f);   // Dark Purple
        palette[2] =  Color(0.90f, 0.75f, 0.55f);   // Beige
        palette[3] =  Color(0.95f, 0.95f, 0.20f);   // Yellow
        palette[4] =  Color(0.40f, 0.30f, 0.15f);   // Brown
        palette[5] =  Color(0.10f, 0.45f, 0.25f);   // Forest Green
        palette[6] =  Color(0.00f, 0.30f, 0.15f);   // Deep Green
        palette[7] =  Color(0.20f, 0.60f, 0.35f);   // Grass Green
        palette[8] =  Color(0.70f, 0.90f, 0.40f);   // Lime
        palette[9] =  Color(0.35f, 0.50f, 0.15f);   // Olive
        palette[10] = Color(0.10f, 0.40f, 0.45f);   // Teal
        palette[11] = Color(0.05f, 0.25f, 0.30f);   // Deep Teal
        palette[12] = Color(0.40f, 0.75f, 1.00f);   // Light Blue
        palette[13] = Color(0.20f, 0.50f, 0.90f);   // Sky Blue
        palette[14] = Color(0.10f, 0.25f, 0.50f);   // Steel Blue
        palette[15] = Color(0.90f, 0.90f, 1.00f);   // Off White
        palette[16] = Color(1.00f, 1.00f, 1.00f);   // White
        palette[17] = Color(0.50f, 0.30f, 0.30f);   // Brick
        palette[18] = Color(0.65f, 0.20f, 0.20f);   // Red
        palette[19] = Color(1.00f, 0.50f, 0.80f);   // Pink
        palette[20] = Color(0.40f, 0.45f, 0.15f);   // Olive Green
        palette[21] = Color(0.25f, 0.20f, 0.05f);   // Mud Brown
        palette[22] = Color(0.30f, 0.20f, 0.30f);   // Dark Mauve
        palette[23] = Color(0.55f, 0.40f, 0.50f);   // Purple Gray
        palette[24] = Color(0.50f, 0.15f, 0.40f);   // Plum

        // App::get_instance()->add_signal<const Color &>("color_selected");
    }

    void PaletteWindow::ready() noexcept
    {
        std::function<void(const Color &)> a = std::bind(&PaletteWindow::add_color, this, std::placeholders::_1);
        App::get_instance()->color_picker.color_picked.connect(a);
    }

    void PaletteWindow::display() noexcept
    {
        ImGui::Begin(label.c_str(), nullptr, flags);

        ImGuiContext& g = *GImGui;
        ImGuiWindow* w = ImGui::GetCurrentWindow();
        ImGuiIO& io = ImGui::GetIO();

        ImDrawList* draw_list = w->DrawList;
        ImDrawList* foreground = ImGui::GetForegroundDrawList();

        const ImVec2 pad = ImGui::GetStyle().WindowPadding;

        const ImVec2 avail = ImGui::GetContentRegionAvail();
        const ImVec2 button_size = ImVec2(22.0f, 22.0f);
        const ImVec2 back_size = button_size + ImVec2(2, 2);

        ImVec2 pos = w->DC.CursorPos;
        const ImVec2 origin = pos;

        uint64_t palette_size = palette.size();
        const uint64_t palette_width = (uint64_t)(avail.x / back_size.x);

        if (palette_width == 0) {
            ImGui::End();
            return;
        }

        const uint64_t palette_height = palette_size / palette_width + (palette_size % palette_width ? 1 : 0);

        for (uint64_t y = 0; y < palette_height; y++) {
            const uint64_t row = palette_size > palette_width ? palette_width : palette_size;
            palette_size -= row;

            ImGui::PushID((int)y);

            ImGui::SetCursorScreenPos(pos);

            for (uint64_t x = 0; x < row; x++) {
                const uint64_t index = y * palette_width + x;
                Color color = palette[index];

                ImGui::SetCursorScreenPos(pos);

                ImGui::PushID(x);
                if (ImGui::InvisibleButton("##color", back_size)) {
                    color_selected.emit(color);
                    // App::get_instance()->emit_signal<const Color &>("color_selected", color);
                    // App::get_instance()->get_window<ColorPickerWindow>("ColorPicker")->main_color = color;
                    selected = index;
                }
                if (ImGui::IsItemHovered()) {
                    CursorManager::set_cursor(Cursor::Point);
                }
                ImGui::PopID();

                draw_list->AddRectFilled(
                    pos,
                    pos + back_size,
                    selected == index ? IM_COL32(255, 255, 255, 255) : IM_COL32(0, 0, 0, 255)
                );

                ImGui::RenderColorRectWithAlphaCheckerboard(
                    draw_list,
                    pos + ImVec2(1, 1),
                    pos + ImVec2(1, 1) + button_size,
                    color.to_u32(),
                    8,
                    ImVec2(0, 0)
                );

                pos.x += back_size.x;
            }
            pos.x = origin.x;
            pos.y += back_size.y;

            ImGui::PopID();
        }

        ImGui::End();
    }

    void PaletteWindow::add_color(const Color &color) noexcept
    {
        palette.push_back(color);
    }

}
