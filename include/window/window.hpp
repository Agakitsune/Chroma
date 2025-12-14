
#pragma once

#include "imgui.h"

#include <string>

namespace chroma {

    class Window {
        public:
            Window(
                const std::string &label,
                ImGuiWindowFlags flags
            ) noexcept;

            virtual ~Window() noexcept = default;

            virtual void display() noexcept = 0;

            // bool begin();
            // bool render();
            // void end();
        
        protected:
            std::string label;
            ImGuiWindowFlags flags;
    };
}
