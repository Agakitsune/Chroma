
#pragma once

#include "window.hpp"

#include "imgui.h"

#include "SDL3/SDL.h"

#include "canvas/canvas.hpp"
#include "menu/fileformat.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace chroma {

class LayerWindow : public Window {
  Canvas *canvas = nullptr;
  uint32_t selected = 0;

  char *name;
  bool open = false;

public:
  LayerWindow() noexcept;
  virtual ~LayerWindow() noexcept;

  virtual void ready() noexcept override final;
  virtual void display() noexcept override final;

  // void new_canvas(uint32_t width, uint32_t height) noexcept;
  // void save_canvas(const std::filesystem::path &directory,
  //                  const std::filesystem::path &file,
  //                  FileFormat format) noexcept;
  // void open_canvas(const std::filesystem::path &directory,
  //                  const std::filesystem::path &file,
  //                  FileFormat format) noexcept;
  // bool is_empty() const noexcept;

  // void fliph() noexcept;
  // void flipv() noexcept;
  // void undo() noexcept;
  // void redo() noexcept;

  // Canvas &get_canvas() noexcept;

private:
  void _on_canvas_selected(Canvas *canvas) noexcept;
  void _on_layer_delete() noexcept;
  // void _on_main_color_changed(const Color &clr) noexcept;
  // void _on_second_color_changed(const Color &clr) noexcept;
};
} // namespace chroma
