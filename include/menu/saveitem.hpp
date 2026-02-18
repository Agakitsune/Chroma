
#pragma once

#include "menu/fileformat.hpp"
#include "menu/menuitem.hpp"

#include <filesystem>
#include <vector>

namespace chroma {

    class SaveMenuItem : public MenuItem {
        char *name;
        char *directory;

        std::filesystem::path home;
        std::filesystem::path current;

        std::vector<std::filesystem::path> directories;
        std::vector<std::filesystem::path> files;

        uint32_t selected = 0;

        bool hidden = false;

      public:
        SaveMenuItem() noexcept;
        virtual ~SaveMenuItem() noexcept override;

        void query_current_directory() noexcept;

        virtual void menubar() noexcept override final;
        virtual void action() noexcept override final;
        virtual void display() noexcept override final;
        virtual void shortcuts() noexcept override final;
    };
} // namespace chroma
