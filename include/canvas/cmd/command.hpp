/**
 * @file command.hpp
 * @author Kitsune (https://github.com/Agakitsune)
 * @brief 
 * @version 0.1
 * @date 2026-02-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

#include "SDL3/SDL.h"

// #include "color.hpp"

namespace chroma {

    struct Layer;

    /**
     * @brief Base class for all Command
     * 
     */
    class Command {
      protected:
        // Layer data
        SDL_Surface *surface;
        SDL_Texture *texture;

      public:
        virtual ~Command() noexcept = default;

        /**
         * @brief Execute command on layer data
         * 
         * @param selection The current selection
         */
        virtual void redo(SDL_Rect &selection) noexcept = 0;

        /**
         * @brief Undo the command on layer data
         * 
         * @param selection The current selection
         */
        virtual void undo(SDL_Rect &selection) noexcept = 0;

        void set_layer(const Layer &layer) noexcept;
    };

} // namespace chroma
