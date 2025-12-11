#include "window/window.hpp"

namespace chroma {

    Window::Window(const std::string &label, ImGuiWindowFlags flags) noexcept
    : label(label), flags(flags)
    {
    }
}
