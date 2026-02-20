
#include "canvas/canvas.hpp"

#include "app.hpp"
#include "canvas/canvas.hpp"

#include <cstring>

#define TILE_SIZE 32

namespace chroma {

    Layer::~Layer() noexcept {
        SDL_GPUDevice *device = App::get_device();

        if (texture) {
            SDL_DestroyTexture(texture);
        }
        if (surface) {
            SDL_DestroySurface(surface);
        }
    }

    Layer::Layer(Layer &&other) noexcept
        : texture(other.texture), surface(other.surface), name(other.name) {
        other.texture = nullptr;
        other.surface = nullptr;
        other.name = "";
    }

    Layer &Layer::operator=(Layer &&other) noexcept {
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
        : width(width), height(height) {
        SDL_Renderer *renderer = App::get_renderer();

        Layer &layer = layers.emplace_back();
        layer.name = "Layer 1";

        layer.surface = SDL_CreateSurface(
            width,
            height,
            SDL_PIXELFORMAT_RGBA32
        );

        layer.texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height
        );

        this->preview = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_TARGET,
            width,
            height
        );

        SDL_SetTextureScaleMode(layer.texture, SDL_SCALEMODE_NEAREST);
        SDL_SetTextureScaleMode(this->preview, SDL_SCALEMODE_NEAREST);

        SDL_FillSurfaceRect(
            layer.surface,
            NULL,
            0
        );
        SDL_UpdateTexture(
            layer.texture,
            NULL,
            layer.surface->pixels,
            layer.surface->pitch
        );

        SDL_SetRenderTarget(renderer, this->preview);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, NULL);
    }

    Canvas::Canvas(SDL_Surface *surface) noexcept
        : width(surface->w), height(surface->h) {
        SDL_Renderer *renderer = App::get_renderer();

        Layer &layer = layers.emplace_back();
        layer.name = "Layer 1";

        layer.surface = surface;

        layer.texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height
        );

        this->preview = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_TARGET,
            width,
            height
        );

        SDL_SetTextureScaleMode(layer.texture, SDL_SCALEMODE_NEAREST);
        SDL_SetTextureScaleMode(this->preview, SDL_SCALEMODE_NEAREST);

        SDL_UpdateTexture(
            layer.texture,
            NULL,
            layer.surface->pixels,
            layer.surface->pitch
        );

        SDL_SetRenderTarget(renderer, this->preview);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, NULL);
    }

    Canvas::~Canvas() noexcept {
        if (preview) {
            SDL_DestroyTexture(this->preview);
        }
        if (this->overlay) {
            SDL_DestroyTexture(this->overlay);
        }
    }

    Canvas::Canvas(Canvas &&other) noexcept
        : name(std::move(other.name)), layers(std::move(other.layers)),
          preview(other.preview), stack(std::move(other.stack)),
          pending(std::move(other.pending)), width(other.width),
          height(other.height), stack_index(other.stack_index),
          layer(other.layer), offset(other.offset), zoom(other.zoom),
          dirty(other.dirty) {
        other.preview = nullptr;
    }

    Canvas &Canvas::operator=(Canvas &&other) noexcept {
        if (this != &other) {
            width = other.width;
            height = other.height;
            name = std::move(other.name);
            stack = std::move(other.stack);
            pending = std::move(other.pending);
            layers = std::move(other.layers);
            preview = other.preview;
            stack_index = other.stack_index;
            layer = other.layer;
            offset = other.offset;
            zoom = other.zoom;
            dirty = other.dirty;

            other.preview = nullptr;
        }
        return *this;
    }

    Color Canvas::get_color(uint32_t x, uint32_t y) const noexcept {
        const Layer &layer = layers[this->layer];

        uint8_t *mapping = (uint8_t*)layer.surface->pixels;

        Color ret;

        mapping += (x + y * width) * 4;
        ret.download(mapping);

        return ret;
    }

    void Canvas::add_command(std::unique_ptr<ICommand> &&cmd) noexcept {
        // Remove undone commands
        const uint64_t size = stack.size();
        const uint64_t remove = size - stack_index;

        stack.erase(stack.end() - remove, stack.end());

        pending.push(std::move(cmd));
    }

    void Canvas::execute_pending() noexcept {
        while (!pending.empty()) {
            std::unique_ptr<ICommand> &cmd = pending.front();

            cmd->redo(*this);

            stack.push_back(std::move(cmd));
            ++stack_index;

            pending.pop();
        }
    }

    void Canvas::undo() noexcept {
        if (stack_index == 0) {
            return;
        }

        ICommand &cmd = *stack[stack_index - 1];
        cmd.undo(*this);
        --stack_index;
    }

    void Canvas::redo() noexcept {
        if (stack_index >= stack.size()) {
            return;
        }

        ICommand &cmd = *stack[stack_index];
        cmd.redo(*this);
        ++stack_index;
    }

    void Canvas::add_layer() noexcept {
        char l[64] = {0};
        SDL_Renderer *renderer = App::get_renderer();

        Layer &layer = layers.emplace_back();
        sprintf(l, "Layer %i", layers.size());

        layer.name = l;

        layer.surface = SDL_CreateSurface(
            width,
            height,
            SDL_PIXELFORMAT_RGBA32
        );

        layer.texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height
        );

        SDL_SetTextureScaleMode(layer.texture, SDL_SCALEMODE_NEAREST);

        SDL_FillSurfaceRect(
            layer.surface,
            NULL,
            0
        );
        SDL_UpdateTexture(
            layer.texture,
            NULL,
            layer.surface->pixels,
            layer.surface->pitch
        );
    }

} // namespace chroma
