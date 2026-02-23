
#pragma once

#include "mouse_command.hpp"

#include <vector>

namespace chroma {

class SelectMarkCommand : public MouseCommand {
  SDL_Point s;
  SDL_Point e;
  SDL_Rect rect = {0, 0, 0, 0};
  SDL_Rect previous = {0, 0, 0, 0};

  // uint8_t *previous_data = nullptr;

public:
  SelectMarkCommand() noexcept = default;
  SelectMarkCommand(const SelectMarkCommand &cmd) noexcept
      : MouseCommand(cmd) {}
  virtual ~SelectMarkCommand() noexcept override = default;

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

  virtual bool can_transform() noexcept { return true; }

  virtual std::unique_ptr<MouseCommand> next() noexcept override final;
  virtual std::unique_ptr<MouseCommand> transform() noexcept override final;
};

} // namespace chroma
