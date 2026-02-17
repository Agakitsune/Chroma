/**
 * @file main.cpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief 
 * @version 0.1
 * @date 2026-02-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "app.hpp"
#include "SDL3_image/SDL_image.h"
/**
 * @brief Main function of Chroma
 * 
 * @return int
 */
int main(int, char**) {
    chroma::App app;
    int err = 0;

    if ((err = app.init()) != 0) {
        return err;
    }
    if ((err = app.run()) != 0) {
        return err;
    }
    return 0;
}
