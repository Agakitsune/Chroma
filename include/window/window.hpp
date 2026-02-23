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

/**
 * @brief Base class for the windows
 *
 */
class Window {
public:
    /**
     * @brief Construct a new Window object
     *
     * @param label The window label
     * @param flags The Window flags
     */
    Window(const std::string& label, ImGuiWindowFlags flags) noexcept;

    virtual ~Window() noexcept = default;

    /**
     * @brief Called at the start of the application, used to connect signals to
     * methods
     *
     */
    virtual void ready() noexcept {};

    /**
     * @brief Display the Window content
     *
     */
    virtual void display() noexcept = 0;

    // bool begin();
    // bool render();
    // void end();

protected:
    std::string label;
    ImGuiWindowFlags flags;
};
} // namespace chroma
