#pragma once

#include "components.h"

#include <blam/dimeter/collision.h>
#include <blam/dimeter/geometry.h>
#include <coffee/components/subsystem.h>
#include <coffee/graphics/apis/gleam/rhi_buffer.h>
#include <coffee/graphics/apis/gleam/rhi_compat.h>
#include <coffee/graphics/apis/gleam/rhi_draw_command.h>
#include <coffee/graphics/apis/gleam/rhi_texture.h>

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* The Halo 2 world as the loader builds it and the occluder and renderer
 * read it. H2MapLoader writes H2Scene, H2Occluder writes H2Visibility;
 * nothing here points back at either */
namespace h2 {

namespace dim  = blam::dimeter;
namespace geo  = blam::dimeter::geo;
namespace coll = blam::dimeter::coll;
using semantic::Span;

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

/* One ltmp geometry bucket decoded: the streams per-vertex lit instances
 * and scenery index into by vertex */
struct LightBucket
{
    std::vector<i16> uv;  /*!< 2 per vertex */
    std::vector<u8>  rgb; /*!< 3 per vertex */
};

/* Where a scenery placement's baked lighting lives */
struct SceneryLight
{
    i32              bucket{-1};
    std::vector<i16> section_offsets; /*!< Vertex offset per model section */
};

/* One cluster's lightmap, decoded to RGBA and placed in the atlas */
struct LightmapImage
{
    u32             width{0}, height{0};
    std::vector<u8> rgba;
    u32             layer{0}, x{0}, y{0};
};

/* Shelf packer over fixed-size pages */
struct LightmapAtlas
{
    static constexpr u32 page = 1024;

    struct shelf_t
    {
        u32 x{0}, y{0}, height{0};
    };
    std::vector<shelf_t> pages; /*!< Current shelf per page */

    bool place(LightmapImage& image)
    {
        if(image.width > page || image.height > page)
            return false;
        for(size_t layer = 0; layer < pages.size(); layer++)
        {
            auto& shelf = pages[layer];
            if(shelf.x + image.width > page)
            {
                /* Next shelf */
                if(shelf.y + shelf.height + image.height > page)
                    continue;
                shelf.y += shelf.height;
                shelf.x      = 0;
                shelf.height = 0;
            }
            if(shelf.y + image.height > page)
                continue;
            image.layer  = static_cast<u32>(layer);
            image.x      = shelf.x;
            image.y      = shelf.y;
            shelf.x += image.width;
            shelf.height = std::max(shelf.height, image.height);
            return true;
        }
        pages.push_back({});
        return place(image);
    }
};

struct Texture
{
    std::shared_ptr<gfx::texture_2d_t> texture;
    std::shared_ptr<gfx::sampler_t>    sampler;
};

/* Fragment program per material family; see fragments/h2_common.glsl */
enum class Variant : u8
{

