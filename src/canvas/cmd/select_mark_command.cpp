
#include "canvas/cmd/select_mark_command.hpp"
#include "canvas/canvas.hpp"
#include "canvas/cmd/select_move_command.hpp"

#include "SDL3/SDL.h"

#include "app.hpp"

namespace chroma {

void SelectMarkCommand::redo(SDL_Rect &selection) noexcept { selection = rect; }

void SelectMarkCommand::undo(SDL_Rect &selection) noexcept {
  selection = previous;
}

void SelectMarkCommand::start(const SDL_Point &point,
                              SDL_Rect &selection) noexcept {
  previous = selection;

  s = point;
  rect.w = 0;
}

void SelectMarkCommand::update(const SDL_Point &point,
                               SDL_Rect &selection) noexcept {
  e = point;
  SDL_Point min = SDL_Point{std::min(s.x, e.x), std::min(s.y, e.y)};
  SDL_Point max = SDL_Point{std::max(s.x, e.x), std::max(s.y, e.y)};
  this->rect = SDL_Rect{min.x, min.y, max.x - min.x + 1, max.y - min.y + 1};
}

void SelectMarkCommand::end(const SDL_Point &point,
                            SDL_Rect &selection) noexcept {
  update(point, selection);

  ImVec2 delta = ImGui::GetMouseDragDelta();

  if (delta.x == 0.f && delta.y == 0.f) {
    // No move, cancel selection
    selection.w = 0;
    this->rect.w = 0;
  } else {
    selection = rect;
  }
}

void SelectMarkCommand::discard(SDL_Rect &selection) noexcept { rect.w = 0; }

void SelectMarkCommand::render(SDL_Texture *preview, SDL_Texture *overlay,
                               SDL_Rect &selection) noexcept {
  SDL_Renderer *renderer = App::get_renderer();

  if (this->rect.w == 0) {
    return;
  }

  SDL_SetRenderTarget(renderer, overlay);

  SDL_FRect rect = {this->rect.x, this->rect.y, this->rect.w, this->rect.h};

  SDL_SetRenderDrawColorFloat(renderer, 0.f, 0.f, 0.f, 1.f);
  SDL_RenderRect(renderer, &rect);
  SDL_SetRenderTarget(renderer, NULL);
}

std::unique_ptr<MouseCommand> SelectMarkCommand::next() noexcept {
  return std::make_unique<SelectMarkCommand>(*this);
}

std::unique_ptr<MouseCommand> SelectMarkCommand::transform() noexcept {
  return std::make_unique<SelectMoveCommand>();
}

} // namespace chroma
