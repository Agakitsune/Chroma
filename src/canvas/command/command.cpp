
#include "canvas/command/command.hpp"

#include "canvas/canvas.hpp"

namespace chroma {

void ICommand::set_layer(const Layer &layer) noexcept {
  surface = layer.surface;
  texture = layer.texture;
}

} // namespace chroma
