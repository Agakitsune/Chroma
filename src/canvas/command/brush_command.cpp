
#include "canvas/command/brush_command.hpp"

#include "SDL3/SDL.h"
#include "SDL3/SDL_gpu.h"

#include "canvas/command/brush_command.hpp"
#include "canvas/canvas.hpp"

#include "app.hpp"

#include <cstring>

static const uint32_t spirv_vertex[] = {
    0x07230203,0x00010000,0x0008000b,0x0000003b,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0009000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000c,0x0000001a,0x00000029,
	0x00000036,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,0x00000000,
	0x00040005,0x00000009,0x74726576,0x00007865,0x00050005,0x0000000c,0x6f506e69,0x69746973,
	0x00006e6f,0x00060005,0x0000001a,0x565f6c67,0x65747265,0x646e4978,0x00007865,0x00050005,
	0x0000001d,0x65646e69,0x6c626178,0x00000065,0x00060005,0x00000027,0x505f6c67,0x65567265,
	0x78657472,0x00000000,0x00060006,0x00000027,0x00000000,0x505f6c67,0x7469736f,0x006e6f69,
	0x00070006,0x00000027,0x00000001,0x505f6c67,0x746e696f,0x657a6953,0x00000000,0x00070006,
	0x00000027,0x00000002,0x435f6c67,0x4470696c,0x61747369,0x0065636e,0x00070006,0x00000027,
	0x00000003,0x435f6c67,0x446c6c75,0x61747369,0x0065636e,0x00030005,0x00000029,0x00000000,
	0x00040005,0x0000002c,0x66696e55,0x006d726f,0x00060006,0x0000002c,0x00000000,0x6a6f7270,
	0x69746365,0x00006e6f,0x00050006,0x0000002c,0x00000001,0x6f6c6f63,0x00000072,0x00050006,
	0x0000002c,0x00000002,0x6f636573,0x0000646e,0x00030005,0x0000002e,0x00000000,0x00030005,
	0x00000036,0x00726c63,0x00040047,0x0000000c,0x0000001e,0x00000000,0x00040047,0x0000001a,
	0x0000000b,0x0000002a,0x00030047,0x00000027,0x00000002,0x00050048,0x00000027,0x00000000,
	0x0000000b,0x00000000,0x00050048,0x00000027,0x00000001,0x0000000b,0x00000001,0x00050048,
	0x00000027,0x00000002,0x0000000b,0x00000003,0x00050048,0x00000027,0x00000003,0x0000000b,
	0x00000004,0x00030047,0x0000002c,0x00000002,0x00040048,0x0000002c,0x00000000,0x00000005,
	0x00050048,0x0000002c,0x00000000,0x00000007,0x00000010,0x00050048,0x0000002c,0x00000000,
	0x00000023,0x00000000,0x00050048,0x0000002c,0x00000001,0x00000023,0x00000040,0x00050048,
	0x0000002c,0x00000002,0x00000023,0x00000050,0x00040047,0x0000002e,0x00000021,0x00000000,
	0x00040047,0x0000002e,0x00000022,0x00000001,0x00040047,0x00000036,0x0000001e,0x00000000,
	0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,0x00000020,
	0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000007,0x00000007,
	0x00040017,0x0000000a,0x00000006,0x00000002,0x00040020,0x0000000b,0x00000001,0x0000000a,
	0x0004003b,0x0000000b,0x0000000c,0x00000001,0x00040015,0x0000000e,0x00000020,0x00000000,
	0x0004002b,0x0000000e,0x0000000f,0x00000004,0x0004001c,0x00000010,0x0000000a,0x0000000f,
	0x0004002b,0x00000006,0x00000011,0x00000000,0x0005002c,0x0000000a,0x00000012,0x00000011,
	0x00000011,0x0004002b,0x00000006,0x00000013,0x3f800000,0x0005002c,0x0000000a,0x00000014,
	0x00000013,0x00000011,0x0005002c,0x0000000a,0x00000015,0x00000011,0x00000013,0x0005002c,
	0x0000000a,0x00000016,0x00000013,0x00000013,0x0007002c,0x00000010,0x00000017,0x00000012,
	0x00000014,0x00000015,0x00000016,0x00040015,0x00000018,0x00000020,0x00000001,0x00040020,
	0x00000019,0x00000001,0x00000018,0x0004003b,0x00000019,0x0000001a,0x00000001,0x00040020,
	0x0000001c,0x00000007,0x00000010,0x00040020,0x0000001e,0x00000007,0x0000000a,0x0004002b,
	0x0000000e,0x00000025,0x00000001,0x0004001c,0x00000026,0x00000006,0x00000025,0x0006001e,
	0x00000027,0x00000007,0x00000006,0x00000026,0x00000026,0x00040020,0x00000028,0x00000003,
	0x00000027,0x0004003b,0x00000028,0x00000029,0x00000003,0x0004002b,0x00000018,0x0000002a,
	0x00000000,0x00040018,0x0000002b,0x00000007,0x00000004,0x0005001e,0x0000002c,0x0000002b,
	0x00000007,0x00000007,0x00040020,0x0000002d,0x00000002,0x0000002c,0x0004003b,0x0000002d,
	0x0000002e,0x00000002,0x00040020,0x0000002f,0x00000002,0x0000002b,0x00040020,0x00000034,
	0x00000003,0x00000007,0x0004003b,0x00000034,0x00000036,0x00000003,0x0004002b,0x00000018,
	0x00000037,0x00000001,0x00040020,0x00000038,0x00000002,0x00000007,0x00050036,0x00000002,
	0x00000004,0x00000000,0x00000003,0x000200f8,0x00000005,0x0004003b,0x00000008,0x00000009,
	0x00000007,0x0004003b,0x0000001c,0x0000001d,0x00000007,0x0004003d,0x0000000a,0x0000000d,
	0x0000000c,0x0004003d,0x00000018,0x0000001b,0x0000001a,0x0003003e,0x0000001d,0x00000017,
	0x00050041,0x0000001e,0x0000001f,0x0000001d,0x0000001b,0x0004003d,0x0000000a,0x00000020,
	0x0000001f,0x00050081,0x0000000a,0x00000021,0x0000000d,0x00000020,0x00050051,0x00000006,
	0x00000022,0x00000021,0x00000000,0x00050051,0x00000006,0x00000023,0x00000021,0x00000001,
	0x00070050,0x00000007,0x00000024,0x00000022,0x00000023,0x00000011,0x00000013,0x0003003e,
	0x00000009,0x00000024,0x00050041,0x0000002f,0x00000030,0x0000002e,0x0000002a,0x0004003d,
	0x0000002b,0x00000031,0x00000030,0x0004003d,0x00000007,0x00000032,0x00000009,0x00050091,
	0x00000007,0x00000033,0x00000031,0x00000032,0x00050041,0x00000034,0x00000035,0x00000029,
	0x0000002a,0x0003003e,0x00000035,0x00000033,0x00050041,0x00000038,0x00000039,0x0000002e,
	0x00000037,0x0004003d,0x00000007,0x0000003a,0x00000039,0x0003003e,0x00000036,0x0000003a,
	0x000100fd,0x00010038
};

