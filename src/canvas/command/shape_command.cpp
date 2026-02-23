
#include "SDL3/SDL.h"
#include "SDL3/SDL_gpu.h"

#include "canvas/canvas.hpp"
#include "canvas/command/shape_command.hpp"

#include "app.hpp"

#include <cmath>
#include <cstring>

namespace chroma {

ShapeCommand::ShapeCommand() noexcept {}

ShapeCommand::~ShapeCommand() noexcept { delete[] previous_data; }

void ShapeCommand::redo(const Canvas &canvas) noexcept {
  // const Layer &layer = canvas.layers[canvas.layer];
  const uint64_t stride = rect.w * 4;
  const uint64_t buffer_size = rect.h * stride;
  const uint64_t skip = (rect.x + rect.y * canvas.width) * 4;

  if (!previous_data) {
    previous_data = new uint8_t[buffer_size];
  }
  uint8_t *mapping = (uint8_t *)surface->pixels;

  mapping += skip;

  for (uint32_t i = 0; i < rect.h; i++) {
    std::memcpy(&previous_data[i * stride], mapping, stride);
    mapping += surface->pitch;
  }

  SDL_FillSurfaceRect(surface, &this->rect, main.to_u32());
  SDL_UpdateTexture(texture, &this->rect, (uint8_t *)surface->pixels + skip,
                    surface->pitch);
}

void ShapeCommand::undo(const Canvas &canvas) noexcept {
  // const Layer &layer = canvas.layers[canvas.layer];
  const uint64_t stride = rect.w * 4;
  const uint64_t buffer_size = rect.h * stride;
  const uint64_t skip = (rect.x + rect.y * canvas.width) * 4;

  uint8_t *mapping = (uint8_t *)surface->pixels;

  mapping += skip;

  for (uint32_t i = 0; i < rect.h; i++) {
    std::memcpy(mapping, &previous_data[i * stride], stride);
    mapping += surface->pitch;
  }

  SDL_UpdateTexture(texture, &this->rect, (uint8_t *)surface->pixels + skip,
                    surface->pitch);
}

void ShapeCommand::start(const Canvas &canvas, const SDL_Point &p,
                         const Color &color) noexcept {
  s = p;
  rect.w = 0;
}

void ShapeCommand::update(const Canvas &canvas, const SDL_Point &p,
                          const Color &color) noexcept {
  e = p;
  SDL_Point min = SDL_Point{std::min(s.x, e.x), std::min(s.y, e.y)};
  SDL_Point max = SDL_Point{std::max(s.x, e.x), std::max(s.y, e.y)};
  this->rect = SDL_Rect{min.x, min.y, max.x - min.x + 1, max.y - min.y + 1};
}

void ShapeCommand::end(const Canvas &canvas, const SDL_Point &p,
                       const Color &color) noexcept {
  update(canvas, p, color);
}

void ShapeCommand::discard(const Canvas &canvas) noexcept { rect.w = 0; }

void ShapeCommand::preview(const Canvas &canvas) noexcept {
  SDL_Renderer *renderer = App::get_renderer();

  // SDL_SetRenderTarget(renderer, canvas.preview);
  // SDL_SetRenderDrawColorFloat(renderer, 0.0, 0.0, 0.0, 0.0);
  // SDL_RenderClear(renderer);

  if (this->rect.w == 0) {
    // SDL_SetRenderTarget(renderer, NULL);
    return;
  }

  // SDL_SetRenderTarget(renderer, canvas.preview);

  // SDL_FRect rect = {this->rect.x, this->rect.y, this->rect.w,
  //                   this->rect.h};

  // SDL_SetRenderDrawColorFloat(renderer, main.r, main.g, main.b, main.a);
  // SDL_RenderRect(renderer, &rect);
  // SDL_SetRenderTarget(renderer, NULL);
}

} // namespace chroma
