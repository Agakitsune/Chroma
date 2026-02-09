
#pragma once

#include "menu/menuitem.hpp"

#include <filesystem>

namespace chroma {

    class SaveMenuItem : public MenuItem {
        char *name;
        char *directory;

        std::filesystem::path home;
        std::filesystem::path current;

        public:
            SaveMenuItem() noexcept;
            ~SaveMenuItem() noexcept;

            virtual void menubar() noexcept override final;
            virtual void display() noexcept override final;
    };
}
