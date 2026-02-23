
#include "canvas/cmd/command.hpp"
#include "canvas/canvas.hpp"

namespace chroma {

void Command::set_layer(const Layer &layer) noexcept {
  surface = layer.surface;
  texture = layer.texture;
}

} // namespace chroma
