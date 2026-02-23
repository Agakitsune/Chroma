/**
 * @file canvas.cpp
 * @author Zeustygien (lucas.gangnant@epitech.eu)
 * @brief Handle canva logic and ability to draw over
 * @version 0.1
 * @date 2026-02-17
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "canvas/canvas.hpp"

#include "app.hpp"
#include "canvas/canvas.hpp"

#include <cstring>

#define TILE_SIZE 32

namespace chroma {

Layer::~Layer() noexcept
{
    if (texture) {
        SDL_DestroyTexture(texture);
    }
    if (surface) {
        SDL_DestroySurface(surface);
    }
}

Layer::Layer(Layer&& other) noexcept
    : texture(other.texture)
    , surface(other.surface)
    , name(other.name)
{
    other.texture = nullptr;
    other.surface = nullptr;
    other.name = "";
}

Layer& Layer::operator=(Layer&& other) noexcept
{
    if (this != &other) {
        texture = other.texture;
        surface = other.surface;
        name = other.name;

        other.texture = nullptr;
        other.surface = nullptr;
        other.name = "";
    }
    return *this;
}

Canvas::Canvas(uint32_t width, uint32_t height) noexcept
    : width(width)
    , height(height)
{
    SDL_Renderer* renderer = App::get_renderer();

    Layer& layer = layers.emplace_back();
    layer.name = "Layer 1";

    layer.surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);

    layer.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING, width, height);

    SDL_SetTextureScaleMode(layer.texture, SDL_SCALEMODE_NEAREST);

    SDL_FillSurfaceRect(layer.surface, nullptr, 0);
    SDL_UpdateTexture(layer.texture, nullptr, layer.surface->pixels,
        layer.surface->pitch);
}

Canvas::Canvas(SDL_Surface* surface) noexcept
    : width(surface->w)
    , height(surface->h)
{
    SDL_Renderer* renderer = App::get_renderer();

    Layer& layer = layers.emplace_back();
    layer.name = "Layer 1";

    layer.surface = surface;

    layer.texture = SDL_CreateTexture(renderer, surface->format, SDL_TEXTUREACCESS_STREAMING,
        surface->w, surface->h);

    SDL_SetTextureScaleMode(layer.texture, SDL_SCALEMODE_NEAREST);

    Color c;
    c.download((uint8_t*)layer.surface->pixels);

    if (SDL_UpdateTexture(layer.texture, nullptr, layer.surface->pixels,
            layer.surface->pitch))
        SDL_GetError();
}

Canvas::~Canvas() noexcept { }

Canvas::Canvas(Canvas&& other) noexcept
    : name(std::move(other.name))
    , layers(std::move(other.layers))
    , stack(std::move(other.stack))
    , pending(std::move(other.pending))
    , width(other.width)
    , height(other.height)
    , stack_index(other.stack_index)
    , layer(other.layer)
    , offset(other.offset)
    , zoom(other.zoom)
    , dirty(other.dirty)
{
}

Canvas& Canvas::operator=(Canvas&& other) noexcept
{
    if (this != &other) {
        width = other.width;
        height = other.height;
        name = std::move(other.name);
        stack = std::move(other.stack);
        pending = std::move(other.pending);
        layers = std::move(other.layers);
        stack_index = other.stack_index;
        layer = other.layer;
        offset = other.offset;
        zoom = other.zoom;
        dirty = other.dirty;
    }
    return *this;
}

Color Canvas::get_color(uint32_t x, uint32_t y) const noexcept
{
    const Layer& layer = layers[this->layer];

    uint8_t* mapping = (uint8_t*)layer.surface->pixels;

    Color ret;

    mapping += (x + y * width) * 4;
    ret.download(mapping);

    return ret;
}

void Canvas::add_command(std::unique_ptr<Command>&& cmd) noexcept
{
    // Remove undone commands
    const uint64_t size = stack.size();
    const uint64_t remove = size - stack_index;

    stack.erase(stack.end() - remove, stack.end());

    pending.push(std::move(cmd));
}

void Canvas::execute_pending(SDL_Rect& selection) noexcept
{
    while (!pending.empty()) {
        std::unique_ptr<Command>& cmd = pending.front();

        cmd->redo(selection);

        stack.push_back(std::move(cmd));
        ++stack_index;

        pending.pop();
    }
}

void Canvas::undo(SDL_Rect& selection) noexcept
{
    if (stack_index == 0) {
        return;
    }

    Command& cmd = *stack[stack_index - 1];
    cmd.undo(selection);
    --stack_index;
}

void Canvas::redo(SDL_Rect& selection) noexcept
{
    if (stack_index >= stack.size()) {
        return;
    }

    Command& cmd = *stack[stack_index];
    cmd.redo(selection);
    ++stack_index;
}

void Canvas::add_layer() noexcept
{
    char l[64] = { 0 };
    SDL_Renderer* renderer = App::get_renderer();

    Layer& layer = layers.emplace_back();
    sprintf(l, "Layer %i", layers.size());

    layer.name = l;

    layer.surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);

    layer.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING, width, height);

    SDL_SetTextureScaleMode(layer.texture, SDL_SCALEMODE_NEAREST);

    SDL_FillSurfaceRect(layer.surface, NULL, 0);
    SDL_UpdateTexture(layer.texture, NULL, layer.surface->pixels,
        layer.surface->pitch);
}

void Canvas::delete_layer() noexcept
{
    if (layers.size() == 1) {
        return;
    }

    layers.erase(layers.begin() + layer);

    if (layer > 0) {
        layer--;
    }
}

} // namespace chroma
