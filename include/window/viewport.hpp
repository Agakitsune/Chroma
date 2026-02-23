/**
 * @file viewport.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "window.hpp"

#include "imgui.h"

#include "SDL3/SDL.h"

#include "canvas/canvas.hpp"
#include "menu/fileformat.hpp"

#include "canvas/cmd/mouse_command.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace chroma {

/**
 * @brief Viewport window
 *
 */
class ViewportWindow : public Window {
    std::vector<Canvas> canvases;
    uint32_t marked = 0;
    uint32_t selected = 0;

    bool dragging = false;
    bool brushing = false;
    bool discarded = false;
    bool on_select = false;

    std::unique_ptr<MouseCommand> cmd;

    SDL_Rect selection = { 0, 0, 0, 0 };
    // uint8_t *select_data = nullptr;
    // SDL_Texture *select_texture = nullptr;

    // SDL_GPUTransferBuffer *transfer_buffer = nullptr;
    // SDL_GPUBuffer *uniform_buffer = nullptr;

    SDL_Texture* preview = nullptr;
    SDL_Texture* overlay = nullptr;

public:
    ViewportWindow() noexcept;
    virtual ~ViewportWindow() noexcept = default;

    virtual void ready() noexcept override final;
    virtual void display() noexcept override final;

private:
    void new_canvas(uint32_t width, uint32_t height) noexcept;
    void save_canvas(const std::filesystem::path& directory,
        const std::filesystem::path& file,
        FileFormat format) noexcept;
    void open_canvas(const std::filesystem::path& directory,
        const std::filesystem::path& file,
        FileFormat format) noexcept;
    bool is_empty() const noexcept;

    void fliph() noexcept;
    void flipv() noexcept;

    void add_layer() noexcept;
    void delete_layer() noexcept;

    void select_all() noexcept;

    void undo() noexcept;
    void redo() noexcept;

    void reload() noexcept;

    Canvas& get_canvas() noexcept;

private:
    void _on_main_color_changed(const Color& clr) noexcept;
    void _on_second_color_changed(const Color& clr) noexcept;
};
} // namespace chroma
