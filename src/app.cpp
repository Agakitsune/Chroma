
#define IMGUI_DEFINE_MATH_OPERATORS

#include "app.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlgpu3.h"
#include "lua/api.hpp"

#include "cursor.hpp"

#include <iostream>
#include <vector>

struct ImGui_ImplSDLGPU3_Data
{
    ImGui_ImplSDLGPU3_InitInfo   InitInfo;

    // Graphics pipeline & shaders
    SDL_GPUShader*               VertexShader           = nullptr;
    SDL_GPUShader*               FragmentShader         = nullptr;
    SDL_GPUGraphicsPipeline*     Pipeline               = nullptr;
    SDL_GPUSampler*              TexSamplerLinear       = nullptr;
    SDL_GPUTransferBuffer*       TexTransferBuffer      = nullptr;
    uint32_t                     TexTransferBufferSize  = 0;
};

namespace chroma {
    App* App::instance = nullptr;

    App::~App() noexcept {
        lua_close(state);

        SDL_WaitForGPUIdle(device);

        ImGui_ImplSDL3_Shutdown();
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui::DestroyContext();

        SDL_ReleaseWindowFromGPUDevice(device, window);
        SDL_DestroyGPUDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    int App::init() noexcept {
        int err = 0;

        instance = this;

        if ((err = setup()) != 0) {
            return err;
        }

        if ((err = setup_imgui()) != 0) {
            return err;
        }

        state = luaL_newstate();
        if (!state) {
            return -1;
        }

        luaL_openlibs(state);

        lua::register_chroma_api(state);

        windows["Viewport"] = std::make_unique<ViewportWindow>();
        windows["ColorPicker"] = std::make_unique<ColorPickerWindow>();
        windows["Palette"] = std::make_unique<PaletteWindow>();

        return 0;
    }

    int App::run() noexcept {
        uint64_t tick = 0;
        uint64_t delta = 0;

        ImGuiIO& io = ImGui::GetIO();

        // bool a = luaL_dofile(state, "../test.lua");
        // if (a != LUA_OK) {
        //     const char *msg = lua_tostring(state, -1);
        //     std::cerr << "Lua Error: " << msg << std::endl;
        //     lua_pop(state, 1);
        // }

        // lua_getglobal(state, "update");
        // if (!lua_isfunction(state, -1)) {
        //     std::cerr << "Lua Error: 'update' is not a function" << std::endl;
        //     lua_pop(state, 1);
        //     return -1;
        // }

        // int update_ref = luaL_ref(state, LUA_REGISTRYINDEX);

        while (!done) {
            process_events(delta);

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

            ImGui_ImplSDLGPU3_Data *bd = (ImGui_ImplSDLGPU3_Data*)io.BackendRendererUserData;

            if (bd->TexSamplerLinear != sampler) {
                if (bd->TexSamplerLinear) {
                    SDL_ReleaseGPUSampler(device, bd->TexSamplerLinear);
                }
                bd->TexSamplerLinear = sampler;
            }

            CursorManager::update();

            CursorManager::set_cursor(Cursor::Default);

            // lua_rawgeti(state, LUA_REGISTRYINDEX, update_ref);
            // if (lua_pcall(state, 0, 0, 0) != LUA_OK) {
            //     const char *msg = lua_tostring(state, -1);
            //     std::cerr << "Lua Error: " << msg << std::endl;
            //     lua_pop(state, 1);
            // }

            // ImGui::SetMouseCursor(ImGuiMouseCursor_None);

            if (!dockspace_initialized) {
                setup_imgui_dockspace();
                dockspace_initialized = true;
            } else {
                imgui_dockspace();
            }
            // imgui_dockspace();

            // viewport_window.display();

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

            // ImGui::Begin("Viewport", nullptr, window_flags);

            // ImGuiWindow* w = ImGui::GetCurrentWindow();
            // ImDrawList* draw_list = w->DrawList;

            // int color_perLine = 5;
            // int total_color_nb = button_colors.size();
            // int rows = (total_color_nb + color_perLine - 1) / color_perLine;

            // std::cout << "Total colors: " << total_color_nb << ", Rows: " << rows << std::endl;
            // if (ImGui::BeginTable("table1", color_perLine))
            // {
            //     int color_nb = 0;

            //     for (int row = 0; row < rows; row++)
            //     {
            //         ImGui::TableNextRow();
            //         for (int column = 0; column < color_perLine; column++)
            //         {
            //             ImGui::TableSetColumnIndex(column);
            //             if (color_nb >= total_color_nb)
            //                 continue;
            //             std::string button_id = "ColorButton##" + std::to_string(color_nb);
            //             if (ImGui::ColorButton(button_id.c_str(), button_colors[color_nb], ImGuiColorEditFlags_NoTooltip))
            //             {
            //                 main_color.r = button_colors[color_nb].x;
            //                 main_color.g = button_colors[color_nb].y;
            //                 main_color.b = button_colors[color_nb].z;
            //                 main_color.a = button_colors[color_nb].w;
            //             }
            //             if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            //             {
            //                 std::cout << "Right clicked on color button " << color_nb << std::endl;
            //                 button_colors.erase(button_colors.begin() + color_nb);
            //             }
            //             color_nb++;
            //         }
            //     }
            //     ImGui::EndTable();
            // }
            // ImGui::End();


            // ImGui::Begin("ColorPick", nullptr, window_flags);

            // // SDL_Texture *old_target = SDL_GetRenderTarget(renderer);
            // // SDL_Texture *target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)window_size.x, (int)window_size.y);

            // // SDL_SetRenderTarget(renderer, target);

            // // SDL_SetRenderDrawColorFloat(renderer, 1.0f, 0.1f, 0.1f, 1.0f);
            // // SDL_RenderClear(renderer);

            // // SDL_SetRenderTarget(renderer, old_target);

            // draw_list->PushClipRectFullScreen();
            // draw_list->AddRectFilled(origin, origin + window_size, IM_COL32(101, 85, 97, 255));
            // draw_list->PopClipRect();

            // draw_list->AddRectFilled(
            //     canvas_offset - ImVec2(2, 2),
            //     canvas_offset + canvas_size + ImVec2(2, 2),
            //     IM_COL32(0, 0, 0, 255)
            // );

            // ImGui::RenderColorRectWithAlphaCheckerboard(
            //     draw_list,
            //     canvas_offset,
            //     canvas_offset + canvas_size,
            //     0,
            //     16.0f,
            //     ImVec2(0, 0)
            // );

            // draw_list->AddImage(
            //     (ImTextureRef)(uintptr_t)canvas,
            //     canvas_offset,
            //     canvas_offset + canvas_size
            // );

            // ImVec2 avail = ImGui::GetContentRegionAvail();

            // ImVec2 sv_picker_size(avail.x, 150.0f);

            // // float sv_picker_size = 250.0f;
            // bool value_changed_sv = false;
            // bool value_changed_h = false;
            // bool value_changed = false;

            // ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
            // // SV rectangle logic
            // ImGui::InvisibleButton("sv", sv_picker_size);
            // bool right_clicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);

            // if (ImGui::IsItemActive()) // click on colorpicker with left mouse button
            // {
            //     S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size.x - 1));
            //     V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size.y - 1));
            //     IM_ASSERT(g.ColorEditCurrentID != 0);
            //     if (!(g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(main_color[0], main_color[1], main_color[2], 0)))) {
            //         // return;
            //         H = g.ColorEditSavedHue;
            //     }
            //     // ImGui::ColorEditRestoreH(&main_color.r, &H); // Greatly reduces hue jitter and reset to 0 when hue == 255 and color is rapidly modified using SV square.
            //     value_changed_sv = true;
            // }
            // if (right_clicked) { // click on colorpicker with right mouse button
            //     S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size.x - 1));
            //     V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size.y - 1));
            //     IM_ASSERT(g.ColorEditCurrentID != 0);
            //     if (!(g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(main_color[0], main_color[1], main_color[2], 0)))) {
            //         // return;
            //         H = g.ColorEditSavedHue;
            //     }
            //     // ImGui::ColorEditRestoreH(&main_color.r, &H); // Greatly reduces hue jitter and reset to 0 when hue == 255 and color is rapidly modified using SV square.
            //     value_changed_sv = true;
            //     button_colors.push_back(ImVec4(main_color[0], main_color[1], main_color[2], 1.0f));
            // }

            // ImGui::End();

            // palette_window.display();

            // ImGui::Begin("Palette", nullptr, window_flags);

            // if (ImGui::ColorButton("#1", color_picker.main_color, ImGuiColorEditFlags_NoTooltip)) {
            //     // main_color.r = 1.0;
            //     // main_color.g = 0.0;
            //     // main_color.b = 0.0;
            //     // main_color.a = 1.0;
            // }

            // ImGui::End();

            // for ((label, window))

            // color_picker.display();

            for (const auto& [label, window] : windows) {
                window->display();
            }

            ImGui::Begin("Layer", nullptr, window_flags);
            ImGui::Text("Layer and shit");
            ImGui::End();

            // Rendering
            ImGui::Render();
            ImDrawData* draw_data = ImGui::GetDrawData();
            const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

            SDL_GPUCommandBuffer *cmd_buffer = SDL_AcquireGPUCommandBuffer(device);

            ViewportWindow *viewport = get_window<ViewportWindow>("Viewport");
            if (!viewport->is_empty()) {
                Canvas &canvas = viewport->get_canvas();

                if (!canvas.pending.empty()) {
                    canvas.execute_pending();
                }

                SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(cmd_buffer);

                canvas.upload(copy_pass);

                SDL_EndGPUCopyPass(copy_pass);

                SDL_GPUColorTargetInfo target_info = {};
                target_info.texture = canvas.preview;
                target_info.clear_color = SDL_FColor { 1.0f, 1.0f, 1.0f, 0.0f };
                target_info.load_op = SDL_GPU_LOADOP_CLEAR;
                target_info.store_op = SDL_GPU_STOREOP_STORE;
                target_info.mip_level = 0;
                target_info.layer_or_depth_plane = 0;
                target_info.cycle = false;
                SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd_buffer, &target_info, 1, nullptr);
                SDL_EndGPURenderPass(render_pass);
            }

            SDL_GPUTexture *swapchain_texture;
            SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buffer, window, &swapchain_texture, nullptr, nullptr);

            if (swapchain_texture != nullptr && !is_minimized)
            {
                // This is mandatory: call ImGui_ImplSDLGPU3_PrepareDrawData() to upload the vertex/index buffer!
                ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, cmd_buffer);
    
                // Setup and start a render pass
                SDL_GPUColorTargetInfo target_info = {};
                target_info.texture = swapchain_texture;
                target_info.clear_color = SDL_FColor { 0.07f, 0.07f, 0.07f, 1.0f };
                target_info.load_op = SDL_GPU_LOADOP_CLEAR;
                target_info.store_op = SDL_GPU_STOREOP_STORE;
                target_info.mip_level = 0;
                target_info.layer_or_depth_plane = 0;
                target_info.cycle = false;
                SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd_buffer, &target_info, 1, nullptr);
    
