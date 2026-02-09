
#include "app.hpp"

#include "SDL3_image/SDL_image.h"

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
