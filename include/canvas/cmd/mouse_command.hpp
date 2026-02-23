/**
 * @file mouse_command.hpp
 * @author  Kitsune (https://github.com/Agakitsune)
 * @brief 
 * @version 0.1
 * @date 2026-02-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

#include "command.hpp"

// #include "SDL3/SDL.h"

#include "color.hpp"

#include <memory>

namespace chroma {

    /**
     * @brief Base class for all Mouse related command.
     * 
     * - start is called when the left mouse button is down
     * 
     * - update is called every frame with the left mouse button down
     * 
     * - end is called when the button is released
     * 
     * - discard is called when the right mouse button is clicked at any point
     * 
     * 
     */
    class MouseCommand : public Command {
      protected:
        // Color data
        Color main;
        Color second;

      public:
        MouseCommand() noexcept = default;
        MouseCommand(const MouseCommand &cmd) noexcept : main(cmd.main), second(cmd.second) {}
        virtual ~MouseCommand() noexcept override = default;

        /**
         * @brief Starting method, called when the left mouse button is down
         * 
         * @param point The current pixel
         * @param selection The current selection, used to restrict the command
         */
        virtual void start(const SDL_Point &point, SDL_Rect &selection) noexcept = 0;

        /**
         * @brief Update every frame until the left mouse button is released
         * 
         * @param point The current pixel
         * @param selection The current selection, used to restrict the command
         */
        virtual void update(const SDL_Point &point, SDL_Rect &selection) noexcept = 0;

        /**
         * @brief Ending method, called when the left mouse button is released
         * 
         * @param point The current pixel
         * @param selection The current selection
         */
        virtual void end(const SDL_Point &point, SDL_Rect &selection) noexcept = 0;

        /**
         * @brief Discard metho,d called when the right mouse button is clicked at any point during dragging.
         * Reset the command to the beginning state and revert any changes made to the layer data
         * 
         * @param selection The current selection
         */
        virtual void discard(SDL_Rect &selection) noexcept = 0;

        /**
         * @brief Rendering method, called every frame to render visual to help the user see what the command is doing
         * 
         * @param preview A texture that is above the current selected layer, every layer above the current layer will be drawn above this texture
         * @param overlay A texture that is above everything
         * @param selection The current selection
         */
        virtual void render(
            SDL_Texture *preview,
            SDL_Texture *overlay,
            SDL_Rect &selection
        ) noexcept = 0;

        void set_main_color(const Color &color) noexcept;
        void set_second_color(const Color &color) noexcept;

        /**
         * @brief Is the command persistent
         * 
         * When the command is ended (see @ref MouseCommand::end())
         * instead of generating a new one using @ref MouseCommand::next(), it will keep on this one for future use
         * 
         * 
         * @return true The command is persitent
         * @return false The command isn't persitent
         * 
         * @warning This method isn't use in this version, you can override it, but there is no effect
         */
        virtual bool is_persitent() noexcept { return false; }

        /**
         * @brief Can the command transform
         * 
         * Special command can transform into another command when on the current selection if there is any
         * 
         * eg. The Select Tool will move the current selection instead of creating another one
         * 
         * @return true The command can be transformed
         * @return false TThe command cannot be transformed
         * 
         * @warning This method isn't use in this version, you can override it, but there is no effect
         */
        virtual bool can_transform() noexcept { return false; }

        /**
         * @brief Next command, generally it's the same type (BrushCommand -> BrushCommand), but can be used to create another chained command
         * 
         * @return std::unique_ptr<MouseCommand> The next command
         * 
         * @warning although it is possible to return the same instance (it then won't be pushed on the stack),
         * used with @ref MouseCommand::is_persitent may cause some crash
         */
        virtual std::unique_ptr<MouseCommand> next() noexcept = 0;

        /**
         * @brief Transformed command
         * 
         * See @ref MouseCommand::can_transform
         * 
         * @return std::unique_ptr<MouseCommand> The transformed command
         * 
         * @warning although it is possible to return the same instance (it then won't be pushed on the stack),
         * used with @ref MouseCommand::is_persitent may cause some crash
         */
        virtual std::unique_ptr<MouseCommand> transform() noexcept { nullptr; }
    };

} // namespace chroma
