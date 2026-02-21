/**
 * @file cursor.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "SDL3/SDL.h"

#include <memory>
#include <vector>

namespace chroma {

enum class Cursor {
  Default,

  Arrow = Default,
  Cross,
  Grab,
  Picker,
  Point,

  Count
};

class CursorManager {
  static std::unique_ptr<CursorManager> instance;

  std::vector<SDL_Cursor *> cursors;

  SDL_Cursor *last_cursor = nullptr;

  Cursor cursor = Cursor::Default;

  CursorManager() noexcept;

  static CursorManager &get_instance() noexcept;

public:
  ~CursorManager() noexcept;

  static void set_cursor(Cursor cursor) noexcept;
  static void update() noexcept;
};

} // namespace chroma
