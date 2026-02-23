/**
 * @file select_move_command.hpp
 * @author Kitsune (https://github.com/Agakitsune)
 * @brief
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "mouse_command.hpp"

#include <vector>

namespace chroma {

/**
 * @brief Select Transform
 *
 * @warning This class is unused
 */
class SelectMoveCommand : public MouseCommand {
  SDL_Point s;
  SDL_Point e;

  uint8_t *data = nullptr;
  uint8_t *previous = nullptr;

  SDL_Texture *tex = nullptr;

public:
  SelectMoveCommand() noexcept = default;
  SelectMoveCommand(const MouseCommand &cmd) noexcept : MouseCommand(cmd) {}
  virtual ~SelectMoveCommand() noexcept override = default;

  virtual void redo(SDL_Rect &selection) noexcept override final;
  virtual void undo(SDL_Rect &selection) noexcept override final;

  virtual void start(const SDL_Point &point,
                     SDL_Rect &selection) noexcept override final;
  virtual void update(const SDL_Point &point,
                      SDL_Rect &selection) noexcept override final;
  virtual void end(const SDL_Point &point,
                   SDL_Rect &selection) noexcept override final;
  virtual void discard(SDL_Rect &selection) noexcept override final;

  virtual void render(SDL_Texture *preview, SDL_Texture *overlay,
                      SDL_Rect &selection) noexcept override final;

  virtual bool is_persitent() noexcept { return true; }
  virtual bool can_transform() noexcept { return true; }

  virtual std::unique_ptr<MouseCommand> next() noexcept override final;
  virtual std::unique_ptr<MouseCommand> transform() noexcept override final;

  // virtual bool is_transform() noexcept override final { return true; }
};

} // namespace chroma
