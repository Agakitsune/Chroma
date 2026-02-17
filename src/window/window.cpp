/**
 * @brief Handling whole window logic of Chroma
 * 
 */

namespace chroma {

    Window::Window(const std::string &label, ImGuiWindowFlags flags) noexcept
    : label(label), flags(flags)
    {
    }
}