static const uint32_t spirv_frag[] = {
    0x07230203,0x00010000,0x0008000b,0x0000000d,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000b,0x00030010,
	0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
	0x00000000,0x00050005,0x00000009,0x67617266,0x6c6f635f,0x0000726f,0x00030005,0x0000000b,
	0x00726c63,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000b,0x0000001e,
	0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
	0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
	0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040020,0x0000000a,0x00000001,
	0x00000007,0x0004003b,0x0000000a,0x0000000b,0x00000001,0x00050036,0x00000002,0x00000004,
	0x00000000,0x00000003,0x000200f8,0x00000005,0x0004003d,0x00000007,0x0000000c,0x0000000b,
	0x0003003e,0x00000009,0x0000000c,0x000100fd,0x00010038
};

namespace chroma {

    BrushCommand::BrushCommand() noexcept {
        SDL_GPUDevice *device = App::get_device();

        SDL_GPUShaderCreateInfo vertex_shader_info = {};
        vertex_shader_info.entrypoint = "main";
        vertex_shader_info.stage = SDL_GPU_SHADERSTAGE_VERTEX;
        vertex_shader_info.num_uniform_buffers  = 1;
        vertex_shader_info.num_storage_buffers = 0;
        vertex_shader_info.num_storage_textures = 0;
        vertex_shader_info.num_samplers = 0;
        vertex_shader_info.format = SDL_GPU_SHADERFORMAT_SPIRV;
        vertex_shader_info.code = (uint8_t*)spirv_vertex;
        vertex_shader_info.code_size = sizeof(spirv_vertex);

        SDL_GPUShaderCreateInfo fragment_shader_info = {};
        fragment_shader_info.entrypoint = "main";
        fragment_shader_info.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
        fragment_shader_info.num_uniform_buffers  = 0;
        fragment_shader_info.num_storage_buffers  = 0;
        fragment_shader_info.num_storage_textures = 0;
        fragment_shader_info.num_samplers = 0;
        fragment_shader_info.format = SDL_GPU_SHADERFORMAT_SPIRV;
        fragment_shader_info.code = (uint8_t*)spirv_frag;
        fragment_shader_info.code_size = sizeof(spirv_frag);

        this->vertex_shader = SDL_CreateGPUShader(device, &vertex_shader_info);
        this->fragment_shader = SDL_CreateGPUShader(device, &fragment_shader_info);

        SDL_GPUVertexBufferDescription vertex_buffer_desc[1];
        vertex_buffer_desc[0].slot = 0;
        vertex_buffer_desc[0].input_rate = SDL_GPU_VERTEXINPUTRATE_INSTANCE;
        vertex_buffer_desc[0].instance_step_rate = 0;
        vertex_buffer_desc[0].pitch = sizeof(Pos);

        // vertex_buffer_desc[1].slot = 1;
        // vertex_buffer_desc[1].input_rate = SDL_GPU_VERTEXINPUTRATE_INSTANCE;
        // vertex_buffer_desc[1].instance_step_rate = 0;
        // vertex_buffer_desc[1].pitch = sizeof(Pos);

        SDL_GPUVertexAttribute vertex_attributes[1];
        vertex_attributes[0].buffer_slot = 0;
        vertex_attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
        vertex_attributes[0].location = 0;
        vertex_attributes[0].offset = offsetof(ImDrawVert,pos);

        // vertex_attributes[1].buffer_slot = 0;
        // vertex_attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
        // vertex_attributes[1].location = 1;
        // vertex_attributes[1].offset = offsetof(ImDrawVert, uv);

        // vertex_attributes[2].buffer_slot = 0;
        // vertex_attributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM;
        // vertex_attributes[2].location = 2;
        // vertex_attributes[2].offset = offsetof(ImDrawVert, col);

        SDL_GPUVertexInputState vertex_input_state = {};
        vertex_input_state.num_vertex_attributes = 1;
        vertex_input_state.vertex_attributes = vertex_attributes;
        vertex_input_state.num_vertex_buffers = 1;
        vertex_input_state.vertex_buffer_descriptions = vertex_buffer_desc;

        SDL_GPURasterizerState rasterizer_state = {};
        rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
        rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
        rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
        rasterizer_state.enable_depth_bias = false;
        rasterizer_state.enable_depth_clip = false;

        SDL_GPUMultisampleState multisample_state = {};
        multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
        multisample_state.enable_mask = false;

        SDL_GPUDepthStencilState depth_stencil_state = {};
        depth_stencil_state.enable_depth_test = false;
        depth_stencil_state.enable_depth_write = false;
        depth_stencil_state.enable_stencil_test = false;

        SDL_GPUColorTargetBlendState blend_state = {};
        blend_state.enable_blend = true;
        blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
        blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
        blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
        blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
        blend_state.color_write_mask = SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A;

        SDL_GPUColorTargetDescription color_target_desc[1];
        color_target_desc[0].format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        color_target_desc[0].blend_state = blend_state;

        SDL_GPUGraphicsPipelineTargetInfo target_info = {};
        target_info.num_color_targets = 1;
        target_info.color_target_descriptions = color_target_desc;
        target_info.has_depth_stencil_target = false;

        SDL_GPUGraphicsPipelineCreateInfo pipeline_info = {};
        pipeline_info.vertex_shader = vertex_shader;
        pipeline_info.fragment_shader = fragment_shader;
        pipeline_info.vertex_input_state = vertex_input_state;
        pipeline_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;
        pipeline_info.rasterizer_state = rasterizer_state;
        pipeline_info.multisample_state = multisample_state;
        pipeline_info.depth_stencil_state = depth_stencil_state;
        pipeline_info.target_info = target_info;

        this->pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipeline_info);
    }

    BrushCommand::~BrushCommand() noexcept {
        SDL_GPUDevice *device = App::get_device();

        if (pipeline) {
            SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
            pipeline = nullptr;
        }

        if (vertex_shader) {
            SDL_ReleaseGPUShader(device, vertex_shader);
            vertex_shader = nullptr;
        }

        if (fragment_shader) {
            SDL_ReleaseGPUShader(device, fragment_shader);
            fragment_shader = nullptr;
        }

        if (instance_buffer) {
            SDL_ReleaseGPUBuffer(device, instance_buffer);
            instance_buffer = nullptr;
        }
    }

    void BrushCommand::add(uint32_t x, uint32_t y, const Color &color) noexcept {
        positions.push_back({(float)x, (float)y});
        previous_colors.push_back(color);

        SDL_GPUDevice *device = App::get_device();
        SDL_GPUCommandBuffer *cmd_buffer = App::get_command_buffer();

        if (instance_buffer) {
            SDL_ReleaseGPUBuffer(device, instance_buffer);
            instance_buffer = nullptr;
        }

        SDL_GPUTransferBufferCreateInfo transfer_info = {};
        transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transfer_info.size = sizeof(Pos) * positions.size();

        SDL_GPUBufferCreateInfo buffer_info = {};
        buffer_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        buffer_info.size = transfer_info.size;

        SDL_GPUTransferBuffer *transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_info);
        instance_buffer = SDL_CreateGPUBuffer(device, &buffer_info);

        void *mapped = SDL_MapGPUTransferBuffer(device, transfer_buffer, true);

        std::memcpy(
            mapped,
            positions.data(),
            sizeof(Pos) * positions.size()
        );

        SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

        SDL_GPUTransferBufferLocation source = {};
        source.transfer_buffer = transfer_buffer;
        source.offset = 0;

        SDL_GPUBufferRegion destination = {};
        destination.buffer = instance_buffer;
        destination.offset = 0;
        destination.size = transfer_info.size;

        SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(cmd_buffer);
        SDL_UploadToGPUBuffer(
            copy_pass,
            &source,
            &destination,
            true
        );

        SDL_EndGPUCopyPass(copy_pass);

        SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
    }

    bool BrushCommand::contains(uint32_t x, uint32_t y) const noexcept {
        for (size_t i = 0; i < positions.size(); ++i) {
            if (positions[i].x == x && positions[i].y == y) {
                return true;
            }
        }
        return false;
    }

    void BrushCommand::redo(Canvas &canvas) noexcept {
        for (size_t i = 0; i < positions.size(); ++i) {
            const Pos &pos = positions[i];
            canvas.set_color(pos.x, pos.y, main);
        }
    }

    void BrushCommand::undo(Canvas &canvas) noexcept {
        for (size_t i = 0; i < positions.size(); ++i) {
            const Pos &pos = positions[i];
            const Color &old_color = previous_colors[i];
            canvas.set_color(pos.x, pos.y, old_color);
        }
    }

    void BrushCommand::start(uint32_t x, uint32_t y, const Color &color) noexcept {
        add(x, y, color);
    }

    void BrushCommand::update(uint32_t x, uint32_t y, const Color &color) noexcept {
        if (!contains(x, y)) {
            add(x, y, color);
        }
    }

    void BrushCommand::end(uint32_t x, uint32_t y, const Color &color) noexcept {
        update(x, y, color);
    }

    void BrushCommand::discard() noexcept {
        positions.clear();
        // SDL_GPUDevice *device = App::get_device();

        // if (instance_buffer) {
        //     SDL_ReleaseGPUBuffer(device, instance_buffer);
        //     instance_buffer = nullptr;
        // }
    }

    void BrushCommand::preview(SDL_GPURenderPass *render_pass) const noexcept {
        if (positions.empty()) return;

        SDL_GPUBufferBinding bindings[1];
        bindings[0].buffer = instance_buffer;
        bindings[0].offset = 0;

        SDL_BindGPUVertexBuffers(
            render_pass,
            0,
            bindings,
            1
        );

        SDL_BindGPUGraphicsPipeline(render_pass, pipeline);

        SDL_DrawGPUPrimitives(
            render_pass,
            4,
            positions.size(),
            0,
            0
        );
    }

}
