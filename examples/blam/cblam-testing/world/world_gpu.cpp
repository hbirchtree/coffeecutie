#include "world_gpu.h"

#include <coffee/graphics/apis/gleam/rhi_vertex.h>

#include <array>
#include <cstddef>
#include <vector>

namespace world {

std::shared_ptr<gfx::vertex_array_t> alloc_vertex_layout(gfx::api& api, bool skinned)
{
    using flags = gfx::vertex_attribute::attribute_flags;
    auto vao    = api.alloc_vertex_array();
    vao->alloc();
    vao->add(gfx::vertex_attribute{
        .index = 0,
        .value = {.offset = 0, .stride = sizeof(Vertex), .count = 3},
    });
    vao->add(gfx::vertex_attribute{
        .index = 1,
        .value = {.offset = offsetof(Vertex, texcoord), .stride = sizeof(Vertex), .count = 2},
    });
    vao->add(gfx::vertex_attribute{
        .index = 2,
        .value = {.offset = offsetof(Vertex, normal), .stride = sizeof(Vertex), .count = 3},
    });
    vao->add(gfx::vertex_attribute{
        .index = 3,
        .value = {.offset = offsetof(Vertex, lightmap), .stride = sizeof(Vertex), .count = 3},
    });
    vao->add(gfx::vertex_attribute{
        .index = 4,
        .value =
            {.offset = offsetof(Vertex, light),
             .stride = sizeof(Vertex),
             .count  = 4,
             .type   = semantic::type_t::u8,
             /* packed: a float input, never an integer attribute */
             .flags = flags::packed | flags::normalized},
    });
    if(!skinned)
    {
        vao->set_attribute_names({
            {"position", 0},
            {"texcoord", 1},
            {"normal", 2},
            {"lightmap", 3},
            {"vertex_light", 4},
        });
        return vao;
    }
    vao->add(gfx::vertex_attribute{
        .index = 5,
        .value =
            {.offset = offsetof(Vertex, nodes),
             .stride = sizeof(Vertex),
             .count  = 4,
             .type   = semantic::type_t::u8,
             .flags  = flags::packed},
    });
    vao->add(gfx::vertex_attribute{
        .index = 6,
        .value =
            {.offset = offsetof(Vertex, weights),
             .stride = sizeof(Vertex),
             .count  = 4,
             .type   = semantic::type_t::u8,
             .flags  = flags::packed | flags::normalized},
    });
    vao->set_attribute_names({
        {"position", 0},
        {"texcoord", 1},
        {"normal", 2},
        {"lightmap", 3},
        {"vertex_light", 4},
        {"bone_nodes", 5},
        {"bone_weights", 6},
    });
    return vao;
}

void FallbackTextures::alloc(gfx::api& api, u8 cube_alpha)
{
    auto const rgba8 = typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8);

    white = api.alloc_texture(gfx::textures::d2, rgba8, 1);
    white->alloc(size_3d<u32>{2, 2, 1});
    white->upload(std::vector<u8>(16, 0xFF), Veci2{0, 0}, Veci2{2, 2}, 0);
    white_sampler = white->sampler();
    white_sampler->alloc();

    white_array = std::make_shared<gfx::compat::texture_2da_t>(&api, rgba8, 1);
    white_array->alloc(size_3d<u32>{2, 2, 1});
    white_array->upload(std::vector<u8>(16, 0xFF), Veci3{0, 0, 0}, size_3d<i32>{2, 2, 1});
    white_array_sampler = white_array->sampler();
    white_array_sampler->alloc();

    black_cube = api.alloc_texture(gfx::textures::cube, rgba8, 1);
    black_cube->alloc(size_3d<u32>{1, 1, 1});
    std::array<u8, 4>                       black{0, 0, 0, cube_alpha};
    std::array<semantic::Span<const u8>, 6> faces;
    faces.fill(semantic::Span<const u8>(black.data(), black.size()));
    black_cube->upload(faces, Veci2{0, 0}, Veci2{1, 1}, 0);
    black_cube_sampler = black_cube->sampler();
    black_cube_sampler->alloc();
}

} // namespace world
