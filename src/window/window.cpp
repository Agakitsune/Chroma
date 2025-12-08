#include "window/window.hpp"

namespace chroma {

    Window::Window(const std::string &label, ImGuiWindowFlags flags)
    : label(label), flags(flags)
    {
    }

    bool Window::begin()
    {
        return ImGui::Begin(label.c_str(), nullptr, flags);
    }

    bool Window::render()
    {
        return false;
    }

    void Window::end()
    {
        ImGui::End();
    }
}
