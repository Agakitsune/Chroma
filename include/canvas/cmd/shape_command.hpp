
#pragma once

#include "mouse_command.hpp"

#include <vector>

namespace chroma {

class ShapeCommand : public MouseCommand {
  SDL_Point s;
  SDL_Point e;
  SDL_Rect rect = {0, 0, 0, 0};

  uint8_t *previous_data = nullptr;

public:
  ShapeCommand() noexcept = default;
  ShapeCommand(const ShapeCommand &cmd) noexcept : MouseCommand(cmd) {}
  virtual ~ShapeCommand() noexcept override = default;

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

  virtual std::unique_ptr<MouseCommand> next() noexcept override final;
};

} // namespace chroma
