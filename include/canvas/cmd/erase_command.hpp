
#pragma once

#include "mouse_command.hpp"

#include <vector>

namespace chroma {

class EraseCommand : public MouseCommand {
  std::vector<SDL_Point> points;
  std::vector<Color> old;

public:
  EraseCommand() noexcept = default;
  EraseCommand(const EraseCommand &cmd) noexcept : MouseCommand(cmd) {}
  virtual ~EraseCommand() noexcept override = default;

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
