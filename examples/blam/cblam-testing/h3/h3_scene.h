#pragma once

#include "../h2/h2_scene.h"

#include <coffee/components/subsystem.h>
#include <coffee/graphics/apis/gleam/rhi_buffer.h>
#include <coffee/graphics/apis/gleam/rhi_draw_command.h>
#include <coffee/graphics/apis/gleam/rhi_texture.h>

#include <memory>
#include <string>
#include <vector>

/* The Halo 3 world as H3MapLoader builds it for H3Renderer. Shares the
 * Halo 2 vertex layout and programs for now */
namespace h3 {

using h2::Texture;
using h2::Vertex;

/* One render method: its base map and every draw range using it */
struct Material
{
    std::vector<gfx::draw_command::data_t> draws;
    std::string                            name;
    Texture                                diffuse;
    Vecf2                                  uv_scale{1.f, 1.f};
    bool                                   alpha_test{false};
    bool                                   additive{false};
    bool                                   blended{false}; /*!< alpha blend */
    bool                                   constant_color{false};
    Vecf4                                  tint{1.f, 1.f, 1.f, 1.f};
    bool                                   visible{true};
};

struct H3Scene : compo::SubsystemBase
{
    using type = H3Scene;

    bool loaded{false};
    u32  generation{0}; /*!< Bumped per load, so readers can resync */

    std::vector<Material>          materials;
    std::shared_ptr<gfx::buffer_t> vertex_buf;
    std::shared_ptr<gfx::buffer_t> element_buf;
};

void alloc_h3(compo::EntityContainer& e);
void alloc_h3_renderer(compo::EntityContainer& e);

} // namespace h3
