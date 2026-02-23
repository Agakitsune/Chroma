/**
 * @file menuitem.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

namespace chroma {

/**
 * @brief Base class for menu items
 *
 */
class MenuItem {
public:
    MenuItem() noexcept = default;
    virtual ~MenuItem() noexcept = default;

    /**
     * @brief Used to render the ImGui::MenuItem, can be used to add multiple
     * items
     *
     */
    virtual void menubar() noexcept = 0;

    /**
     * @brief Define an action when executed (Deprecated)
     *
     */
    virtual void action() noexcept {};

    /**
     * @brief Display Modals windows and such
     *
     */
    virtual void display() noexcept {};

    /**
     * @brief Process shortcuts
     *
     */
    virtual void shortcuts() noexcept {};
};
} // namespace chroma
