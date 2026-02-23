
#include "canvas/cmd/mouse_command.hpp"
#include "canvas/canvas.hpp"

namespace chroma {

void MouseCommand::set_main_color(const Color &color) noexcept { main = color; }

void MouseCommand::set_second_color(const Color &color) noexcept {
  second = color;
}

} // namespace chroma
