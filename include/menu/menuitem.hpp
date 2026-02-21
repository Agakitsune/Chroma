/**
 * @file menuitem.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

namespace chroma {

class MenuItem {
public:
  MenuItem() noexcept = default;
  virtual ~MenuItem() noexcept = default;

  virtual void menubar() noexcept = 0;
  virtual void action() noexcept {};
  virtual void display() noexcept {}; // Not everything has to display something
  virtual void shortcuts() noexcept {};
};
} // namespace chroma
