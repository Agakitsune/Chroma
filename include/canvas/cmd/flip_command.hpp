/**
 * @file select_mark_command.hpp
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

#include <vector>

namespace chroma {

/**
 * @brief Flip
 *
 */
class FlipCommand : public Command {
public:
    SDL_FlipMode mode;

public:
    FlipCommand() noexcept = default;
    virtual ~FlipCommand() noexcept override = default;

    virtual void redo(SDL_Rect& selection) noexcept override final;
    virtual void undo(SDL_Rect& selection) noexcept override final;
};

}
