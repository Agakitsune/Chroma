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
 * @brief Select all
 *
 */
class MarkCommand : public Command {
public:
    SDL_Rect rect = { 0, 0, 0, 0 };
    SDL_Rect previous = { 0, 0, 0, 0 };

public:
    MarkCommand() noexcept = default;
    virtual ~MarkCommand() noexcept override = default;

    virtual void redo(SDL_Rect& selection) noexcept override final;
    virtual void undo(SDL_Rect& selection) noexcept override final;
};

}
