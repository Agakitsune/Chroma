
#define IMGUI_DEFINE_MATH_OPERATORS

#include "app.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "lua/api.hpp"

#include "cursor.hpp"

#include <iostream>

namespace chroma {
    App* App::instance = nullptr;

    App::~App() noexcept {
        lua_close(state);

        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        SDL_DestroyRenderer(renderer);
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

        register_lua(state);

        SDL_SetRenderTarget(renderer, nullptr);

        return 0;
    }

    int App::run() noexcept {
        uint64_t tick = 0;
        uint64_t delta = 0;

        ImGuiIO& io = ImGui::GetIO();

        luaL_dofile(state, "../test.lua");

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
            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            CursorManager::update();

            CursorManager::set_cursor(Cursor::Default);

            // ImGui::SetMouseCursor(ImGuiMouseCursor_None);

            if (!dockspace_initialized) {
                setup_imgui_dockspace();
                dockspace_initialized = true;
            } else {
                imgui_dockspace();
            }
            // imgui_dockspace();

            viewport_window.display();

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

            // ImGui::Begin("Viewport", nullptr, window_flags);

            // ImGuiWindow* w = ImGui::GetCurrentWindow();
            // ImDrawList* draw_list = w->DrawList;

            // ImVec2 pad = ImGui::GetStyle().WindowPadding;

            // ImVec2 window_size = ImGui::GetContentRegionAvail() + pad * 2.0f;
            // ImVec2 canvas_size = ImVec2(64, 64);

            // ImVec2 origin = w->DC.CursorPos - pad;
            // ImVec2 canvas_offset = origin + (window_size - canvas_size) * 0.5f;

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

            // // draw_list->AddImage(
            // //     (ImTextureRef)(uintptr_t)canvas,
            // //     w->DC.CursorPos,
            // //     w->DC.CursorPos + window_size
            // // );

            // ImGui::End();

            ImGui::Begin("Palette", nullptr, window_flags);

            if (ImGui::ColorButton("#1", color_picker.main_color, ImGuiColorEditFlags_NoTooltip)) {
                // main_color.r = 1.0;
                // main_color.g = 0.0;
                // main_color.b = 0.0;
                // main_color.a = 1.0;
            }

            ImGui::End();

            // for ((label, window))

            color_picker.display();

            ImGui::Begin("Layer", nullptr, window_flags);
            ImGui::Text("Layer and shit");
            ImGui::End();

            // Rendering
            ImGui::Render();
            ImDrawData* draw_data = ImGui::GetDrawData();
            const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

            SDL_SetRenderTarget(renderer, nullptr);

            SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
            SDL_SetRenderDrawColorFloat(renderer, 0.07f, 0.07f, 0.07f, 1.0f);
            SDL_RenderClear(renderer);

            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
            SDL_RenderPresent(renderer);

            const uint64_t end_tick = SDL_GetTicks();
            delta = end_tick - tick;
        }
        return 0;
    }

    SDL_Renderer *App::get_renderer() noexcept
    {
        if (!instance) return nullptr;
        return instance->renderer;
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
        this->renderer = SDL_CreateRenderer(window, "vulkan");
        if (renderer == nullptr)
        {
            SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
            return 1;
        }

        // SDL_SetRenderVSync(renderer, 1);
        SDL_SetRenderDrawColorFloat(renderer, 0.07f, 0.07f, 0.07f, 1.0f);

        // this->canvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 64, 64);

        // if (canvas == nullptr) {
        //     SDL_Log("Error: SDL_CreateTexture(): %s\n", SDL_GetError());
        //     return 1;
        // }

        // SDL_SetRenderTarget(renderer, canvas);
        // SDL_SetRenderDrawColorFloat(renderer, 1.0f, 1.0f, 1.0f, 0.0f);
        // SDL_RenderClear(renderer);

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
        ImGuiDebugLogFlags_EventPopup |
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
        if (!ImGui_ImplSDL3_InitForSDLRenderer(window, renderer)) {
            SDL_Log("Error: ImGui_ImplSDL3_InitForSDLRenderer(): %s\n", SDL_GetError());
            return 1;
        }

        if (!ImGui_ImplSDLRenderer3_Init(renderer)) {
            SDL_Log("Error: ImGui_ImplSDLRenderer3_Init(): %s\n", SDL_GetError());
            return 1;
        }

        return 0;
    }

    // int App::setup_windows() {
    //     windows["color_picker"] = std::make_unique<ColorPickerWindow>();

    //     return 0;
    // }

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
        dock_palette = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.30f, nullptr, &dock_main);
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
                // viewport_window.new_canvas(renderer, 64, 64);
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
                viewport_window.new_canvas(renderer, w, h);
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
