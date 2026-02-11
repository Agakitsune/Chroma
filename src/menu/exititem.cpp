
#include "menu/exititem.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "app.hpp"

namespace chroma {

    void ExitMenuItem::menubar() noexcept
    {
        if (ImGui::MenuItem("Exit", "Ctrl+Q")) {
            SDL_Event event;
            event.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&event);
        }
    }

    void ExitMenuItem::shortcuts() noexcept
    {
        if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Q)) {
            SDL_Event event;
            event.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&event);
            // App::get_instance()->emit_signal("edit_flipv");
        }
    }
}