                // Render ImGui
                ImGui_ImplSDLGPU3_RenderDrawData(draw_data, cmd_buffer, render_pass);
    
                SDL_EndGPURenderPass(render_pass);
            }
    
            SDL_SubmitGPUCommandBuffer(cmd_buffer);

            const uint64_t end_tick = SDL_GetTicks();
            delta = end_tick - tick;
        }
        return 0;
    }

    App* App::get_instance() noexcept
    {
        return instance;
    }

    SDL_GPUDevice *App::get_device() noexcept
    {
        if (!instance) return nullptr;
        return instance->device;
    }

    int App::create_window() noexcept
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
        {
            SDL_Log("Error: SDL_Init(): %s\n", SDL_GetError());
            return 1;
        }

        // Create SDL window graphics context
        const float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

        SDL_Rect display_bounds;
        SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &display_bounds);

        const SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_VULKAN;

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

    int App::create_device() noexcept {
        // Create GPU Device

        this->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
        if (device == nullptr) {
            SDL_Log("Error: SDL_CreateGPUDevice(): %s\n", SDL_GetError());
            return 1;
        }

        if (!SDL_ClaimWindowForGPUDevice(device, window)) {
            SDL_Log("Error: SDL_ClaimWindowForGPUDevice(): %s\n", SDL_GetError());
            return 1;
        }
        SDL_SetGPUSwapchainParameters(device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_IMMEDIATE);

        return 0;
    }

    int App::setup() noexcept {
        int err = 0;
        if ((err = create_window()) != 0) {
            return err;
        }
        if ((err = create_device()) != 0) {
            return err;
        }
        return 0;
    }

    int App::setup_imgui() noexcept {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGuiContext *ctx = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ctx->DebugLogFlags |=
        // ImGuiDebugLogFlags_EventDocking |
        // ImGuiDebugLogFlags_EventPopup |
        0;

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
        init_info.PresentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;

        if (!ImGui_ImplSDLGPU3_Init(&init_info)) {
            SDL_Log("Error: ImGui_ImplSDLGPU3_Init(): %s\n", SDL_GetError());
            return 1;
        }

        SDL_GPUSamplerCreateInfo sampler_info = {};
        sampler_info.min_filter = SDL_GPU_FILTER_NEAREST;
        sampler_info.mag_filter = SDL_GPU_FILTER_NEAREST;
        sampler_info.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
        sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        sampler_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        sampler_info.mip_lod_bias = 0.0f;
        sampler_info.min_lod = -1000.0f;
        sampler_info.max_lod = 1000.0f;
        sampler_info.enable_anisotropy = false;
        sampler_info.max_anisotropy = 1.0f;
        sampler_info.enable_compare = false;

        this->sampler = SDL_CreateGPUSampler(device, &sampler_info);
        if (sampler == nullptr) {
            SDL_Log("Error: SDL_CreateGPUSampler(): %s\n", SDL_GetError());
            return 1;
        }

        return 0;
    }

    int App::setup_imgui_dockspace() noexcept {
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
        ImGui::DockBuilderDockWindow("Viewport", dock_main);
        ImGui::DockBuilderDockWindow("Palette", dock_palette);
        ImGui::DockBuilderDockWindow("Color Pick", dock_colorpick);
        ImGui::DockBuilderDockWindow("Layer", dock_layer);

        ImGui::DockBuilderFinish(dockspace_id);

        return 0;
    }

    int App::imgui_dockspace() noexcept {
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

        ImGui::BeginMenuBar();

        if (ImGui::BeginMenu("File")) {
            // ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_N); // doesn't woerk, need to find another way
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                // New file action
                ImGui::PushOverrideID(32);
                ImGui::OpenPopup("New");
                w = 16;
                h = 16;
                ImGui::PopID();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                done = true;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();

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
                get_window<ViewportWindow>("Viewport")->new_canvas(w, h);
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

        // Menubar possibly here i dunno

        ImGui::End();

        return 0;
    }

    int App::process_events(float delta) noexcept {
        static float time = 0;

        time += delta / 1000.0f;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            idle = false; // Wake up on any event
            time = 0;

            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
            if (event.type == SDL_EVENT_WINDOW_MINIMIZED && event.window.windowID == SDL_GetWindowID(window))
                idle = true; // Idle when minimized
        }

        if (time >= 3.0f) {
            idle = true; // Idle if no events for a while
        }

        return 0;
    }

}
