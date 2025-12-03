/*
 * This example code creates an SDL window and renderer, and then clears the
 * window to a different color every frame, so you'll effectively get a window
 * that's smoothly fading between colors.
 *
 * This code is public domain. Feel free to use it for any purpose!
*/

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "SDL3_image/SDL_image.h"
#include <iostream>
 
/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static float scale = 1.0f;
static int texture_width = 0;
static int texture_height = 0;

// std::array_tag <SDL_FRect> [];


#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
 
/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_Surface *surface = NULL;
    char *picture_path = NULL;

    SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/clear", 1280, 960, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    /*This code section load a picture*/
    // SDL_asprintf(&picture_path, "/home/lucas/Chroma/build/kermit.png", SDL_GetBasePath());
    surface = IMG_Load("/home/lucas/Chroma/asset/kermit.png");
    if (!surface) {
        SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_free(picture_path);  /* done with this, the file is loaded. */
    texture_width = surface->w;
    texture_height = surface->h;
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_DestroySurface(surface);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}
 
/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        float mouse_x = 0;
        float mouse_y = 0;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        std::cout << "x: " << mouse_x << " y:" << mouse_y << std::endl;
        
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}
 
/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    float mouse_x = 0;
    float mouse_y = 0;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    SDL_FRect dst_rect;
    dst_rect.w = (float)texture_width * 0.2f;
    dst_rect.h = (float)texture_height * 0.2f;
    dst_rect.x = mouse_x - dst_rect.w/2;
    dst_rect.y = mouse_y - dst_rect.h/2;
    // std::cout << "x: " << mouse_x << " y:" << mouse_y << std::endl;
    // std::cout << "dst_rect.x: " << dst_rect.x << " dst_rect.y:" << dst_rect.y << std::endl;
    
    const double now = ((double)SDL_GetTicks()) / 1000.0;  /* convert from milliseconds to seconds. */

    /* choose the color for the frame we will draw. The sine wave trick makes it fade between colors smoothly. */
    const float red = (float) (0.5 + 0.5 * SDL_sin(now));
    const float green = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
    const float blue = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));
    SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);  /* new color, full alpha. */

    /* clear the window to the draw color. */
    SDL_RenderClear(renderer);

    // dst_rect.x = (WINDOW_WIDTH  - dst_rect.w) / 2.0f;
    // dst_rect.y = (WINDOW_HEIGHT - dst_rect.h) / 2.0f;
    SDL_RenderTexture(renderer, texture, NULL, &dst_rect);
    /* put the newly-cleared rendering on the screen. */
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}
 
/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}
