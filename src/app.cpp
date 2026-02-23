/**
 * @file app.cpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#define IMGUI_DEFINE_MATH_OPERATORS

#include "app.hpp"

#include "menu/exititem.hpp"
#include "menu/flipitem.hpp"
#include "menu/layeritem.hpp"
#include "menu/newitem.hpp"
#include "menu/openitem.hpp"
#include "menu/saveitem.hpp"
#include "menu/undoredoitem.hpp"

#include "window/color_picker.hpp"
#include "window/layer.hpp"
#include "window/palette.hpp"
#include "window/viewport.hpp"

#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "imgui.h"
#include "imgui_internal.h"

#include "cursor.hpp"

#include <iostream>
#include <vector>

namespace chroma {
    App *App::instance = nullptr;

    App::~App() noexcept {
        windows.clear(); // Release all windows and their resources

        SDL_WaitForGPUIdle(device);

        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        // ImGui_ImplSDLGPU3_Shutdown();
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

        add_signal<uint32_t, uint32_t>("create_canvas_requested");
        add_signal<const std::filesystem::path &, const std::filesystem::path &,
                   FileFormat>("save_canvas_requested");
        add_signal<const std::filesystem::path &, const std::filesystem::path &,
                   FileFormat>("open_canvas_requested");

        add_signal<Canvas *>("canvas_selected");

        add_signal("edit_fliph");
        add_signal("edit_flipv");
        add_signal("edit_undo");
        add_signal("edit_redo");

        add_signal("layer_new");
        add_signal("layer_delete");

        add_signal<SDL_Rect>("select_mark");
        add_signal("select_move_start");
        add_signal<SDL_Point>("select_move");
        add_signal("select_move_end");
        add_signal("select_clear");

        add_signal("popup_save");

        windows.push_back(std::make_unique<ViewportWindow>());
        windows.push_back(std::make_unique<ColorPickerWindow>());
        windows.push_back(std::make_unique<PaletteWindow>());
        windows.push_back(std::make_unique<LayerWindow>());

        for (const auto &win : windows) {
            win->ready();
        }

        add_menu<NewMenuItem>("File");
        add_menu<OpenMenuItem>("File");
        separator("File");
        SaveMenuItem *save_menu = add_menu<SaveMenuItem>("File");
        separator("File");
        add_menu<ExitMenuItem>("File");

        add_menu<UndoRedoMenuItem>("Edit");
        separator("Edit");
        add_menu<FlipMenuItem>("Edit");

        add_menu<LayerMenuItem>("Layer");

        connect_signal("popup_save", save_menu, &SaveMenuItem::action);

        emit_signal<const Color &>("main_color_selected", WHITE);
        emit_signal<const Color &>("second_color_selected", BLACK);

        return 0;
    }

    int App::run() noexcept {
        uint64_t tick = 0;
        uint64_t delta = 0;

        ImGuiIO &io = ImGui::GetIO();

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

            for (auto &[menu, items] : menu_bar) {
                for (auto &item : items) {
                    if (item) {
                        item->shortcuts();
                    }
                }
            }

            CursorManager::update();

            CursorManager::set_cursor(Cursor::Default);

            if (!dockspace_initialized) {
                setup_imgui_dockspace();
                dockspace_initialized = true;
            } else {
                imgui_dockspace();
            }

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoMove;

            for (const auto &window : windows) {
                window->display();
            }

            // Rendering
            ImGui::Render();
            ImDrawData *draw_data = ImGui::GetDrawData();
            const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f ||
                                       draw_data->DisplaySize.y <= 0.0f);

            SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x,
                               io.DisplayFramebufferScale.y);
            SDL_SetRenderDrawColorFloat(renderer, 0.07f, 0.07f, 0.07f, 1.0f);
            SDL_RenderClear(renderer);
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(),
                                                  renderer);
            SDL_RenderPresent(renderer);

            const uint64_t end_tick = SDL_GetTicks();
            delta = end_tick - tick;
        }
        return 0;
    }

    App *App::get_instance() noexcept { return instance; }

    SDL_Renderer *App::get_renderer() noexcept {
        if (!instance)
            return nullptr;
        return instance->renderer;
    }
    
    int App::create_window() noexcept {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_Log("Error: SDL_Init(): %s\n", SDL_GetError());
            return 1;
        }

        // Create SDL window graphics context
        const float main_scale =
            SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

        SDL_Rect display_bounds;
        SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &display_bounds);

        const SDL_WindowFlags window_flags =
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN |
            SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_VULKAN;

        SDL_Log("Creating window of size %dx%d at scale %.2f\n",
                (int)(display_bounds.w * main_scale),
                (int)(display_bounds.h * main_scale), main_scale);

        this->window = SDL_CreateWindow(
            "Chroma", (int)(display_bounds.w * main_scale),
            (int)(display_bounds.h * main_scale), window_flags);
        if (window == nullptr) {
            SDL_Log("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
            return 1;
        }
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(window);

        return 0;
    }

    int App::create_device() noexcept {
        this->renderer = SDL_CreateRenderer(this->window, "opengl");
        if (this->renderer == nullptr) {
            SDL_Log("Error: SDL_CreateGPURenderer(): %s\n", SDL_GetError());
        }

        SDL_SetRenderVSync(this->renderer, 1);

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
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        ctx->DebugLogFlags |=
            // ImGuiDebugLogFlags_EventDocking |
            // ImGuiDebugLogFlags_EventPopup |
            0;

        io.ConfigFlags |=
            ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        const float main_scale =
            SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

        // Setup scaling
        ImGuiStyle &style = ImGui::GetStyle();
        style.ScaleAllSizes(
            main_scale); // Bake a fixed style scale. (until we have a solution
                         // for dynamic style scaling, changing this requires
                         // resetting Style + calling this again)
        style.FontScaleDpi =
            main_scale; // Set initial font scale. (using
                        // io.ConfigDpiScaleFonts=true makes this unnecessary.
                        // We leave both here for documentation purpose)

        if (!ImGui_ImplSDL3_InitForSDLRenderer(window, renderer)) {
            SDL_Log("Error: ImGui_ImplSDL3_InitForSDLRenderer(): %s\n",
                    SDL_GetError());
            return 1;
        }

        if (!ImGui_ImplSDLRenderer3_Init(renderer)) {
            SDL_Log("Error: ImGui_ImplSDLRenderer3_Init(): %s\n",
                    SDL_GetError());
            return 1;
        }

        return 0;
    }

    int App::setup_imgui_dockspace() noexcept {
        // static bool layout_initialized = false;

        ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                        ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("DockSpace Window", NULL, window_flags);
        ImGui::PopStyleVar(2);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        ImGui::End();

        // Clear out existing layout
        ImGui::DockBuilderRemoveNode(dockspace_id); // Clear any previous layout
        ImGui::DockBuilderAddNode(
            dockspace_id,
            ImGuiDockNodeFlags_DockSpace |
                ImGuiDockNodeFlags_NoTabBar); // Add root node
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        // Split into major regions
        ImGuiID dock_main = dockspace_id;
        ImGuiID dock_palette;
        ImGuiID dock_colorpick;
        ImGuiID dock_layer;

        // Split right 20% (Inspector)
        dock_palette = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left,
                                                   0.10f, nullptr, &dock_main);
        dock_colorpick = ImGui::DockBuilderSplitNode(
            dock_palette, ImGuiDir_Down, 0.30f, nullptr, &dock_palette);
        dock_layer = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down,
                                                 0.20f, nullptr, &dock_main);
        // // Split bottom 25% (Console)
        // dock_id_down = ImGui::DockBuilderSplitNode(dock_main_id,
        // ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);

        // Dock windows
        ImGui::DockBuilderDockWindow("Viewport", dock_main);
        ImGui::DockBuilderDockWindow("Palette", dock_palette);
        ImGui::DockBuilderDockWindow("Color Pick", dock_colorpick);
        ImGui::DockBuilderDockWindow("Layer", dock_layer);

        ImGui::DockBuilderFinish(dockspace_id);

        return 0;
    }

    int App::imgui_dockspace() noexcept {
        ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        window_flags |= ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                        ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("DockSpace Window", NULL, window_flags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        ImGui::BeginMenuBar();

        for (auto &[menu, items] : menu_bar) {
            if (ImGui::BeginMenu(menu.c_str())) {
                for (auto &item : items) {
                    if (item) {
                        item->menubar();
                    } else {
                        ImGui::Separator();
                    }
                }
                ImGui::EndMenu();
            }
        }

        ImGui::EndMenuBar();

        for (auto &[menu, items] : menu_bar) {
            for (auto &item : items) {
                if (item) {
                    item->display();
                }
            }
        }

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
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == SDL_GetWindowID(window))
                done = true;
            if (event.type == SDL_EVENT_WINDOW_MINIMIZED &&
                event.window.windowID == SDL_GetWindowID(window))
                idle = true; // Idle when minimized
        }

        if (time >= 3.0f) {
            idle = true; // Idle if no events for a while
        }

        return 0;
    }

} // namespace chroma
