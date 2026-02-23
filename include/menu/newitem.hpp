/**
 * @file newitem.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "menu/menuitem.hpp"

#include <cstdint>

namespace chroma {

/**
 * @brief New Menu item
 *
 */
class NewMenuItem : public MenuItem {
  uint32_t w;
  uint32_t h;

public:
  NewMenuItem() noexcept = default;
  ~NewMenuItem() noexcept = default;

  virtual void menubar() noexcept override final;
  virtual void action() noexcept override final;
  virtual void display() noexcept override final;
  virtual void shortcuts() noexcept override final;
};
} // namespace chroma
