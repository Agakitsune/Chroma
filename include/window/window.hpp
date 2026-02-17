/**
 * @file window.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief 
 * @version 0.1
 * @date 2026-02-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
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

            virtual void ready() noexcept {};
            virtual void display() noexcept = 0;

            // bool begin();
            // bool render();
            // void end();
        
        protected:
            std::string label;
            ImGuiWindowFlags flags;
    };
}
