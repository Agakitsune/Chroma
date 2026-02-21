/**
 * @brief Handling whole window logic of Chroma
 *
 */

namespace chroma {
/**
 * @brief Construct a new Window:: Window object
 *
 * @param label
 * @param flags
 */
Window::Window(const std::string &label, ImGuiWindowFlags flags) noexcept
    : label(label), flags(flags) {}
} // namespace chroma
