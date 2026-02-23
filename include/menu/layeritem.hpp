
#pragma once

#include "menu/menuitem.hpp"

#include <cstdint>

namespace chroma {

class LayerMenuItem : public MenuItem {
public:
  LayerMenuItem() noexcept = default;
  virtual ~LayerMenuItem() noexcept override = default;

  virtual void menubar() noexcept override final;
  virtual void shortcuts() noexcept override final;
};
} // namespace chroma
