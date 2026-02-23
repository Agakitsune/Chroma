/**
 * @file layeritem.hpp
 * @author Kitsune (https://github.com/Agakitsune)
 * @brief
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "menu/menuitem.hpp"

#include <cstdint>

namespace chroma {

/**
 * @brief Layer Menu item
 *
 */
class LayerMenuItem : public MenuItem {
public:
    LayerMenuItem() noexcept = default;
    virtual ~LayerMenuItem() noexcept override = default;

    virtual void menubar() noexcept override final;
    virtual void shortcuts() noexcept override final;
};
} // namespace chroma
