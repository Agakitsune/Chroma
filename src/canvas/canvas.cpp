
#include "canvas/canvas.hpp"

#include "app.hpp"
#include "canvas/canvas.hpp"

#include <cstring>

#define TILE_SIZE 32

namespace chroma {

    Layer::~Layer() noexcept
    {
        SDL_GPUDevice *device = App::get_device();

        if (buffer) {
            SDL_ReleaseGPUTransferBuffer(device, buffer);
        }
        if (texture) {
            SDL_ReleaseGPUTexture(device, texture);
        }
        if (data) {
            delete[] data;
        }
        if (dirty_flags) {
            delete[] dirty_flags;
        }
    }

    Layer::Layer(Layer&&other) noexcept
    : data(other.data),
    dirty_flags(other.dirty_flags),
    buffer(other.buffer),
    texture(other.texture)
    {
        other.data = nullptr;
        other.dirty_flags = nullptr;
        other.buffer = nullptr;
        other.texture = nullptr;
    }

    Layer& Layer::operator=(Layer&&other) noexcept
    {
        if (this != &other) {
            data = other.data;
            dirty_flags = other.dirty_flags;
            buffer = other.buffer;
            texture = other.texture;

            other.data = nullptr;
            other.dirty_flags = nullptr;
            other.buffer = nullptr;
            other.texture = nullptr;
        }
        return *this;
    }

    Canvas::Canvas(uint32_t width, uint32_t height) noexcept
    : width(width), height(height)
    {
        SDL_GPUDevice *device = App::get_device();

        const uint64_t buffer_size = (width * height) * 4; // RGBA8
        const uint64_t tile_x = (width / TILE_SIZE) + ((width & (TILE_SIZE - 1)) ? 1 : 0);
        const uint64_t tile_y = (height / TILE_SIZE) + ((height & (TILE_SIZE - 1)) ? 1 : 0);
        const uint64_t dirty_flags_size = tile_x * tile_y;

        SDL_GPUTextureCreateInfo texture_info = {};
        texture_info.type = SDL_GPU_TEXTURETYPE_2D;
        texture_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        texture_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
        texture_info.width = width;
        texture_info.height = height;
        texture_info.layer_count_or_depth = 1;
        texture_info.num_levels = 1;
        texture_info.sample_count = SDL_GPU_SAMPLECOUNT_1;
        texture_info.props = 0;

        SDL_GPUTransferBufferCreateInfo buffer_info = {};
        buffer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        buffer_info.size = buffer_size;
        buffer_info.props = 0;

        Layer &layer = layers.emplace_back();
        TileTransfer transfer;

        layer.data = new uint8_t[buffer_size];
        layer.dirty_flags = new bool[dirty_flags_size];

        for (uint64_t i = 0; i < buffer_size; ++i) {
            layer.data[i] = 0;
        }

        transfer.index = 0;
        transfer.w = width;
        transfer.h = height;
        transfer.x = 0;
        transfer.y = 0;

        pending_uploads.push_back(transfer);

        // for (uint64_t i = 0; i < dirty_flags_size; ++i) {
        //     layer.dirty_flags[i] = true; // Upload everything at the end of the frame
        // }

        layer.texture = SDL_CreateGPUTexture(
            device,
            &texture_info
        );

        layer.buffer = SDL_CreateGPUTransferBuffer(
            device,
            &buffer_info
        );

        texture_info.usage |= SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;

        preview = SDL_CreateGPUTexture(
            device,
            &texture_info
        );
    }

    Canvas::~Canvas() noexcept
    {
        SDL_GPUDevice *device = App::get_device();

        if (preview) {
            SDL_ReleaseGPUTexture(device, preview);
        }
    }

    Canvas::Canvas(Canvas&&other) noexcept
    : name(std::move(other.name)),
    layers(std::move(other.layers)),
    preview(other.preview),
    stack(std::move(other.stack)),
    pending(std::move(other.pending)),
    width(other.width),
    height(other.height),
    stack_index(other.stack_index),
    layer(other.layer),
    offset(other.offset),
    zoom(other.zoom),
    dirty(other.dirty)
    {
        other.preview = nullptr;
    }

