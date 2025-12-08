
#pragma once

#include "imgui.h"

#include <string>

namespace chroma {

    class Window {
        public:
            Window(
                const std::string &label,
                ImGuiWindowFlags flags
            );

            bool begin();
            bool render();
            void end();
        
        private:
            std::string label;
            ImGuiWindowFlags flags;
    };
}
