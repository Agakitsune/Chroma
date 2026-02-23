/**
 * @file cursor.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "SDL3/SDL.h"

#include <memory>
#include <vector>

namespace chroma {

    /**
     * @brief Cursor enum
     * 
     */
    enum class Cursor {
        Default, // Default cursor

        Arrow = Default, // Arrow cusor (default)
        Cross, // Cross
        Grab, // Grab
        Picker, // Color Picker
        Point, // Point

        Count
    };

    /**
     * @brief Custom SDL cursor manager
     * 
     * Remplace `ImGui::SetMouseCursor` since there is little customisation on how the cursor looks
     * 
     */
    class CursorManager {
        static std::unique_ptr<CursorManager> instance;

        std::vector<SDL_Cursor *> cursors;

        SDL_Cursor *last_cursor = nullptr;

        Cursor cursor = Cursor::Default;

        CursorManager() noexcept;

        static CursorManager &get_instance() noexcept;

      public:
        ~CursorManager() noexcept;

        /**
         * @brief Set the cursor object
         * 
         * @param cursor The @ref Cursor type
         */
        static void set_cursor(Cursor cursor) noexcept;

        /**
         * @brief Update the instance
         * 
         */
        static void update() noexcept;
    };

} // namespace chroma
