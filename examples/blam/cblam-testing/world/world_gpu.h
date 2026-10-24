#pragma once

#include "types.h"

#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_compat.h>

#include <memory>

namespace world {

/*! A vertex array laid out for world::Vertex; `skinned` adds bone nodes and
 * weights */
std::shared_ptr<gfx::vertex_array_t> alloc_vertex_layout(gfx::api& api, bool skinned);

/*! Stand-ins bound where a material has no texture of its own */
struct FallbackTextures
{
    std::shared_ptr<gfx::texture_2d_t>          white;
    std::shared_ptr<gfx::sampler_t>             white_sampler;
    std::shared_ptr<gfx::compat::texture_2da_t> white_array; /*!< No lightmap or light probe */
    std::shared_ptr<gfx::sampler_t>             white_array_sampler;
    std::shared_ptr<gfx::texture_cube_t>        black_cube;
    std::shared_ptr<gfx::sampler_t>             black_cube_sampler;

    void alloc(gfx::api& api, u8 cube_alpha);
};

} // namespace world
