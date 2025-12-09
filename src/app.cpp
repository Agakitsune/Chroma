
#define IMGUI_DEFINE_MATH_OPERATORS

#include "app.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlgpu3.h"

#include <iostream>



namespace chroma {
    App::~App() {
        SDL_WaitForGPUIdle(device);
        ImGui_ImplSDL3_Shutdown();
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui::DestroyContext();

        SDL_ReleaseWindowFromGPUDevice(device, window);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    int App::init() {
        int err = 0;
        
        if ((err = setup()) != 0) {
            return err;
        }

        if ((err = setup_imgui()) != 0) {
            return err;
        }

        return 0;
    }
            
    int App::run() {
        uint64_t tick = 0;
        uint64_t delta = 0;

        ImVec4 button_colors[] = {
            {1.00f, 0.50f, 0.00f, 1.0f},   // Orange
            {0.20f, 0.15f, 0.25f, 1.0f},   // Dark Purple
            {0.90f, 0.75f, 0.55f, 1.0f},   // Beige
            {0.95f, 0.95f, 0.20f, 1.0f},   // Yellow
            {0.40f, 0.30f, 0.15f, 1.0f},   // Brown

            {0.10f, 0.45f, 0.25f, 1.0f},   // Forest Green
            {0.00f, 0.30f, 0.15f, 1.0f},   // Deep Green
            {0.20f, 0.60f, 0.35f, 1.0f},   // Grass Green
            {0.70f, 0.90f, 0.40f, 1.0f},   // Lime
            {0.35f, 0.50f, 0.15f, 1.0f},   // Olive

            {0.10f, 0.40f, 0.45f, 1.0f},   // Teal
            {0.05f, 0.25f, 0.30f, 1.0f},   // Deep Teal
            {0.40f, 0.75f, 1.00f, 1.0f},   // Light Blue
            {0.20f, 0.50f, 0.90f, 1.0f},   // Sky Blue
            {0.10f, 0.25f, 0.50f, 1.0f},   // Steel Blue

            {0.90f, 0.90f, 1.00f, 1.0f},   // Off White
            {1.00f, 1.00f, 1.00f, 1.0f},   // White
            {0.50f, 0.30f, 0.30f, 1.0f},   // Brick
            {0.65f, 0.20f, 0.20f, 1.0f},   // Red
            {1.00f, 0.50f, 0.80f, 1.0f},   // Pink

            {0.40f, 0.45f, 0.15f, 1.0f},   // Olive Green
            {0.25f, 0.20f, 0.05f, 1.0f},   // Mud Brown
            {0.30f, 0.20f, 0.30f, 1.0f},   // Dark Mauve
            {0.55f, 0.40f, 0.50f, 1.0f},   // Purple Gray
            {0.50f, 0.15f, 0.40f, 1.0f},   // Plum
        };

        ImGuiIO& io = ImGui::GetIO();

        while (!done) {
            process_events();

            const SDL_WindowFlags flags = SDL_GetWindowFlags(window);
            const bool is_focused = (flags & SDL_WINDOW_INPUT_FOCUS) != 0;

            tick = SDL_GetTicks();

            // Idling handling
            if (!is_focused || idle) {
                SDL_Delay(100);
                // continue;
            }

            // Start the Dear ImGui frame
            ImGui_ImplSDLGPU3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            if (!dockspace_initialized) {
                setup_imgui_dockspace();
                dockspace_initialized = true;
            } else {
                imgui_dockspace();
            }
            // imgui_dockspace();

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

            ImGui::ShowDemoWindow();

            ImGui::Begin("Debug", nullptr, window_flags);
            ImGui::Text("Time: %lu ms (%.1f FPS)", delta, delta > 0 ? 1000.0f / delta : 0.0f);
            ImGui::End();

            ImGui::Begin("Palette", nullptr, window_flags);

            int color_perLine = 5;
            int total_color_nb = sizeof(button_colors) / sizeof(button_colors[0]);
            int rows = (total_color_nb + color_perLine - 1) / color_perLine;

            if (ImGui::BeginTable("table1", color_perLine))
            {
                int color_nb = 0;

                for (int row = 0; row < rows; row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < color_perLine; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        // Si on dépasse le nombre de couleurs → cellules vides
                        if (color_nb >= total_color_nb)
                            continue;
                        std::string button_id = "ColorButton##" + std::to_string(color_nb);
                        if (ImGui::ColorButton(button_id.c_str(), button_colors[color_nb], ImGuiColorEditFlags_NoTooltip))
                        {
                            main_color.r = button_colors[color_nb].x;
                            main_color.g = button_colors[color_nb].y;
                            main_color.b = button_colors[color_nb].z;
                            main_color.a = button_colors[color_nb].w;
                        }
                        color_nb++;
                    }
                }
                ImGui::EndTable();
            }
            ImGui::End();


            ImGui::Begin("ColorPick", nullptr, window_flags);

            ImGuiContext& g = *GImGui;
            ImGuiWindow* w = ImGui::GetCurrentWindow();
            ImDrawList* draw_list = w->DrawList;

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
            if (ImGui::IsItemActive())
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
            }

            const float bar_height = 15.0;

            // Hue bar logic
            ImGui::SetCursorScreenPos(ImVec2(picker_pos.x, picker_pos.y + sv_picker_size.y));
            ImGui::InvisibleButton("hue", ImVec2(sv_picker_size.x, bar_height));
            if (ImGui::IsItemActive())
            {
                H = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size.x - 1));
                value_changed_h = true;
            }

            ImGui::SetCursorScreenPos(ImVec2(picker_pos.x, picker_pos.y + sv_picker_size.y + bar_height));
            ImGui::InvisibleButton("alpha", ImVec2(sv_picker_size.x, bar_height));
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
            // if (!(flags & ImGuiColorEditFlags_NoOptions))
            //     OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

            // // Hue bar logic
            // ImGui::SetCursorScreenPos(ImVec2(bar0_pos_x, picker_pos.y));
            // ImGui::InvisibleButton("hue", ImVec2(bars_width, sv_picker_size));
            // if (IsItemActive() && !is_readonly)
            // {
            //     H = ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size - 1));
            //     value_changed = value_changed_h = true;
            // }
            
            // ImGui::ColorPicker4("ColorPicker##4", &main_color.r, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoLabel);

            ImGui::End();

            ImGui::Begin("Layer", nullptr, window_flags);
            ImGui::Text("Layer and shit");
            ImGui::End();

            // Rendering
            ImGui::Render();
            ImDrawData* draw_data = ImGui::GetDrawData();
            const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

            SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(device); // Acquire a GPU command buffer

            SDL_GPUTexture* swapchain_texture;
            SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer, window, &swapchain_texture, nullptr, nullptr); // Acquire a swapchain texture

            if (swapchain_texture != nullptr) {
                // This is mandatory: call ImGui_ImplSDLGPU3_PrepareDrawData() to upload the vertex/index buffer!
                ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, command_buffer);

                // Setup and start a render pass
                SDL_GPUColorTargetInfo target_info = {};
                target_info.texture = swapchain_texture;
                target_info.clear_color = SDL_FColor { 0.07, 0.07, 0.07, 1.0 };
                target_info.load_op = SDL_GPU_LOADOP_CLEAR;
                target_info.store_op = SDL_GPU_STOREOP_STORE;
                target_info.mip_level = 0;
                target_info.layer_or_depth_plane = 0;
                target_info.cycle = false;
                SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);

                // Render ImGui
                ImGui_ImplSDLGPU3_RenderDrawData(draw_data, command_buffer, render_pass);

                SDL_EndGPURenderPass(render_pass);
            }

            const uint64_t end_tick = SDL_GetTicks();
            delta = end_tick - tick;

            // Submit the command buffer
            SDL_SubmitGPUCommandBuffer(command_buffer);
        }
        return 0;
    }

    int App::create_window() {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
        {
            SDL_Log("Error: SDL_Init(): %s\n", SDL_GetError());
            return 1;
        }

        // Create SDL window graphics context
        const float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
        
        SDL_Rect display_bounds;
        SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &display_bounds);
        
        const SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;

        SDL_Log("Creating window of size %dx%d at scale %.2f\n", (int)(display_bounds.w * main_scale), (int)(display_bounds.h * main_scale), main_scale);
        
        this->window = SDL_CreateWindow("Chroma", (int)(display_bounds.w * main_scale), (int)(display_bounds.h * main_scale), window_flags);
        if (window == nullptr)
        {
            SDL_Log("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
            return 1;
        }
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(window);

        return 0;
    }

    int App::create_device() {
        // Create GPU Device
        this->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB,true,nullptr);
        if (device == nullptr)
        {
            SDL_Log("Error: SDL_CreateGPUDevice(): %s\n", SDL_GetError());
            return 1;
        }

        // Claim window for GPU Device
        if (!SDL_ClaimWindowForGPUDevice(device, window))
        {
            SDL_Log("Error: SDL_ClaimWindowForGPUDevice(): %s\n", SDL_GetError());
            return 1;
        }
        SDL_SetGPUSwapchainParameters(device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC);

        return 0;
    }

    int App::setup() {
        int err = 0;
        if ((err = create_window()) != 0) {
            return err;
        }
        if ((err = create_device()) != 0) {
            return err;
        }
        return 0;
    }

    int App::setup_imgui() {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGuiContext *ctx = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        
        ctx->DebugLogFlags |= ImGuiDebugLogFlags_EventDocking;
        
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        const float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

        // Setup scaling
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

        // Setup Platform/Renderer backends
        if (!ImGui_ImplSDL3_InitForSDLGPU(window)) {
            SDL_Log("Error: ImGui_ImplSDL3_InitForSDLGPU(): %s\n", SDL_GetError());
            return 1;
        }
        
        ImGui_ImplSDLGPU3_InitInfo init_info = {};
        init_info.Device = device;
        init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(device, window);
        init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;                      // Only used in multi-viewports mode.
        init_info.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;  // Only used in multi-viewports mode.
        init_info.PresentMode = SDL_GPU_PRESENTMODE_VSYNC;
        
        if (!ImGui_ImplSDLGPU3_Init(&init_info)) {
            SDL_Log("Error: ImGui_ImplSDLGPU3_Init(): %s\n", SDL_GetError());
            return 1;
        }

        return 0;
    }

    int App::setup_imgui_dockspace() {
        // static bool layout_initialized = false;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("DockSpace Window", NULL, window_flags);
        ImGui::PopStyleVar(2);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        ImGui::End();

        // Clear out existing layout
        ImGui::DockBuilderRemoveNode(dockspace_id);             // Clear any previous layout
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoTabBar); // Add root node
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        // Split into major regions
        ImGuiID dock_main = dockspace_id;
        ImGuiID dock_palette;
        ImGuiID dock_colorpick;
        ImGuiID dock_layer;

        // Split right 20% (Inspector)
        dock_palette = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.10f, nullptr, &dock_main);
        dock_colorpick = ImGui::DockBuilderSplitNode(dock_palette, ImGuiDir_Down, 0.30f, nullptr, &dock_palette);
        dock_layer = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.20f, nullptr, &dock_main);
        // // Split bottom 25% (Console)
        // dock_id_down = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);

        // Dock windows
        ImGui::DockBuilderDockWindow("Debug", dock_main);
        ImGui::DockBuilderDockWindow("Palette", dock_palette);
        ImGui::DockBuilderDockWindow("ColorPick", dock_colorpick);
        ImGui::DockBuilderDockWindow("Layer", dock_layer);

        ImGui::DockBuilderFinish(dockspace_id);

        return 0;
    }

    int App::imgui_dockspace() {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("DockSpace Window", NULL, window_flags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        // Menubar possibly here i dunno

        ImGui::End();

        return 0;
    }

    int App::process_events() {
        static uint64_t count = 0;

        count++;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            idle = false; // Wake up on any event
            count = 0;
            
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
            if (event.type == SDL_EVENT_WINDOW_MINIMIZED && event.window.windowID == SDL_GetWindowID(window))
                idle = true; // Idle when minimized
        }

        if (count >= 20) {
            idle = true; // Idle if no events for a while
        }

        return 0;
    }

}
