/**
 * @file openitem.hpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "menu/fileformat.hpp"
#include "menu/menuitem.hpp"

#include <filesystem>
#include <vector>

namespace chroma {

/**
 * @brief Open Menu item
 *
 */
class OpenMenuItem : public MenuItem {
  char *name;
  char *directory;

  std::filesystem::path home;
  std::filesystem::path current;

  std::vector<std::filesystem::path> directories;
  std::vector<std::filesystem::path> files;

  uint32_t selected = 0;

  bool hidden = false;

public:
  OpenMenuItem() noexcept;
  ~OpenMenuItem() noexcept;

  void query_current_directory() noexcept;

  virtual void menubar() noexcept override final;
  virtual void action() noexcept override final;
  virtual void display() noexcept override final;
  virtual void shortcuts() noexcept override final;
};
} // namespace chroma
