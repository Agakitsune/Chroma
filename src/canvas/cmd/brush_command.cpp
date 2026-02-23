
#include "canvas/cmd/brush_command.hpp"
#include "canvas/canvas.hpp"

#include "SDL3/SDL.h"

#include "app.hpp"

namespace chroma {

void BrushCommand::redo(SDL_Rect &selection) noexcept {
  uint8_t *mapping = (uint8_t *)surface->pixels;

  for (const SDL_FPoint &p : points) {
    int x = p.x;
    int y = p.y;

    main.upload(mapping + (x + y * surface->w) * 4);
  }

        if (selection.w > 0) {
            mapping += selection.x * 4 + selection.y * surface->pitch;
            SDL_UpdateTexture(texture, &selection, mapping, surface->pitch);
        } else {
            SDL_UpdateTexture(texture, nullptr, mapping, surface->pitch);
        }
    }

void BrushCommand::undo(SDL_Rect &selection) noexcept {
  uint8_t *mapping = (uint8_t *)surface->pixels;

  for (uint32_t i = 0; i < points.size(); i++) {
    const SDL_FPoint &p = points[i];
    int x = p.x;
    int y = p.y;

    old[i].upload(((uint8_t *)surface->pixels) + (x + y * surface->w) * 4);
  }

        if (selection.w > 0) {
            mapping += selection.x * 4 + selection.y * surface->pitch;
            SDL_UpdateTexture(texture, &selection, mapping, surface->pitch);
        } else {
            SDL_UpdateTexture(texture, nullptr, mapping, surface->pitch);
        }
    }

void BrushCommand::start(const SDL_Point &point, SDL_Rect &selection) noexcept {
  SDL_FPoint p{(float)point.x, (float)point.y};
  Color c;
  uint8_t *pix = (uint8_t *)surface->pixels;

  if (selection.w > 0 && !SDL_PointInRect(&point, &selection)) {
    return;
  }

  points.push_back(p);

  c.download(&pix[point.x * 4 + point.y * surface->pitch]);

  old.push_back(c);
}

void BrushCommand::update(const SDL_Point &point,
                          SDL_Rect &selection) noexcept {
  SDL_FPoint p{(float)point.x, (float)point.y};
  Color c;
  uint8_t *pix = (uint8_t *)surface->pixels;

  if (selection.w > 0 && !SDL_PointInRect(&point, &selection)) {
    return;
  }

  for (const SDL_FPoint &pt : points) {
    if (pt.x == p.x && pt.y == p.y) {
      return;
    }
  }

  points.push_back(p);

  c.download(&pix[point.x * 4 + point.y * surface->pitch]);

  old.push_back(c);
}

void BrushCommand::end(const SDL_Point &point, SDL_Rect &selection) noexcept {
  update(point, selection);
}

void BrushCommand::discard(SDL_Rect &selection) noexcept {
  points.clear();
  old.clear();
}

void BrushCommand::render(SDL_Texture *preview, SDL_Texture *overlay,
                          SDL_Rect &selection) noexcept {
  SDL_Renderer *renderer = App::get_renderer();

  if (points.empty()) {
    return;
  }

  SDL_SetRenderTarget(renderer, preview);

  SDL_SetRenderDrawColorFloat(renderer, main.r, main.g, main.b, main.a);
  SDL_RenderPoints(renderer, points.data(), points.size());

  SDL_SetRenderTarget(renderer, NULL);
}

std::unique_ptr<MouseCommand> BrushCommand::next() noexcept {
  return std::make_unique<BrushCommand>(*this);
}

} // namespace chroma