    base,
    detail,
    blend,
    base_illum,
    detail_illum,
    blend_illum,
    sky,
    sky_mask,
    water,
    clouds,
    ocean,
    plasma,
    count,
};

inline constexpr std::string_view variant_files[] = {
    "h2_base",
    "h2_detail",
    "h2_blend",
    "h2_base_illum",
    "h2_detail_illum",
    "h2_blend_illum",
    "h2_sky",
    "h2_sky_mask",
    "h2_water",
    "h2_clouds",
    "h2_ocean",
    "h2_plasma",
};

static_assert(
    std::size(variant_files) == static_cast<size_t>(Variant::count));

/* Where a draw lives, each -1 = unconstrained: `section` is the structure
 * BSP it belongs to, `cluster` or `instance` its place in the visibility
 * data of `cluster_section` */
struct DrawTag
{
    i32 section{-1};
    i32 cluster_section{-1};
    i32 cluster{-1};
    i32 instance{-1};
    i32  object{-1}; /*!< Into H2Scene::moving_objects: drawn where it is */
    i32  animated{-1}; /*!< Into H2Scene::animated_objects */
    bool placed{false}; /*!< A scenario object, not structure */
    bool first_person{false}; /*!< Placed on the camera, own view only */
};

/* One shad tag: its diffuse map and every draw range using it */
struct Material
{
    Variant                                variant{Variant::base};
    struct draw_t
    {
        gfx::draw_command::data_t cmd;
        DrawTag                   tag;
        /*! Bounding sphere, center + radius; object space for objects
         * drawn where they are */
        Vecf4 bounds{0.f, 0.f, 0.f, -1.f};
    };
    std::vector<draw_t>                    section_draws;
    std::string                            name;
    std::shared_ptr<gfx::texture_2d_t>     texture;
    std::shared_ptr<gfx::sampler_t>        sampler;
    Vecf2                                  uv_scale{1.f, 1.f};
    /* Detail layers: 1 = detail modulated x2, 2 = two details blended by
     * the base alpha then modulated x2 */
    i32                                    detail_mode{0};
    Texture                                detail;
    Vecf2                                  detail_scale{1.f, 1.f};
    Texture                                detail2;
    Vecf2                                  detail2_scale{1.f, 1.f};
    Texture                                mask; /*!< Sky clouds: alpha */
    Texture                                illum; /*!< Added on top */
    Vecf2                                  illum_scale{1.f, 1.f};
    Vecf3                                  illum_color{1.f, 1.f, 1.f};
    Texture                                bump; /*!< Normal map, rg = u, v */
    Vecf2                                  bump_scale{1.f, 1.f};
    bool                                   alpha_test{false};
    bool                                   sky{false}; /*!< follows the camera, unlit */
    bool                                   water{false}; /*!< bump in `texture`, drawn last */
    bool                                   additive{false}; /*!< transparent template: glow pass */
    bool                                   blended{false};  /*!< transparent template: alpha blend */
    /*! water_static and its edge blend: bump in `texture`, sky in `detail`,
     * the edge fade in `mask` */
    bool                                   ocean{false};
    bool                                   water_fan{false};  /*!< laid out around the camera */
    /*! plasma_alpha: noise A in `texture`, noise B in `detail`, shape in
     * `mask`; the pixel constants ramp it from dim to bright */
    bool                                   plasma{false};
    /*! Environment reflection: a cube map, tinted from facing to glancing,
     * each rgb + brightness */
    std::shared_ptr<gfx::texture_cube_t>   env_texture;
    std::shared_ptr<gfx::sampler_t>        env_sampler;
    Vecf4                                  env_facing{0.f};
    Vecf4                                  env_glancing{0.f};
    std::array<Vecf3, 2>                   plasma_colors{};
    Vecf2                                  mask_scale{1.f, 1.f};
};

/* A sky light's lens flare: sprites along the line from the light through
 * the screen centre */
struct SkyFlare
{
    struct Sprite
    {
        Texture image;
        f32     position{0.f}; /*!< 0 at the light, 0.5 at the screen centre */
        f32     radius{0.f};   /*!< Fraction of the screen height */
        Vecf3   color{1.f, 1.f, 1.f};
    };

    Vecf3               direction{0.f, 0.f, 1.f}; /*!< Towards the light */
    Vecf3               color{1.f, 1.f, 1.f};     /*!< Its radiosity colour */
    std::vector<Sprite> sprites;
};

/* Triangle soup for Bullet; owned by the physics body once adopted */
struct CollisionMesh
{
    std::vector<Vecf3> vertices;
    std::vector<i32>   indices;
};

/* One structure BSP of the scenario: only the active one draws and
 * collides, each carries its own far-off stand-ins for the others */
struct Section
{
    struct Portal
    {
        i16                front, back;
        std::vector<Vecf3> vertices;

        inline i16 front_cluster() const
        {
            return front;
        }

        inline i16 back_cluster() const
        {
            return back;
        }
    };

    struct Cluster
    {
        Vecf3               lo, hi;
        std::vector<Portal> portals;
        Vecf4               fog{0.f, 0.f, 0.f, 0.f}; /*!< rgb, max density */
        Vecf2               fog_range{0.f, 1.f};    /*!< start, opaque distance */
    };

    /* The section's planar fog: everything under `plane` (normal, d) */
    struct PlanarFog
    {
        Vecf4 plane{0.f, 0.f, 1.f, -1e9f};
        Vecf4 color{0.f, 0.f, 0.f, 0.f}; /*!< rgb, max density */
        Vecf2 opaque{1.f, 1.f};          /*!< distance into, depth below */
    };

    std::string                    name;
    Vecf3                          lo, hi; /*!< World bounds, stand-ins included */
    std::shared_ptr<CollisionMesh> collision;
    PlanarFog                      planar_fog;

    /* Visibility: the collision tree's leaves name their cluster, clusters
     * their portals; instances list the clusters that own them */
    std::vector<coll::bsp3d_node_t> nodes;
    std::vector<coll::plane_t>      planes;
    std::vector<i16>                leaf_cluster;
    std::vector<Cluster>            clusters;
    std::vector<std::vector<i16>>   instance_clusters;

