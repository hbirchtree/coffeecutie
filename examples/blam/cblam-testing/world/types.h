#pragma once

#include "../components.h"

#include <coffee/graphics/apis/gleam/rhi_texture.h>

#include <memory>
#include <vector>

/* Value types the Halo 2 and 3 worlds share */
namespace world {

struct Vertex
{
    Vecf3 position;
    Vecf2 texcoord;
    Vecf3 normal;
    Vecf3 lightmap{0.f, 0.f, -1.f}; /*!< Atlas u, v, layer; -1 = none */
    u8    light[4]{0, 0, 0, 0};     /*!< Baked per-vertex light, a = valid */
    u8    nodes[4]{0, 0, 0, 0};     /*!< Model nodes skinning the vertex */
    u8    weights[4]{0, 0, 0, 0};   /*!< Their weights; all 0 = unskinned */
};

static_assert(sizeof(Vertex) == 56);

struct Texture
{
    std::shared_ptr<gfx::texture_2d_t> texture;
    std::shared_ptr<gfx::sampler_t>    sampler;
};

/* Collision polygons fanned into triangles */
struct CollisionMesh
{
    std::vector<Vecf3> vertices;
    std::vector<i32>   indices;
    std::vector<u16>   materials; /*!< Per triangle: matg material index, 0xFFFF none; empty when unknown */
};

/* Depth the first-person view model is squeezed into, in front of the world */
inline constexpr double first_person_depth = 0.98;

} // namespace world