    Canvas& Canvas::operator=(Canvas&&other) noexcept
    {
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

    Color Canvas::get_color(uint32_t x, uint32_t y) const noexcept
    {
        const Layer &layer = layers[this->layer];

        const uint64_t div_x = x / TILE_SIZE;
        const uint64_t div_y = y / TILE_SIZE;
        const uint64_t mod_x = x & (TILE_SIZE - 1);
        const uint64_t mod_y = y & (TILE_SIZE - 1);

        const uint64_t div_width = width / TILE_SIZE;
        const uint64_t div_height = height / TILE_SIZE;
        const uint64_t mod_width = width & (TILE_SIZE - 1);
        const uint64_t mod_height = height & (TILE_SIZE - 1);

        uint64_t index = 0;
        const uint8_t *base = nullptr;

        Color ret;

        if (div_x < div_width && div_y < div_height) {
            // Full 64x64 tile
            index = (div_x * TILE_SIZE) + (div_y * (div_width * TILE_SIZE));

            base = layer.data + (index * 4);
            ret.download(base + (((mod_y * TILE_SIZE) + mod_x) * 4));
        } else {
            index = (div_width * TILE_SIZE) + (div_height * TILE_SIZE); // Skip all full tiles

            if (div_y == div_height) {
                // Skip last partial row
                index += div_height * (mod_width * TILE_SIZE);

                index += div_x * (mod_height * TILE_SIZE);
            } else {
                index += div_y * (mod_height * TILE_SIZE);
            }

            base = layer.data + (index * 4);

            if (div_x == div_width) {
                ret.download(base + (((mod_y * mod_width) + mod_x) * 4));
            } else {
                ret.download(base + (((mod_y * TILE_SIZE) + mod_x) * 4));
            }
        }

        return ret;
    }

    void Canvas::set_color(uint32_t x, uint32_t y, const Color &color) noexcept
    {
        const Layer &layer = layers[this->layer];

        const uint64_t div_x = x / TILE_SIZE;
        const uint64_t div_y = y / TILE_SIZE;
        const uint64_t mod_x = x & (TILE_SIZE - 1);
        const uint64_t mod_y = y & (TILE_SIZE - 1);

        const uint64_t div_width = width / TILE_SIZE;
        const uint64_t div_height = height / TILE_SIZE;
        const uint64_t mod_width = width & (TILE_SIZE - 1);
        const uint64_t mod_height = height & (TILE_SIZE - 1);

        // uint64_t index = 0;
        uint8_t *base = nullptr;

        bool new_transfer = true;

        TileTransfer transfer;

        transfer.x = div_x * TILE_SIZE;
        transfer.y = div_y * TILE_SIZE;

        // transfer.index = 

        if (div_x < div_width && div_y < div_height) {
            // Full 64x64 tile
            transfer.index = (div_x * TILE_SIZE * TILE_SIZE) + (div_y * (div_width * TILE_SIZE * TILE_SIZE));

            transfer.w = TILE_SIZE;
            transfer.h = TILE_SIZE;
            
            // color.upload(base + (((mod_y * TILE_SIZE) + mod_x) * 4));
        } else {
            transfer.index = (div_width * TILE_SIZE) * (div_height * TILE_SIZE); // Skip all full tiles
            // flag_index = div_width * div_height;

            if (div_y == div_height) {
                // Skip last partial row
                transfer.index += div_height * (mod_width * TILE_SIZE);

                transfer.index += div_x * (mod_height * TILE_SIZE);
                transfer.h = mod_height;
            } else {
                transfer.index += div_y * (mod_height * TILE_SIZE);
                transfer.h = TILE_SIZE;
            }

            // base = layer.data + (index * 4);
        }

        base = layer.data + (transfer.index * 4);

        if (div_x == div_width) {
            color.upload(base + (((mod_y * mod_width) + mod_x) * 4));
            transfer.w = mod_width;
        } else {
            color.upload(base + (((mod_y * TILE_SIZE) + mod_x) * 4));
            transfer.w = TILE_SIZE;
        }

        for (const TileTransfer &t : pending_uploads) {
            if (transfer.index == t.index) {
                new_transfer = false;
                break;
            }
        }

        if (new_transfer) {
            pending_uploads.push_back(transfer);
        }

        // layer.dirty_flags[flag_index] = true;
    }

    void Canvas::add_command(std::unique_ptr<ICommand> &&cmd) noexcept
    {
        // Remove undone commands
        const uint64_t size = stack.size();
        const uint64_t remove = size - stack_index;

        stack.erase(stack.end() - remove, stack.end());

        pending.push(std::move(cmd));
    }

    void Canvas::execute_pending() noexcept
    {
        while (!pending.empty()) {
            std::unique_ptr<ICommand> &cmd = pending.front();

            cmd->redo(*this);

            stack.push_back(std::move(cmd));
            ++stack_index;

            pending.pop();
        }
    }

    void Canvas::undo() noexcept
    {
        if (stack_index == 0) {
            return;
        }

        ICommand &cmd = *stack[stack_index - 1];
        cmd.undo(*this);
        --stack_index;
    }

    void Canvas::redo() noexcept
    {
        if (stack_index >= stack.size()) {
            return;
        }

        ICommand &cmd = *stack[stack_index];
        cmd.redo(*this);
        ++stack_index;
    }

    void Canvas::upload(SDL_GPUCopyPass *pass) noexcept
    {
        SDL_GPUDevice *device = App::get_device();

        const uint64_t div_width = width / TILE_SIZE;
        const uint64_t div_height = height / TILE_SIZE;
        const uint64_t mod_width = width & (TILE_SIZE - 1);
        const uint64_t mod_height = height & (TILE_SIZE - 1);

        uint64_t index = 0;

        void *mapping = nullptr;

        SDL_GPUTextureTransferInfo source = {};
        SDL_GPUTextureRegion destination = {};

        destination.mip_level = 0;
        destination.layer = 0;
        destination.z = 0;
        destination.d = 1;

        const Layer &layer = layers[this->layer];

        source.transfer_buffer = layer.buffer;
        destination.texture = layer.texture;

        mapping = SDL_MapGPUTransferBuffer(
            device,
            layer.buffer,
            true
        );

        for (const TileTransfer &transfer : pending_uploads) {
            const uint32_t s = transfer.index * 4;
            const uint8_t *base = layer.data + s;
            uint8_t *dst = (uint8_t*)mapping + s;

            source.offset = s;
            source.pixels_per_row = transfer.w;
            source.rows_per_layer = transfer.h;

            destination.w = transfer.w;
            destination.h = transfer.h;
            destination.x = transfer.x;
            destination.y = transfer.y;

            std::memcpy(
                dst, base, transfer.w * transfer.h * 4
            );

            SDL_UploadToGPUTexture(
                pass,
                &source,
                &destination,
                false
            );
        }

        SDL_UnmapGPUTransferBuffer(
            device,
            layer.buffer
        );

        pending_uploads.clear();

        // for (Layer &layer : layers) {
        //     mapping = SDL_MapGPUTransferBuffer(
        //         device,
        //         layer.buffer,
        //         true
        //     );

        //     source.transfer_buffer = layer.buffer;
        //     destination.texture = layer.texture;

        //     for (uint64_t y = 0; y < div_height; ++y) {
        //         for (uint64_t x = 0; x < div_width; ++x) {
        //             const uint64_t flag_index = x + y * div_width;

        //             if (layer.dirty_flags[flag_index]) {
        //                 layer.dirty_flags[flag_index] = false;

        //                 index = (x * TILE_SIZE) + (y * (div_width * TILE_SIZE));
        //                 const uint8_t *base = layer.data + (index * 4);

        //                 uint8_t *dst = (uint8_t*)mapping + (index * 4);
        //                 std::memcpy(
        //                     dst,
        //                     base,
        //                     TILE_SIZE * TILE_SIZE * 4
        //                 );

        //                 source.offset = index * 4;
        //                 source.pixels_per_row = TILE_SIZE;
        //                 source.rows_per_layer = TILE_SIZE;

        //                 destination.w = TILE_SIZE;
        //                 destination.h = TILE_SIZE;
        //                 destination.x = x * TILE_SIZE;
        //                 destination.y = y * TILE_SIZE;

        //                 SDL_UploadToGPUTexture(
        //                     pass,
        //                     &source,
        //                     &destination,
        //                     true
        //                 );
        //             }
        //         }
        //     }

        //     for (uint64_t y = 0; y < div_height; ++y) {
        //         const uint64_t flag_index = div_width * div_height + y;

        //         if (layer.dirty_flags[flag_index]) {
        //             layer.dirty_flags[flag_index] = false;

        //             index = (div_width * TILE_SIZE) * (div_height * TILE_SIZE); // Skip all full tiles
        //             index += y * (mod_width * TILE_SIZE);

        //             const uint8_t *base = layer.data + (index * 4);

        //             uint8_t *dst = (uint8_t*)mapping + (index * 4);
        //             std::memcpy(
        //                 dst,
        //                 base,
        //                 mod_width * TILE_SIZE * 4
        //             );

        //             source.offset = index * 4;
        //             source.pixels_per_row = mod_width;
        //             source.rows_per_layer = TILE_SIZE;

        //             destination.w = mod_width;
        //             destination.h = TILE_SIZE;
        //             destination.x = div_width * TILE_SIZE;
        //             destination.y = y * TILE_SIZE;

        //             SDL_UploadToGPUTexture(
        //                 pass,
        //                 &source,
        //                 &destination,
        //                 true
        //             );
        //         }
        //     }

        //     for (uint64_t x = 0; x < div_width; ++x) {
        //         const uint64_t flag_index = x + div_height + div_height * div_width;

        //         if (layer.dirty_flags[flag_index]) {
        //             layer.dirty_flags[flag_index] = false;

        //             index = (div_width * TILE_SIZE) * (div_height * TILE_SIZE); // Skip all full tiles
        //             index += div_height * (mod_width * TILE_SIZE);
        //             index += x * (mod_height * TILE_SIZE);

        //             const uint8_t *base = layer.data + (index * 4);

        //             uint8_t *dst = (uint8_t*)mapping + (index * 4);
        //             std::memcpy(
        //                 dst,
        //                 base,
        //                 TILE_SIZE * mod_height * 4
        //             );

        //             source.offset = index * 4;
        //             source.pixels_per_row = TILE_SIZE;
        //             source.rows_per_layer = mod_height;

        //             destination.w = TILE_SIZE;
        //             destination.h = mod_height;
        //             destination.x = x * TILE_SIZE;
        //             destination.y = div_height * TILE_SIZE;

        //             SDL_UploadToGPUTexture(
        //                 pass,
        //                 &source,
        //                 &destination,
        //                 true
        //             );
        //         }
        //     }

        //     const uint64_t flag_index = div_width * div_height + div_height + div_width;

        //     if (layer.dirty_flags[flag_index]) {
        //         layer.dirty_flags[flag_index] = false;

        //         index = (div_width * TILE_SIZE) * (div_height * TILE_SIZE); // Skip all full tiles
        //         index += div_width * (mod_height * TILE_SIZE);
        //         index += div_height * (mod_width * TILE_SIZE);

        //         const uint8_t *base = layer.data + (index * 4);

        //         uint8_t *dst = (uint8_t*)mapping + (index * 4);
        //         std::memcpy(
        //             dst,
        //             base,
        //             mod_width * mod_height * 4
        //         );

        //         source.offset = index * 4;
        //         source.pixels_per_row = mod_width;
        //         source.rows_per_layer = mod_height;

        //         destination.w = mod_width;
        //         destination.h = mod_height;
        //         destination.x = div_width * TILE_SIZE;
        //         destination.y = div_height* TILE_SIZE;

        //         SDL_UploadToGPUTexture(
        //             pass,
        //             &source,
        //             &destination,
        //             true
        //         );
        //     }

        //     SDL_UnmapGPUTransferBuffer(
        //         device,
        //         layer.buffer
        //     );
        // }

    }

    void Canvas::refresh() noexcept
    {
        TileTransfer transfer;

        transfer.index = 0;
        transfer.w = width;
        transfer.h = height;
        transfer.x = 0;
        transfer.y = 0;

        pending_uploads.push_back(transfer);
    }

    // void Canvas::upload(SDL_GPUCopyPass *pass) noexcept {
    //     if (pending_uploads.empty()) return;

    //     for (const auto& transfer : pending_uploads) {
    //         SDL_GPUTextureTransferInfo transferInfo{};
    //         transferInfo.transfer_buffer = layers[transfer.index].buffer; // Assuming Layer holds the buffer
    //         transferInfo.offset = 0; // Or calculate based on tile logic

    //         SDL_GPUTextureRegion region{};
    //         region.texture = layers[transfer.index].texture;
    //         region.w = transfer.w;
    //         region.h = transfer.h;
    //         region.x = transfer.x;
    //         region.y = transfer.y;

    //         SDL_UploadToGPUTexture(pass, &transferInfo, &region, false);
    //     }
    //     pending_uploads.clear(); 
    // }


}