    /* The cluster around a point, nullopt in solid space */
    inline std::optional<u32> cluster_of(Vecf3 const& p) const
    {
        auto leaf = coll::find_leaf(
            Span<coll::bsp3d_node_t const>(nodes),
            Span<coll::plane_t const>(planes),
            p.x,
            p.y,
            p.z);
        if(!leaf || *leaf >= leaf_cluster.size())
            return std::nullopt;
        i16 c = leaf_cluster[*leaf];
        if(c < 0 || static_cast<size_t>(c) >= clusters.size())
            return std::nullopt;
        return static_cast<u32>(c);
    }
};

/* Camera inside this volume while in `source` switches to `destination` */
struct SwitchTrigger
{
    Vecf3 lo, hi;
    i32   source, destination;
};


/* Everything a map load produces; GPU buffers and textures included */
struct H2Scene : compo::SubsystemBase
{
    using type = H2Scene;

    bool loaded{false};
    u32  generation{0}; /*!< Bumped per load, so readers can resync */
    i32  start_section{0};

    std::vector<Section>       sections;
    std::vector<SwitchTrigger> switches;
    std::vector<Material>      materials;

    std::shared_ptr<gfx::buffer_t>              vertex_buf;
    std::shared_ptr<gfx::buffer_t>              element_buf;
    std::shared_ptr<gfx::compat::texture_2da_t> lightmap_tex;
    std::shared_ptr<gfx::sampler_t>             lightmap_sampler;
    f32                                         sky_scale{1.f};
    Vecf3                                       sky_clear{0.f, 0.f, 0.f};
    /*! Towards the sky's first light, with a lens flare or not */
    Vecf3                                       sun_direction{0.f, 0.f, 1.f};
    std::vector<SkyFlare>                       sky_flares;
    /*! The BSP's water definition: how the sea shades */
    struct water_t
    {
        Vecf3 sun_spot{1.f, 1.f, 1.f};
        f32   sun_power{0.f};
        Vecf3 reflection{1.f, 1.f, 1.f};
        Vecf3 refraction{1.f, 1.f, 1.f};
        Vecf3 horizon{0.f, 0.f, 0.f};
    } water;
    /*! Entities of the objects with physics; their geometry is in object
     * space and follows the entity's Model */
    std::vector<u64>                            moving_objects;

    /*! One looping animation as skin matrices (bind pose model space to
     * animated model space), frame-major */
    struct animation_t
    {
        u32                frames{0};
        u32                nodes{0};
        f32                rate{30.f}; /*!< Frames a second */
        std::vector<Matf4> palette;
    };
    /*! A placed object playing its idle loop; geometry in object space */
    struct animated_t
    {
        Matf4 transform{1.f};
        u32   animation{0};
    };
    std::vector<animation_t> animations;
    std::vector<animated_t>  animated_objects;
    /*! The player's view models, into animated_objects; hands and weapon
     * hang from the eye, the body stands with its head there */
    struct first_person_t
    {
        i32   hands{-1};
        i32   weapon{-1};
        i32   body{-1};
        Vecf3 eye{0.f, 0.f, 0.6f}; /*!< Body space, as posed */
    } first_person;
};

/* What the occluder decided for this frame */
struct H2Visibility : compo::SubsystemBase
{
    using type = H2Visibility;

    i32 active{-1}; /*!< Section index, -1 = none */
    /* Per cluster of the active section; empty = everything */
    std::vector<bool> visible;
    u32               camera_cluster{std::numeric_limits<u32>::max()};
    Vecf4             fog{0.f, 0.f, 0.f, 0.f};
    Vecf2             fog_range{0.f, 1.f};
    Section::PlanarFog planar_fog;

    bool draw_visible(DrawTag const& tag, std::vector<Section> const& sections) const
    {
        if(tag.section >= 0 && tag.section != active)
            return false;
        if(visible.empty() || tag.cluster_section != active)
            return true;
        if(tag.cluster >= 0)
            return static_cast<size_t>(tag.cluster) < visible.size() &&
                   visible[tag.cluster];
        if(tag.instance >= 0)
        {
            auto const& owners = sections[active].instance_clusters;
            if(static_cast<size_t>(tag.instance) >= owners.size() ||
               owners[tag.instance].empty())
                return true;
            for(i16 c : owners[tag.instance])
                if(c >= 0 && static_cast<size_t>(c) < visible.size() &&
                   visible[c])
                    return true;
            return false;
        }
        return true;
    }
};

/* Entity id of the map's static collision body; out of the range the
 * object spawns use */
inline constexpr u64 collision_body_id = 0x4832'0000'0000'0000ull;

void alloc_h2(compo::EntityContainer& e);
void alloc_h2_occluder(compo::EntityContainer& e);
void alloc_h2_renderer(compo::EntityContainer& e);

} // namespace h2
