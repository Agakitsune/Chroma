/**
 * @file saveitem.hpp
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
#include "menu/fileformat.hpp"

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
            ~SaveMenuItem() noexcept;

            void query_current_directory() noexcept;

            virtual void menubar() noexcept override final;
            virtual void action() noexcept override final;
            virtual void display() noexcept override final;
            virtual void shortcuts() noexcept override final;
    };
}
