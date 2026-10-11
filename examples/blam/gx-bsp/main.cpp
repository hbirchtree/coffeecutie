/* Trimmed BlamGraphics alternative for the GameCube (gexxo): load a PC or Xbox
 * Halo map (demand-paged), walk its BSP geometry, and draw it diffuse *
 * lightmap with the GX fixed-function pipeline, plus the placed objects lit by
 * a GX hardware light.
 *
 * Per BSP `lightmap` struct = one page (geometry sharing a lightmap bitmap).
 * Each material in a page = a submesh with its own diffuse texture (from the
 * shader's base map). Vertices carry both the diffuse UV and the lightmap UV.
 * Everything is byte-swapped to host (big-endian) order, and Xbox's compressed
 * vertices are expanded on the way, since GX can't read either map's layout.
 */

#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/coffee.h>

#include <peripherals/semantic/chunk.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>

#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_bsp_structures.h>
#include <blam/volta/blam_endian.h>
#include <blam/volta/blam_globals.h>
#include <blam/volta/blam_mod2.h>
#include <blam/volta/blam_scenario.h>
#include <blam/volta/blam_shaders.h>
#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_swizzle.h>
#include <blam/volta/blam_unit.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#if defined(FEATURE_ENABLE_Gexxo)
#include <coffee/gexxo/gexxo_api.h>
#include <coffee/graphics/apis/CGexxo>
#include <coffee/graphics/apis/gexxo/rhi_resources.h>
#include <malloc.h>
#include <ogc/cache.h>
#include <ogc/gx.h>
#include <ogc/lwp_watchdog.h>
#include <ogc/system.h>
#endif
#if defined(COFFEE_GEKKO)
#include <ogc/pad.h>
#include <platforms/gekko/dvd.h>
#include <platforms/gekko/paged_mmap.h>

#include "net_client.h"
#endif

using namespace Coffee;
using blam::from_le;
using typing::vector_types::Matf4;
using typing::vector_types::Vecf2;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;

static constexpr char const* MAP_NAME = "bloodgulch.map";

// Highest-detail model LOD; GC memory may want lod_high or lod_medium
static constexpr auto kModelLod = blam::mod2::lod_high_ext;

// Player bipeds stood on the first spawn points, MP maps place none
static constexpr u32 kSpawnBipeds = 4;

#define LOG(...)                     \
    do                               \
    {                                \
        printf("bsp: " __VA_ARGS__); \
        printf("\n");                \
        fflush(stdout);              \
    } while(0)

/* Interleaved GX vertex: position + diffuse UV + lightmap UV + detail UV
 * (diffuse UV pre-scaled by the shader's detail map scale). */
struct gx_vertex
{
    f32 pos[3];
    f32 duv[2];
    f32 luv[2];
    f32 detuv[2];
};

/* A run of triangles in a page sharing one diffuse texture. */
struct submesh_t
{
    u32                        index_offset{0};
    u32                        index_count{0};
    Vecf3                      center{};  // world-space bounding sphere
    f32                        radius{0}; // for frustum culling
    blam::bitm::image_t const* diffuse_img{nullptr};
    blam::bitm::image_t const* detail_img{nullptr};
#if defined(FEATURE_ENABLE_Gexxo)
    std::shared_ptr<gexxo::texture_t> diffuse;
    std::shared_ptr<gexxo::texture_t> detail;
#endif
};

/* One lightmap page: geometry + its lightmap bitmap + per-material submeshes.
 */
struct page_t
{
    std::vector<gx_vertex> verts;
    std::vector<u16>       indices;
    std::vector<submesh_t> submeshes;
    i16                    lm_idx{-1};

#if defined(FEATURE_ENABLE_Gexxo)
    std::shared_ptr<gexxo::vertex_array_t> vao;
    std::shared_ptr<gexxo::buffer_t>       vbo, ebo;
    std::shared_ptr<gexxo::texture_t>      lightmap;
#endif
};

/* Objects: placed mod2 model instances. Geometry is per unique model (deduped
 * by mod2 tag id); each instance just carries a world transform. Normals are
 * S8 with 6 fraction bits, which is what GX lighting reads natively. */
struct mdl_vertex
{
    f32 pos[3];
    i8  nrm[3];
    i8  pad;
    f32 uv[2];
};

struct mdl_part
{
    std::vector<mdl_vertex>    verts;
    std::vector<u16>           indices; // triangle strip
    blam::bitm::image_t const* diffuse_img{nullptr};
#if defined(FEATURE_ENABLE_Gexxo)
    std::shared_ptr<gexxo::vertex_array_t> vao;
    std::shared_ptr<gexxo::buffer_t>       vbo, ebo;
    std::shared_ptr<gexxo::texture_t>      diffuse;
#endif
};

struct model_t
{
    std::vector<mdl_part> parts;
    Vecf3                 center{}; // local-space bounding sphere (for culling)
    f32                   radius{0};
};

enum object_kind
{
    kind_scenery,
    kind_biped,
    kind_vehicle,
    kind_equipment,
    kind_weapon,
    kind_machine,
    kind_control,
    kind_light_fixture,
    kind_netgame_item,
    kind_count,
};

static char const* const g_kind_names[kind_count] = {
    "scenery",
    "bipeds",
    "vehicles",
    "equipment",
    "weapons",
    "machines",
    "controls",
    "light fixtures",
    "netgame items",
};

/* BlamGraphics' ScenarioGroup, per kind: servers address scenario
 * placements as group << 24 | index << 8 | sub */
static constexpr u32 g_kind_net_group[kind_count] = {1, 6, 5, 7, 8, 3, 4, 2, 9};

static constexpr u32 net_id_of(object_kind kind, u32 index, u32 sub = 0)
{
    return (g_kind_net_group[kind] << 24) | ((index & 0xFFFF) << 8) |
           (sub & 0xFF);
}

struct object_inst
{
    u32   model_id;
    Matf4 xform;
    Vecf3 center{}; // world-space bounding sphere
    f32   radius{0};
    u32   net_id{0};
};

static std::map<u32, model_t>   g_models;  // by mod2 tag id
static std::vector<object_inst> g_objects; // placed instances
static u32                      g_kind_count[kind_count] = {};
static u32 g_player_model = 0xFFFFFFFF; // mod2 of the unit players spawn as

static std::vector<page_t> g_pages;
static Vecf3               g_bb_min{1e9f, 1e9f, 1e9f};
static Vecf3               g_bb_max{-1e9f, -1e9f, -1e9f};
static int                 g_err = -1;

// First player spawn (Halo world space, Z up) for a first-person camera.
static bool  g_have_spawn = false;
static Vecf3 g_spawn_pos{};
static f32   g_spawn_rot = 0.f;
static i16   g_spawn_bsp = 0; // which structure BSP the spawn is in
static u32   g_bsp_count = 0;

static bool g_xbox = false;

struct page_bitmap
{
    blam::bitm::image_t const* image{nullptr};
    blam::map_ptr              pix_magic;
};

static std::vector<page_bitmap> g_page_bitmaps;
static blam::map_ptr            g_internal_magic;

#if defined(COFFEE_GEKKO)
static platform::file::gekko::vmem::mapping g_map_window;
#endif

static semantic::BytesConst load_map_bytes()
{
#if defined(COFFEE_GEKKO)
    g_map_window = platform::file::gekko::vmem::map(MAP_NAME);
    if(!g_map_window)
        return {};
    return semantic::BytesConst::ofBytes(g_map_window.base, g_map_window.size);
#else
    static Resource r(MAP_NAME, RSCA::AssetFile);
    return C_OCAST<semantic::BytesConst>(r);
#endif
}

/* Promote a file range of the map to ARAM when the whole map didn't fit:
 * whatever the CPU keeps walking (tag data, the active BSP) shouldn't page
 * off the disc, bitmaps and sounds are read at most once. */
static void stage_hot_range(char const* what, u32 off, u32 size)
{
#if defined(COFFEE_GEKKO)
    namespace vmem = platform::file::gekko::vmem;
    bool const ok  = vmem::stage(g_map_window, off, size);
    LOG("stage %s: %u KiB at 0x%x -> %s (ARAM free %u KiB)",
        what,
        size / 1024,
        off,
        ok ? "ARAM" : "disc",
        vmem::aram_free() / 1024);
#else
    (void)what, (void)off, (void)size;
#endif
}

/* Little-endian scalar out of unaligned map bytes */
template<typename T>
static T rd(u8 const* p)
{
    T v;
    std::memcpy(&v, p, sizeof(T));
    return from_le(v);
}

/* Xbox 11:11:10 signed normal (r = low 11 bits) */
static Vecf3 unpack_normal(u32 v)
{
    auto field = [](u32 x, u32 bits) {
        i32 const s = static_cast<i32>(x << (32 - bits)) >> (32 - bits);
        return static_cast<f32>(s) / static_cast<f32>((1 << (bits - 1)) - 1);
    };
    return {
        field(v & 0x7FF, 11), field((v >> 11) & 0x7FF, 11), field(v >> 22, 10)};
}

static i8 to_s8_frac6(f32 v)
{
    return static_cast<i8>(std::clamp(v * 64.f, -128.f, 127.f));
}

/* --- tag/shader/bitmap resolution ----------------------------------------- */

template<typename V>
static blam::tag_t const* tag_by_id(blam::map_container<V> const& c, u32 tag_id)
{
    u32 const cnt = from_le(c.tags->tag_count);
    u32 const idx = tag_id & 0xFFFF; // salt is the high 16 bits
    if(tag_id == 0xFFFFFFFF || idx >= cnt)
        return nullptr;
    return &c.tags->tags(c.map)[idx];
}

template<typename V>
static blam::bitm::header_t const* resolve_bitm(
    blam::map_container<V> const& c, blam::tagref_t const& ref)
{
    auto const* t = tag_by_id(c, from_le(ref.tag_id));
    if(!t)
        return nullptr;
    auto hr = t->template data<blam::bitm::header_t>(c.magic);
    return hr.has_error() ? nullptr : hr.value();
}

template<typename V>
static blam::bitm::image_t const* first_image(
    blam::map_container<V> const& c, blam::bitm::header_t const* bitm)
{
    if(!bitm)
        return nullptr;
    auto imgs = bitm->images.data(c.magic);
    if(imgs.has_error() || imgs.value().empty())
        return nullptr;
    return &imgs.value()[0];
}

struct shader_info
{
    blam::bitm::image_t const* diffuse{nullptr};
    blam::bitm::image_t const* detail{nullptr};
    f32                        detail_scale{1.f};
};

/* Material shader -> base diffuse + (primary) detail map. shader_environment
 * (BSP) + shader_model (objects) are handled; other classes are untextured. */
template<typename V>
static shader_info resolve_shader(
    blam::map_container<V> const& c, blam::tagref_t const& shader_ref)
{
    shader_info out;
    auto const* st = tag_by_id(c, from_le(shader_ref.tag_id));
    if(!st)
        return out;
    if(st->matches(blam::tag_class_t::shader_environment))
    {
        auto sh = st->template data<blam::shader::shader_env>(c.magic);
        if(sh.has_error())
            return out;
        out.diffuse = first_image(c, resolve_bitm(c, sh.value()->diffuse.base));
        out.detail =
            first_image(c, resolve_bitm(c, sh.value()->diffuse.primary.map));
        out.detail_scale = from_le(sh.value()->diffuse.primary.scale);
    } else if(st->matches(blam::tag_class_t::shader_model))
    {
        auto sh = st->template data<blam::shader::shader_model>(c.magic);
        if(sh.has_error())
            return out;
        out.diffuse = first_image(c, resolve_bitm(c, sh.value()->maps.base));
        out.detail =
            first_image(c, resolve_bitm(c, sh.value()->maps.detail.map));
        out.detail_scale = from_le(sh.value()->maps.detail.scale);
    }
    if(out.detail_scale == 0.f)
        out.detail_scale = 1.f;
    return out;
}

/* --- cluster lookup (RAM copy of the collision tree) ---------------------- */

// Per-object cluster_for_point thrashes the paged map (random collision-tree
// faults). Copy the small collision arrays into RAM once (sequential read =
// each page faulted once), then query against RAM with no further paging.
struct coll_node
{
    i32 plane, back, front;
};

struct coll_plane
{
    Vecf3 n;
    f32   d;
};

static std::vector<coll_node>  g_coll_nodes;
static std::vector<coll_plane> g_coll_planes;
static std::vector<i16>        g_leaf_cluster;

static void build_cluster_lookup(
    blam::bsp::header const& hdr, blam::bsp_ptr const& bm)
{
    auto coll = hdr.collision_header.data(bm, blam::single_value);
    if(coll.has_error())
        return;
    auto nodes  = coll.value()->nodes_3d.data(bm);
    auto planes = coll.value()->planes.data(bm);
    auto leaves = hdr.leaves.data(bm);
    if(nodes.has_error() || planes.has_error() || leaves.has_error())
        return;
    for(auto const& n : nodes.value())
        g_coll_nodes.push_back(
            {from_le(n.plane), from_le(n.back), from_le(n.front)});
    for(auto const& p : planes.value())
        g_coll_planes.push_back(
            {{from_le(p.plane.x), from_le(p.plane.y), from_le(p.plane.z)},
             from_le(p.d)});
    for(auto const& l : leaves.value())
        g_leaf_cluster.push_back(from_le(l.cluster));
}

// Cluster index containing point, or nullopt for solid/outside-the-BSP points.
static std::optional<u32> cluster_at(Vecf3 const& pt)
{
    if(g_coll_nodes.empty())
        return std::nullopt;
    i32 node = 0;
    for(size_t guard = 0; guard <= g_coll_nodes.size(); guard++)
    {
        if(node == -1)
            return std::nullopt;
        if(node < 0)
        {
            u32 const idx = static_cast<u32>(node) & 0x7fffffffu;
            if(idx >= g_leaf_cluster.size())
                return std::nullopt;
            i16 const c = g_leaf_cluster[idx];
            return c < 0 ? std::nullopt : std::optional<u32>(c);
        }
        if(static_cast<u32>(node) >= g_coll_nodes.size())
            return std::nullopt;
        coll_node const& n = g_coll_nodes[node];
        if(n.plane < 0 || static_cast<u32>(n.plane) >= g_coll_planes.size())
            return std::nullopt;
        coll_plane const& pl = g_coll_planes[n.plane];
        node                 = glm::dot(pl.n, pt) >= pl.d ? n.front : n.back;
    }
    return std::nullopt;
}

/* Is a placement in the loaded structure BSP. bsp_flags is a section mask
 * (filled for scenery + light fixtures only), else ask the collision tree. */
static bool in_active_section(Vecf3 const& pos, u16 bsp_flags)
{
    if(g_bsp_count <= 1)
        return true;
    if(bsp_flags)
        return (bsp_flags >> g_spawn_bsp) & 1;
    return cluster_at(pos).has_value();
}

/* --- objects (mod2 models) ------------------------------------------------ */

template<typename V>
static void load_model_cpu(blam::map_container<V> const& c, u32 mod2_id)
{
    using namespace blam;
    auto const* t = tag_by_id(c, mod2_id);
    if(!t)
        return;
    auto hdr = t->template data<mod2::header<V>>(c.magic);
    if(hdr.has_error())
        return;
    std::optional<mod2::model_data_t> md;
    try
    {
        md = hdr.value()->model_at(kModelLod, c.magic);
    } catch(std::exception const&)
    {
    }
    if(!md)
        return;
    Vecf2 const uvscale{
        from_le(hdr.value()->uvscale.x), from_le(hdr.value()->uvscale.y)};
    auto shaders = hdr.value()->shaders.data(c.magic);
    auto vmag    = c.tags->vertex_magic(c.magic);

    model_t model;
    Vecf3   mn{1e9f, 1e9f, 1e9f}, mx{-1e9f, -1e9f, -1e9f};
    for(mod2::part const* p : md->parts)
    {
        mdl_part part;
        try
        {
            auto vseg = p->vertex_segment(*c.tags, c.magic).data(vmag);
            auto iseg = p->index_segment(*c.tags).data(vmag);
            if(vseg.has_error() || iseg.has_error())
                continue;
            for(auto const& mv : vseg.value())
            {
                u8 const*  raw = reinterpret_cast<u8 const*>(&mv);
                mdl_vertex v;
                Vecf3      n;
                for(u32 k = 0; k < 3; k++)
                    v.pos[k] = rd<f32>(raw + k * 4);
                if constexpr(std::is_same_v<V, xbox_version_t>)
                {
                    // mod2_vertex<compressed>: normal u32 @12, uv i16 @24
                    n       = unpack_normal(rd<u32>(raw + 12));
                    v.uv[0] = rd<i16>(raw + 24) / 32767.f * uvscale.x;
                    v.uv[1] = rd<i16>(raw + 26) / 32767.f * uvscale.y;
                } else
                {
                    // mod2_vertex<uncompressed>: normal f32x3 @12, uv @48
                    n = {
                        rd<f32>(raw + 12),
                        rd<f32>(raw + 16),
                        rd<f32>(raw + 20)};
                    v.uv[0] = rd<f32>(raw + 48) * uvscale.x;
                    v.uv[1] = rd<f32>(raw + 52) * uvscale.y;
                }
                for(u32 k = 0; k < 3; k++)
                    v.nrm[k] = to_s8_frac6(n[k]);
                v.pad = 0;
                part.verts.push_back(v);
                Vecf3 const p3{v.pos[0], v.pos[1], v.pos[2]};
                mn = glm::min(mn, p3);
                mx = glm::max(mx, p3);
            }
            for(u16 idx : iseg.value())
                part.indices.push_back(from_le(idx));
        } catch(std::exception const&)
        {
            continue;
        }
        if(!shaders.has_error())
        {
            u16 const si = from_le(p->shader_idx);
            if(si < shaders.value().size())
                part.diffuse_img =
                    resolve_shader(c, shaders.value()[si].ref).diffuse;
        }
        if(!part.verts.empty() && part.indices.size() >= 3)
            model.parts.push_back(std::move(part));
    }
    if(mx.x >= mn.x)
    {
        model.center = (mn + mx) * 0.5f;
        model.radius = glm::length(mx - model.center);
    }
    g_models[mod2_id] = std::move(model);
}

static Matf4 spawn_transform(Vecf3 const& pos, Vecf3 const& rot)
{
    Matf4 m = glm::translate(Matf4{1.f}, pos);
    m       = glm::rotate(m, rot.x, Vecf3{0, 0, 1}); // yaw (Z up)
    m       = glm::rotate(m, rot.y, Vecf3{0, 1, 0}); // pitch
    m       = glm::rotate(m, rot.z, Vecf3{1, 0, 0}); // roll
    return m;
}

/* Any object tag (obje and everything deriving from it) -> its model */
template<typename V>
static bool place_object(
    blam::map_container<V> const& c,
    u32                           object_tag_id,
    Matf4 const&                  xform,
    object_kind                   kind,
    u32                           net_id = 0)
{
    auto const* t = tag_by_id(c, object_tag_id);
    if(!t)
        return false;
    auto obj = t->template data<blam::scn::object>(c.magic);
    if(obj.has_error())
        return false;
    u32 const mod2_id = from_le(obj.value()->model.tag_id);
    if(!tag_by_id(c, mod2_id))
        return false;
    if(g_models.find(mod2_id) == g_models.end())
        load_model_cpu(c, mod2_id);
    auto const& mdl = g_models[mod2_id];
    if(mdl.parts.empty())
        return false;
    Vecf3 const wc = Vecf3(xform * Vecf4(mdl.center, 1.f));

    g_objects.push_back({mod2_id, xform, wc, mdl.radius, net_id});
    g_kind_count[kind]++;
    return true;
}

template<typename V, typename Spawn>
static void place_group(
    blam::map_container<V> const&         c,
    blam::scn::reflex_group<Spawn> const& group,
    object_kind                           kind)
{
    auto insts = group.instances.data(c.magic);
    auto pal   = group.palette.data(c.magic);
    if(insts.has_error() || pal.has_error())
        return;
    for(u32 i = 0; i < insts.value().size(); i++)
    {
        Spawn const& s = insts.value()[i];
        Vecf3 const  pos{from_le(s.pos.x), from_le(s.pos.y), from_le(s.pos.z)};
        if(!in_active_section(pos, static_cast<u16>(from_le(s.bsp_flags))))
            continue;
        i16 const ref = from_le(s.ref.index);
        if(ref < 0 || static_cast<size_t>(ref) >= pal.value().size())
            continue;
        Vecf3 const rot{from_le(s.rot.x), from_le(s.rot.y), from_le(s.rot.z)};
        place_object(
            c,
            from_le(pal.value()[ref][0].tag_id),
            spawn_transform(pos, rot),
            kind,
            net_id_of(kind, i));
    }
}

/* Netgame weapons/equipment hang off item collections, not object palettes;
 * place the collection's first item */
template<typename V>
static void place_netgame_items(
    blam::map_container<V> const& c, blam::scn::scenario<V> const* scn)
{
    auto equipment = scn->netgame.equipment.data(c.magic);
    if(equipment.has_error())
        return;
    for(u32 e = 0; e < equipment.value().size(); e++)
    {
        auto const& entry    = equipment.value()[e];
        auto const* coll_tag = tag_by_id(c, from_le(entry.item.tag_id));
        if(!coll_tag || !coll_tag->matches(blam::tag_class_t::itmc))
            continue;
        auto coll =
            coll_tag->template data<blam::scn::item_collection>(c.magic);
        if(coll.has_error())
            continue;
        auto items = coll.value()->items.data(c.magic);
        if(items.has_error() || items.value().empty())
            continue;
        Vecf3 const pos{
            from_le(entry.pos.x), from_le(entry.pos.y), from_le(entry.pos.z)};
        place_object(
            c,
            from_le(items.value()[0].item.tag_id),
            spawn_transform(pos, {from_le(entry.facing), 0.f, 0.f}),
            kind_netgame_item,
            net_id_of(kind_netgame_item, e));
    }
}

/* The unit players spawn as (globals: multiplayer, else singleplayer) */
template<typename V>
static u32 player_biped_id(blam::map_container<V> const& c)
{
    u32 const   cnt = from_le(c.tags->tag_count);
    auto const* tg  = c.tags->tags(c.map);
    for(u32 i = 0; i < cnt; i++)
    {
        if(!tg[i].matches(blam::tag_class_t::matg))
            continue;
        auto g = tg[i].template data<blam::globals::globals>(c.magic);
        if(g.has_error())
            return 0xFFFFFFFF;
        if(auto mp = g.value()->multiplayer.data(c.magic);
           !mp.has_error() && !mp.value().empty())
            return from_le(mp.value()[0].unit.tag_id);
        if(auto sp = g.value()->player.data(c.magic);
           !sp.has_error() && !sp.value().empty())
            return from_le(sp.value()[0].unit.tag_id);
        return 0xFFFFFFFF;
    }
    return 0xFFFFFFFF;
}

template<typename V>
static void build_objects(
    blam::map_container<V> const& c, blam::scn::scenario<V> const* scn)
{
    auto const& o = scn->objects;
    place_group(c, o.scenery, kind_scenery);
    place_group(c, o.bipeds, kind_biped);
    place_group(c, o.vehicles, kind_vehicle);
    place_group(c, o.equips, kind_equipment);
    place_group(c, o.weapon_spawns, kind_weapon);
    place_group(c, o.machines, kind_machine);
    place_group(c, o.controls, kind_control);
    place_group(c, o.light_fixtures, kind_light_fixture);
    place_netgame_items(c, scn);

    if(u32 const biped = player_biped_id(c); biped != 0xFFFFFFFF)
        if(auto locs = scn->player_start.locations.data(c.magic);
           !locs.has_error())
        {
            u32 n = 0;
            for(auto const& l : locs.value())
            {
                if(n == kSpawnBipeds)
                    break;
                if(from_le(l.bsp_index) != static_cast<u16>(g_spawn_bsp))
                    continue;
                Vecf3 const pos{
                    from_le(l.pos.x), from_le(l.pos.y), from_le(l.pos.z)};
                Matf4 const m =
                    spawn_transform(pos, {from_le(l.rot), 0.f, 0.f});
                if(place_object(c, biped, m, kind_biped))
                    n++;
            }
        }
    if(u32 const   biped = player_biped_id(c);
       auto const* t     = tag_by_id(c, biped))
        if(auto unit = t->template data<blam::scn::object>(c.magic);
           !unit.has_error())
        {
            g_player_model = from_le(unit.value()->model.tag_id);
            if(g_models.find(g_player_model) == g_models.end() &&
               tag_by_id(c, g_player_model))
                load_model_cpu(c, g_player_model);
        }

    u32 verts = 0;
    for(auto const& [id, m] : g_models)
        for(auto const& p : m.parts)
            verts += static_cast<u32>(p.verts.size());
    for(u32 k = 0; k < kind_count; k++)
        if(g_kind_count[k])
            LOG("objects: %u %s", g_kind_count[k], g_kind_names[k]);
    LOG("objects: %u placed, %u models, %u verts (%u KiB as GX vertices)",
        static_cast<unsigned>(g_objects.size()),
        static_cast<unsigned>(g_models.size()),
        verts,
        static_cast<unsigned>(verts * sizeof(mdl_vertex) / 1024));
}

/* --- geometry ------------------------------------------------------------- */

template<typename V>
static void append_material(
    blam::map_container<V> const& c,
    page_t&                       page,
    blam::bsp::material const&    mat,
    blam::bsp::header const&      hdr,
    blam::bsp_ptr const&          bm)
{
    namespace vert = blam::vert;
    auto verts     = mat.vertices().data(bm);
    auto lverts    = mat.light_verts().data(bm);
    if(verts.has_error() || lverts.has_error())
        return;
    bool const compressed =
        from_le(mat.pc.type) != vert::vertex_type_t::sbsp_uncompressed_vertex;
    u32 const vsz = compressed ? sizeof(vert::vertex<vert::compressed>)
                               : sizeof(vert::vertex<vert::uncompressed>);
    u32 const lsz = compressed ? sizeof(vert::light_vertex<vert::compressed>)
                               : sizeof(vert::light_vertex<vert::uncompressed>);
    // Diffuse UV follows position + 3 normals (packed u32 or f32x3 each)
    u32 const uv_off = compressed ? 24 : 48;

    auto      vspan  = verts.value();
    auto      lspan  = lverts.value();
    u32 const nverts = static_cast<u32>(vspan.size_bytes() / vsz);
    u32 const nlight = static_cast<u32>(lspan.size_bytes() / lsz);
    if(nverts == 0 || nlight < nverts)
        return;

    shader_info const si = resolve_shader(c, mat.shader);

    Vecf3     mn{1e9f, 1e9f, 1e9f}, mx{-1e9f, -1e9f, -1e9f};
    u16 const vert_base = static_cast<u16>(page.verts.size());
    for(u32 i = 0; i < nverts; i++)
    {
        u8 const*   pv = vspan.data() + i * vsz;
        u8 const*   lv = lspan.data() + i * lsz;
        Vecf3 const p{rd<f32>(pv), rd<f32>(pv + 4), rd<f32>(pv + 8)};
        f32 const   du = rd<f32>(pv + uv_off);
        f32 const   dv = rd<f32>(pv + uv_off + 4);
        gx_vertex   gv;
        gv.pos[0] = p.x;
        gv.pos[1] = p.y;
        gv.pos[2] = p.z;
        gv.duv[0] = du;
        gv.duv[1] = dv;
        if(compressed) // normal u32, then i16x2 normalized
        {
            gv.luv[0] = rd<i16>(lv + 4) / 32767.f;
            gv.luv[1] = rd<i16>(lv + 6) / 32767.f;
        } else // normal f32x3, then f32x2
        {
            gv.luv[0] = rd<f32>(lv + 12);
            gv.luv[1] = rd<f32>(lv + 16);
        }
        gv.detuv[0] = du * si.detail_scale; // detail tiles at a higher rate
        gv.detuv[1] = dv * si.detail_scale;
        page.verts.push_back(gv);
        g_bb_min = glm::min(g_bb_min, p);
        g_bb_max = glm::max(g_bb_max, p);
        mn       = glm::min(mn, p);
        mx       = glm::max(mx, p);
    }

    auto faces = mat.indices(hdr).data(bm);
    if(faces.has_error())
        return;
    u32 const idx_start = static_cast<u32>(page.indices.size());
    for(auto const& f : faces.value())
    {
        page.indices.push_back(vert_base + from_le(f.x));
        page.indices.push_back(vert_base + from_le(f.y));
        page.indices.push_back(vert_base + from_le(f.z));
    }
    u32 const idx_count = static_cast<u32>(page.indices.size()) - idx_start;
    if(idx_count)
    {
        Vecf3 const center = (mn + mx) * 0.5f;
        f32 const   radius = glm::length(mx - center);
        page.submeshes.push_back(
            {idx_start, idx_count, center, radius, si.diffuse, si.detail});
    }
}

/* BlamGraphics' MapFingerprint: FNV-1a 64 over every tag's classes, id and
 * name as stored, which a server checks before trusting our tag ids */
struct map_fingerprint
{
    u32 tag_hash[2]{};
    u32 tag_count{0};
    u32 base_tag{0};
};

static map_fingerprint g_fingerprint;

template<typename V>
static map_fingerprint fingerprint_of(blam::map_container<V> const& c)
{
    u64  hash   = 0xcbf29ce484222325;
    auto update = [&hash](void const* data, size_t size) {
        auto const* bytes = static_cast<u8 const*>(data);
        for(size_t i = 0; i < size; ++i)
            hash = (hash ^ bytes[i]) * 0x100000001b3;
    };
    map_fingerprint out;
    u32 const       cnt = from_le(c.tags->tag_count);
    auto const*     tg  = c.tags->tags(c.map);
    for(u32 i = 0; i < cnt; i++)
    {
        update(tg[i].tagclass_e.data(), sizeof(tg[i].tagclass_e));
        update(&tg[i].tag_id, sizeof(tg[i].tag_id));
        u32 const              name = from_le(tg[i].name.offset);
        std::string_view const str =
            name ? std::string_view(
                       reinterpret_cast<char const*>(
                           c.magic.base_ptr + name - c.magic.file_offset))
                 : std::string_view("[empty string]");
        update(str.data(), str.size());
        ++out.tag_count;
    }
    out.tag_hash[0] = static_cast<u32>(hash);
    out.tag_hash[1] = static_cast<u32>(hash >> 32);
    out.base_tag    = from_le(c.tags->base_tag);
    return out;
}

template<typename V>
static int extract_map(semantic::BytesConst const& bytes)
{
    auto const* fh = reinterpret_cast<blam::file_header_t const*>(bytes.data);
    stage_hot_range(
        "tag data",
        from_le(fh->tag_index_offset),
        static_cast<u32>(bytes.size) - from_le(fh->tag_index_offset));

    auto map = blam::map_container<V>::from_bytes(
        bytes, V{}, [](std::string_view, libc_types::i16) {});
    if(map.has_error())
    {
        g_err = static_cast<int>(map.error());
        LOG("map parse failed, error=%d", g_err);
        return 2;
    }
    auto const& container = map.value();
    g_internal_magic      = container.magic.ptr_only();
    g_fingerprint         = fingerprint_of(container);
    LOG("fingerprint %08x%08x/%u/%08x",
        g_fingerprint.tag_hash[1],
        g_fingerprint.tag_hash[0],
        g_fingerprint.tag_count,
        g_fingerprint.base_tag);

    auto scn = container.tags->scenario(container.map, container.magic);
    if(!scn)
        return 3;

    // Grab the first player spawn for the camera.
    if(auto locs = (*scn)->player_start.locations.data(container.magic);
       !locs.has_error() && !locs.value().empty())
    {
        auto const& l = locs.value()[0];
        g_spawn_pos   = {from_le(l.pos.x), from_le(l.pos.y), from_le(l.pos.z)};
        g_spawn_rot   = from_le(l.rot);
        g_spawn_bsp   = from_le(l.bsp_index);
        g_have_spawn  = true;
        LOG("spawn at %d,%d,%d rot=%d",
            (int)g_spawn_pos.x,
            (int)g_spawn_pos.y,
            (int)g_spawn_pos.z,
            (int)(g_spawn_rot * 57.3f));
    }

    auto bsps = (*scn)->bsp_info.data(container.magic);
    if(bsps.has_error())
        return 4;
    g_bsp_count = static_cast<u32>(bsps.value().size());
    LOG("bsp_info count=%u spawn_bsp=%d", g_bsp_count, (int)g_spawn_bsp);

    // Only the spawn's structure BSP -- campaign maps have several BSPs that
    // overlap in world space (Halo loads one at a time), and rendering them all
    // would also blow the 24MB budget.
    u32 bsp_idx = 0;
    for(auto const& bi : bsps.value())
    {
        if(bsp_idx++ != static_cast<u32>(g_spawn_bsp))
            continue;
        stage_hot_range("bsp", from_le(bi.offset), from_le(bi.size));
        auto const bm = bi.bsp_magic(container.magic);
        auto hdr_r    = bi.to_bsp(bm).to_header().data(bm, blam::single_value);
        if(hdr_r.has_error())
            continue;
        auto const* hdr = hdr_r.value();
        build_cluster_lookup(*hdr, bm);
        LOG("cluster lookup: %u nodes, %u leaves",
            static_cast<unsigned>(g_coll_nodes.size()),
            static_cast<unsigned>(g_leaf_cluster.size()));
        auto const* bitm = resolve_bitm(container, hdr->lightmap_);

        auto lms = hdr->lightmaps.data(bm);
        if(lms.has_error())
            continue;
        for(auto const& lm : lms.value())
        {
            auto mats = lm.materials.data(bm);
            if(mats.has_error())
                continue;

            g_pages.emplace_back();
            g_page_bitmaps.emplace_back();
            auto& page  = g_pages.back();
            page.lm_idx = from_le(lm.lightmap_idx);

            for(auto const& mat : mats.value())
                append_material(container, page, mat, *hdr, bm);

            if(bitm && page.lm_idx >= 0)
            {
                auto imgs = bitm->images.data(container.magic);
                if(!imgs.has_error() &&
                   static_cast<u32>(page.lm_idx) < imgs.value().size())
                {
                    g_page_bitmaps.back().image = &imgs.value()[page.lm_idx];
                    g_page_bitmaps.back().pix_magic = g_internal_magic;
                }
            }
        }
    }

    LOG("pages=%u", static_cast<unsigned>(g_pages.size()));

    build_objects(container, *scn);

    return g_pages.empty() ? 5 : 0;
}

static int extract_bsp()
{
    LOG("loading map bytes...");
    auto bytes = load_map_bytes();
    LOG("map bytes = %u", static_cast<unsigned>(bytes.size));
    if(bytes.size < sizeof(blam::file_header_t))
        return 1;

    auto const* fh = reinterpret_cast<blam::file_header_t const*>(bytes.data);
    auto const  version = from_le(fh->version);
    if(version == blam::version_t::xbox)
    {
        // Compressed Xbox maps would need inflating into RAM; the transcoder
        // writes them out inflated
        if(from_le(fh->decomp_len) != bytes.size)
        {
            LOG("xbox map is compressed, run it through MapTranscode");
            return 2;
        }
        g_xbox = true;
        LOG("xbox map");
        return extract_map<blam::xbox_version_t>(bytes);
    }
    LOG("pc map");
    return extract_map<blam::pc_version_t>(bytes);
}

#if defined(FEATURE_ENABLE_Gexxo)
/* --- texture decode (kept-alive tiled buffers + diffuse dedup) -------------
 */

u32                       g_lm_bytes  = 0;
u32                       g_dxt_bytes = 0;
static std::vector<void*> g_tiled_bufs;

// Cap total texture memory so a big map (e.g. b30) degrades to fewer textures
// instead of OOMing; the rest fall back to lightmap/untextured.
static u32 constexpr kTexBudget = 9u * 1024 * 1024;
static u32 g_tex_used           = 0;

// Drop the top N mip levels of offline-transcoded (native) textures at upload:
// GX then samples from a lower-resolution base, saving texture memory (the base
// level is ~3/4 of a mip chain). 0 = full res.
static u32 constexpr g_mip_skip = 1;

// GX tiled byte size of one mip level at w x h for a GX_TF_* format (mirrors
// the transcoder's mtx::tiled_size, keyed on the GX format constant).
static u32 gx_level_bytes(u8 gxfmt, u16 w, u16 h)
{
    auto up = [](u16 v, u16 a) -> u16 {
        return static_cast<u16>((v + (a - 1)) & ~(a - 1));
    };
    switch(gxfmt)
    {
    case GX_TF_CMPR:
        return static_cast<u32>(up(w, 8)) * up(h, 8) / 2;
    case GX_TF_RGB565:
    case GX_TF_IA8:
        return static_cast<u32>(up(w, 4)) * up(h, 4) * 2;
    case GX_TF_I8:
        return static_cast<u32>(up(w, 8)) * up(h, 4);
    }
    return 0;
}

static std::map<blam::bitm::image_t const*, std::shared_ptr<gexxo::texture_t>>
    g_diffuse_cache;

/* Tile a linear RGB565 image into the GX 4x4 layout. swap=true byte-swaps each
 * texel (for little-endian map data); swap=false stores host order (for values
 * computed on the big-endian CPU). */
static void tile_rgb565(u16 const* src, u16* dst, u16 w, u16 h, bool swap)
{
    u32 o = 0;
    for(u16 ty = 0; ty < h; ty += 4)
        for(u16 tx = 0; tx < w; tx += 4)
            for(u16 ry = 0; ry < 4; ry++)
                for(u16 rx = 0; rx < 4; rx++)
                {
                    u16 const x = tx + rx, y = ty + ry;
                    u16 const t = (x < w && y < h) ? src[y * w + x] : 0;
                    dst[o++]    = swap ? __builtin_bswap16(t) : t;
                }
}

/* Re-tile a linear S3TC image's DXT1 colour block into GX_TF_CMPR (same 0.5
 * B/px size, 4x smaller than expanding to RGB565): 4x4 blocks grouped into 8x8
 * tiles (TL,TR,BL,BR), 16-bit endpoints byte-swapped to big-endian, 2-bit index
 * groups reversed per byte. Handles BC1 (block 8, colour@0) and BC2/BC3 (block
 * 16, colour@8; alpha dropped). */
static void dxt_to_cmpr(
    u8 const* src, u8* dst, u16 w, u16 h, u32 block_size, u32 color_off)
{
    u32 const bw = (w + 3) / 4, bh = (h + 3) / 4;
    u32       o   = 0;
    auto      rev = [](u8 v) -> u8 {
        return ((v & 0x03) << 6) | ((v & 0x0C) << 2) | ((v & 0x30) >> 2) |
               ((v & 0xC0) >> 6);
    };
    for(u32 ty = 0; ty < bh; ty += 2)
        for(u32 tx = 0; tx < bw; tx += 2)
            for(u32 sy = 0; sy < 2; sy++)
                for(u32 sx = 0; sx < 2; sx++)
                {
                    u32 const bx = tx + sx, by = ty + sy;
                    u8        b[8] = {0};
                    if(bx < bw && by < bh)
                        std::memcpy(
                            b,
                            src + (by * bw + bx) * block_size + color_off,
                            8);
                    dst[o++] = b[1]; // endpoint0 -> BE
                    dst[o++] = b[0];
                    dst[o++] = b[3]; // endpoint1 -> BE
                    dst[o++] = b[2];
                    dst[o++] = rev(b[4]); // index rows, columns reversed
                    dst[o++] = rev(b[5]);
                    dst[o++] = rev(b[6]);
                    dst[o++] = rev(b[7]);
                }
}

static bool read_bitmap_pixels(
    blam::bitm::image_t const&   img,
    blam::map_ptr const&         internal_magic,
    std::vector<libc_types::u8>& out)
{
    u32 const  size = from_le(img.size);
    bool const shared =
        !g_xbox && (static_cast<u16>(from_le(img.flags)) &
                    static_cast<u16>(blam::bitm::flags_t::shared)) != 0;
    out.resize(size);
#if defined(COFFEE_GEKKO)
    if(shared)
    {
        static u32 const bmap_off = [] {
            auto const* e =
                platform::file::gekko::dvd::detail::find("bitmaps.map");
            return e ? e->off : 0u;
        }();
        if(!bmap_off)
            return false;
        return platform::file::gekko::dvd::detail::read_at(
            out.data(), size, bmap_off + from_le(img.offset));
    }
#endif
    auto pix =
        blam::reference<libc_types::u8>{.count = img.size, .offset = img.offset}
            .data(internal_magic);
    if(pix.has_error())
        return false;
    std::copy(pix.value().begin(), pix.value().end(), out.begin());
    return true;
}

static bool is_gx_native(blam::bitm::format_t fmt)
{
    return (static_cast<u16>(fmt) &
            static_cast<u16>(blam::bitm::format_t::PLATFORM_SPECIFIC_MASK)) !=
           0;
}

/* Linear RGB565 (little-endian) out of an uncompressed source: deswizzles
 * Xbox images and narrows 32-bit ones (stored B,G,R,A). */
static bool to_linear_rgb565(
    blam::bitm::image_t const&   img,
    std::vector<libc_types::u8>& px,
    u16                          w,
    u16                          h)
{
    using fmt_t    = blam::bitm::format_t;
    auto const fmt = from_le(img.format);
    u32 const  bpp = fmt == fmt_t::R5G6B5 ? 2 : 4;
    u32 const  n   = static_cast<u32>(w) * h;
    if(px.size() < n * bpp)
        return false;
    if(static_cast<u16>(from_le(img.flags)) &
       static_cast<u16>(blam::bitm::flags_t::swizzled))
    {
        std::vector<libc_types::u8> lin(n * bpp);
        if(!blam::swizzle::deswizzle_bytes(
               semantic::Span<const u8>(px.data(), n * bpp),
               semantic::Span<u8>(lin.data(), lin.size()),
               w,
               h,
               bpp))
            return false;
        px = std::move(lin);
    }
    if(bpp == 4)
    {
        for(u32 i = 0; i < n; i++)
        {
            u8 const* s = px.data() + i * 4;
            u16 const c = static_cast<u16>(
                ((s[2] >> 3) << 11) | ((s[1] >> 2) << 5) | (s[0] >> 3));
            px[i * 2]     = c & 0xFF; // keep it little-endian like R5G6B5
            px[i * 2 + 1] = c >> 8;
        }
        px.resize(n * 2);
    }
    return true;
}

/* Decode a bitmap image (RGB565 or DXT1) into a GX texture. wrap = GX_CLAMP for
 * [0,1] lightmaps, GX_REPEAT for tiling diffuse maps. */
static std::shared_ptr<gexxo::texture_t> load_texture(
    blam::bitm::image_t const& img,
    blam::map_ptr const&       magic,
    u8                         wrap = GX_CLAMP)
{
    auto const fmt = from_le(img.format);
    i16 const  w   = from_le(img.isize.x);
    i16 const  h   = from_le(img.isize.y);
    if(w <= 0 || h <= 0)
        return nullptr;

    // Offline-transcoded (map-transcode tool): pixel data is already in a GX
    // tiled layout -> upload directly, no decode/re-tile.
    if(is_gx_native(fmt))
    {
        u8 gxfmt;
        switch(fmt)
        {
        case blam::bitm::format_t::BC1_GX_TILED:
            gxfmt = GX_TF_CMPR;
            break;
        case blam::bitm::format_t::RGB565_GX_TILED:
            gxfmt = GX_TF_RGB565;
            break;
        case blam::bitm::format_t::I8_GX_TILED:
            gxfmt = GX_TF_I8;
            break;
        case blam::bitm::format_t::IA8_GX_TILED:
            gxfmt = GX_TF_IA8;
            break;
        default:
            return nullptr;
        }
        u32 const total = from_le(img.size);
        if(!total)
            return nullptr;
        // img.mipmaps = GX maxlod; pixel data holds levels 0..maxlod packed
        // consecutively. Skip the top g_mip_skip levels (keep >= 1) so GX's
        // base is a lower-res mip: advance past their tiled bytes + halve the
        // dims.
        u32 const maxlod = from_le(img.mipmaps);
        u32 const skip   = std::min<u32>(g_mip_skip, maxlod);
        u16       bw = w, bh = h;
        u32       off = 0;
        for(u32 k = 0; k < skip; k++)
        {
            off += gx_level_bytes(gxfmt, bw, bh);
            bw = static_cast<u16>(bw > 1 ? bw >> 1 : 1);
            bh = static_cast<u16>(bh > 1 ? bh >> 1 : 1);
        }
        u32 const tbytes = (off <= total) ? (total - off) : 0;
        if(!tbytes || g_tex_used + tbytes > kTexBudget)
            return nullptr;
        std::vector<libc_types::u8> pixels;
        if(!read_bitmap_pixels(img, magic, pixels) || pixels.size() < total)
            return nullptr;
        void* tiled = memalign(32, (tbytes + 31) & ~31u);
        if(!tiled)
            return nullptr;
        g_tex_used += tbytes;
        (gxfmt == GX_TF_RGB565 ? g_lm_bytes : g_dxt_bytes) += tbytes;
        std::memcpy(tiled, pixels.data() + off, tbytes);
        DCFlushRange(tiled, tbytes);
        g_tiled_bufs.push_back(tiled);
        auto tex = std::make_shared<gexxo::texture_t>(
            gexxo::textures::type::d2, typing::pixels::PixDesc{}, 1);
        // Transcoded Xbox levels under one 8x8 CMPR tile come out wrong
        u32 lod = maxlod - skip;
        while(lod && std::min(bw >> lod, bh >> lod) < 8)
            lod--;
        tex->init_raw(tiled, bw, bh, gxfmt, wrap, wrap, static_cast<u8>(lod));
        return tex;
    }

    using fmt_t = blam::bitm::format_t;
    bool const is_bcn =
        fmt == fmt_t::BC1 || fmt == fmt_t::BC2 || fmt == fmt_t::BC3;
    bool const is_565 =
        fmt == fmt_t::R5G6B5 || fmt == fmt_t::XRGB8 || fmt == fmt_t::ARGB8;
    if(!is_565 && !is_bcn)
        return nullptr; // palettised/intensity need the transcoder

    std::vector<libc_types::u8> pixels;
    if(!read_bitmap_pixels(img, magic, pixels))
        return nullptr;
    if(is_565 && !to_linear_rgb565(img, pixels, w, h))
        return nullptr;

    // Lightmaps (RGB565 source) tile 4x4 -> GX_TF_RGB565 (2 B/px);
    // diffuse/detail (DXT source) re-tile -> GX_TF_CMPR (0.5 B/px, 4x smaller
    // than expanding).
    u16 const tw     = is_565 ? ((w + 3) & ~3) : ((w + 7) & ~7);
    u16 const th     = is_565 ? ((h + 3) & ~3) : ((h + 7) & ~7);
    u32 const tbytes = is_565 ? (static_cast<u32>(tw) * th * 2)
                              : (static_cast<u32>(tw) * th / 2);
    u8 const  gxfmt  = is_565 ? GX_TF_RGB565 : GX_TF_CMPR;

    if(g_tex_used + tbytes > kTexBudget)
        return nullptr;
    void* tiled = memalign(32, (tbytes + 31) & ~31u);
    if(!tiled)
        return nullptr;
    g_tex_used += tbytes;
    (is_565 ? g_lm_bytes : g_dxt_bytes) += tbytes;

    if(is_565)
    {
        // Map RGB565 is little-endian -> swap to big-endian for GX.
        tile_rgb565(
            reinterpret_cast<u16 const*>(pixels.data()),
            reinterpret_cast<u16*>(tiled),
            w,
            h,
            true);
    } else // BC1/BC2/BC3 -> GX_TF_CMPR (re-tile the DXT1 colour block)
    {
        bool const bc1 = fmt == fmt_t::BC1;
        dxt_to_cmpr(
            pixels.data(),
            reinterpret_cast<u8*>(tiled),
            w,
            h,
            bc1 ? 8 : 16,
            bc1 ? 0 : 8);
    }
    DCFlushRange(tiled, tbytes);
    g_tiled_bufs.push_back(tiled);

    auto tex = std::make_shared<gexxo::texture_t>(
        gexxo::textures::type::d2, typing::pixels::PixDesc{}, 1);
    tex->init_raw(tiled, w, h, gxfmt, wrap, wrap);
    return tex;
}

static std::shared_ptr<gexxo::texture_t> load_diffuse(
    blam::bitm::image_t const* img)
{
    if(!img)
        return nullptr;
    auto it = g_diffuse_cache.find(img);
    if(it != g_diffuse_cache.end())
        return it->second;
    auto tex             = load_texture(*img, g_internal_magic, GX_REPEAT);
    g_diffuse_cache[img] = tex;
    return tex;
}

// Fly-cam state (Halo world space, Z up), driven by the GameCube pad.
static bool  g_cam_ready = false;
static Vecf3 g_cam_pos{};
static f32   g_cam_yaw   = 0.f; // around +Z
static f32   g_cam_pitch = 0.f; // up/down

/* View-frustum culling: 6 planes from the view-projection matrix (normalized),
 * sphere-vs-frustum test. Matches the gexxo-fixed 90deg/4:3/0.1-100 projection.
 */
struct frustum_t
{
    Vecf4 planes[6];
};

static frustum_t make_frustum(Matf4 const& m)
{
    auto row = [&](int i) { return Vecf4(m[0][i], m[1][i], m[2][i], m[3][i]); };
    frustum_t f;
    f.planes[0] = row(3) + row(0); // left
    f.planes[1] = row(3) - row(0); // right
    f.planes[2] = row(3) + row(1); // bottom
    f.planes[3] = row(3) - row(1); // top
    f.planes[4] = row(3) + row(2); // near
    f.planes[5] = row(3) - row(2); // far
    for(auto& p : f.planes)
        p /= glm::length(Vecf3(p));
    return f;
}

static bool sphere_visible(frustum_t const& f, Vecf3 const& c, f32 r)
{
    for(auto const& p : f.planes)
        if(p.x * c.x + p.y * c.y + p.z * c.z + p.w < -r)
            return false;
    return true;
}

static Matf4 const g_proj =
    glm::perspective(glm::radians(90.f), 4.f / 3.f, 0.1f, 100.f);

static Vecf3 cam_forward()
{
    f32 const cp = std::cos(g_cam_pitch);
    return Vecf3{
        std::cos(g_cam_yaw) * cp,
        std::sin(g_cam_yaw) * cp,
        std::sin(g_cam_pitch)};
}

/* Update the fly-cam from the controller: left stick = move/strafe, C-stick =
 * look, A/B = up/down, R = speed boost. dt in seconds. No-op without a pad
 * (headless), so the camera just sits at the spawn. */
static void update_camera(f32 dt)
{
    if(!g_cam_ready)
    {
        // Behind and above the first spawn, so its biped is in view
        g_cam_yaw   = g_spawn_rot;
        g_cam_pitch = -0.25f;
        g_cam_pos   = g_spawn_pos - cam_forward() * 1.5f + Vecf3{0, 0, 0.9f};
        g_cam_ready = true;
    }
#if defined(COFFEE_GEKKO)
    PAD_ScanPads();
    auto dz = [](s8 v) { // -127..127 -> -1..1 with a deadzone
        return std::abs(static_cast<int>(v)) < 15 ? 0.f : v / 127.f;
    };
    f32 const lx = dz(PAD_StickX(0)), ly = dz(PAD_StickY(0));
    f32 const cx = dz(PAD_SubStickX(0)), cy = dz(PAD_SubStickY(0));

    g_cam_yaw -= cx * 2.5f * dt; // look
    g_cam_pitch -= cy * 2.0f * dt;
    g_cam_pitch = std::clamp(g_cam_pitch, -1.45f, 1.45f);

    u16 const btn   = PAD_ButtonsHeld(0);
    f32 const boost = 1.f + (PAD_TriggerR(0) / 255.f) * 4.f;
    f32 const speed = 8.f * dt * boost; // world units/sec

    Vecf3 const fwd   = cam_forward();
    Vecf3 const right = glm::normalize(glm::cross(fwd, Vecf3{0.f, 0.f, 1.f}));
    g_cam_pos += fwd * (ly * speed) + right * (lx * speed);
    if(btn & PAD_BUTTON_A)
        g_cam_pos.z += speed;
    if(btn & PAD_BUTTON_B)
        g_cam_pos.z -= speed;
#else
    (void)dt;
#endif
}

static Matf4 make_modelview(f32 spin)
{
    // First-person fly-cam from the player spawn (Halo world space is Z-up;
    // world units feed the fixed 90deg/0.1-100 GX projection directly).
    if(g_have_spawn || g_cam_ready)
    {
        Vecf3 const fwd = cam_forward();
        return glm::lookAt(g_cam_pos, g_cam_pos + fwd, Vecf3{0.f, 0.f, 1.f});
    }
    (void)spin;

    // Fallback: orbit the whole BSP scaled to fit.
    Vecf3 const center = (g_bb_min + g_bb_max) * 0.5f;
    Vecf3 const ext    = g_bb_max - g_bb_min;
    f32 const   radius = std::max({ext.x, ext.y, ext.z, 1.f}) * 0.5f;
    f32 const   scale  = 18.f / radius;
    Matf4       m{1.f};
    m = glm::translate(m, Vecf3{0.f, 0.f, -45.f});
    m = glm::rotate(m, glm::radians(-55.f), Vecf3{1.f, 0.f, 0.f});
    m = glm::rotate(m, spin, Vecf3{0.f, 0.f, 1.f});
    m = glm::scale(m, Vecf3{scale});
    m = glm::translate(m, -center);
    return m;
}

/* One directional sun for the objects, in eye space (GX lights are) */
static void load_sun(Matf4 const& view)
{
    Vecf3 const dir = glm::normalize(Vecf3{-0.4f, -0.3f, -0.85f});
    Vecf3 const eye = glm::mat3(view) * dir;
    GXLightObj  sun;
    // A far point light is how GX does a directional one
    GX_InitLightPos(&sun, -eye.x * 1e5f, -eye.y * 1e5f, -eye.z * 1e5f);
    GX_InitLightColor(&sun, GXColor{0xC8, 0xC4, 0xB8, 0xFF});
    GX_LoadLightObj(&sun, GX_LIGHT0);
    GX_SetChanAmbColor(GX_COLOR0A0, GXColor{0x58, 0x58, 0x60, 0xFF});
}
#endif

#if defined(COFFEE_GEKKO)
/* --- network play through BlamGCGateway ----------------------------------- */

static gx_net::client_t      g_net;
static std::map<u32, size_t> g_net_objects;    // net id -> g_objects
static std::map<u32, size_t> g_remote_players; // player idx -> g_objects
static constexpr f32         kEyeHeight = 0.62f;

/* gateway.txt on the disc: "gateway-ip[:port] [server-ip:port]", no file =
 * no networking */
static void start_network()
{
    namespace dvd = platform::file::gekko::dvd;
    auto const* e = dvd::detail::find("gateway.txt");
    if(!e || !e->size || e->size > 256)
        return;
    std::string cfg(e->size, '\0');
    if(!dvd::detail::read_at(cfg.data(), e->size, e->off))
        return;
    while(!cfg.empty() && std::isspace(static_cast<u8>(cfg.back())))
        cfg.pop_back();
    auto const  space   = cfg.find(' ');
    std::string gateway = cfg.substr(0, space);
    std::string server =
        space == std::string::npos ? std::string() : cfg.substr(space + 1);
    for(size_t i = 0; i < g_objects.size(); i++)
        if(g_objects[i].net_id)
            g_net_objects[g_objects[i].net_id] = i;
    if(!g_net.start(gateway, server))
        LOG("network play unavailable");
}

static void place_instance(object_inst& inst, Matf4 const& xform)
{
    inst.xform  = xform;
    auto it     = g_models.find(inst.model_id);
    inst.center = Vecf3(
        xform * Vecf4(it != g_models.end() ? it->second.center : Vecf3{}, 1.f));
}

static void poll_network(u32 frame)
{
    gx_net::fingerprint_t const fp{
        {g_fingerprint.tag_hash[0], g_fingerprint.tag_hash[1]},
        g_fingerprint.tag_count,
        g_fingerprint.base_tag};
    gx_net::handlers_t on;
    on.object = [](gx_net::object_sync_t const& o) {
        auto it = g_net_objects.find(o.net_id);
        if(it == g_net_objects.end())
            return;
        place_instance(
            g_objects[it->second],
            glm::translate(Matf4{1.f}, o.position) *
                glm::mat4_cast(o.rotation));
    };
    on.spawn = [](gx_net::entity_spawn_t const& e) {
        auto it = g_net_objects.find(e.net_id);
        if(it == g_net_objects.end())
        {
            // The map is unmapped by now, so nothing new can be loaded
            LOG("net: no model for spawned object %08x (tag %08x)",
                e.net_id,
                e.tag_id);
            return;
        }
        place_instance(
            g_objects[it->second],
            glm::translate(Matf4{1.f}, e.position) *
                glm::mat4_cast(e.rotation));
    };
    on.camera = [](gx_net::camera_sync_t const& c) {
        if(g_models.find(g_player_model) == g_models.end())
            return;
        auto it = g_remote_players.find(c.player_idx);
        if(it == g_remote_players.end())
        {
            LOG("net: player %u is in the game", c.player_idx);
            g_objects.push_back(
                {g_player_model,
                 Matf4{1.f},
                 {},
                 g_models[g_player_model].radius});
            it = g_remote_players.emplace(c.player_idx, g_objects.size() - 1)
                     .first;
        }
        f32 const yaw = std::atan2(c.forward.y, c.forward.x);
        place_instance(
            g_objects[it->second],
            glm::rotate(
                glm::translate(
                    Matf4{1.f}, c.position - Vecf3{0.f, 0.f, kEyeHeight}),
                yaw,
                Vecf3{0.f, 0.f, 1.f}));
    };
    on.teleport = [](gx_net::camera_sync_t const& c) {
        LOG("net: server put us at %d,%d,%d",
            (int)c.position.x,
            (int)c.position.y,
            (int)c.position.z);
        g_cam_pos   = c.position;
        g_cam_yaw   = std::atan2(c.forward.y, c.forward.x);
        g_cam_pitch = std::asin(std::clamp(c.forward.z, -1.f, 1.f));
    };
    on.player_spawn = [](Vecf3 const& pos, f32 facing) {
        LOG("net: spawned at %d,%d,%d", (int)pos.x, (int)pos.y, (int)pos.z);
        g_cam_pos   = pos + Vecf3{0.f, 0.f, kEyeHeight};
        g_cam_yaw   = facing;
        g_cam_pitch = 0.f;
    };
    g_net.poll(fp, on, frame);
    if(frame % 2 == 0)
        g_net.send_camera(g_cam_pos, cam_forward());
}
#endif

i32 coffee_main(i32, cstring_w*)
{
#if defined(FEATURE_ENABLE_Gexxo)
    gexxo::api gfx;
    gfx.load({});
    auto rt       = gfx.default_rendertarget();
    auto free_mem = [] {
        return static_cast<u32>(
            reinterpret_cast<uintptr_t>(SYS_GetArena1Hi()) -
            reinterpret_cast<uintptr_t>(SYS_GetArena1Lo()));
    };
    LOG("mem free at start: %u KiB", free_mem() / 1024);
#endif

    LOG("start");
#if defined(FEATURE_ENABLE_Gexxo)
    u64 const load_start = gettime();
#endif
    int stage = 6;
    try
    {
        stage = extract_bsp();
    } catch(std::exception const& e)
    {
        LOG("exception: %s", e.what());
        stage = 6;
    }
    LOG("done stage=%d err=%d", stage, g_err);
#if defined(COFFEE_GEKKO)
    LOG("map pages faulted: %u", platform::file::gekko::vmem::fault_count());
#endif

#if defined(FEATURE_ENABLE_Gexxo)
    static Vecf4 const fail_colors[] = {
        {0.f, 0.7f, 0.f, 1.f},
        {0.7f, 0.f, 0.7f, 1.f},
        {0.7f, 0.f, 0.f, 1.f},
        {0.9f, 0.5f, 0.f, 1.f},
        {0.8f, 0.8f, 0.f, 1.f},
        {0.f, 0.f, 0.8f, 1.f},
        {1.f, 1.f, 1.f, 1.f},
    };
    if(stage != 0)
    {
        for(int i = 0; i < 180; i++)
        {
            rt->clear(fail_colors[stage], 1.f, 0);
            gexxo::swapBuffers();
        }
        return 1;
    }

    // Build GPU resources: per-page buffers + lightmap, per-submesh diffuse.
    u32 diffuse_count = 0;
    u32 mdl_parts     = 0;
    try
    {
        for(u32 pi = 0; pi < g_pages.size(); pi++)
        {
            auto& page = g_pages[pi];
            if(page.verts.empty() || page.indices.empty())
                continue;
            page.vao = gfx.alloc_vertex_array();
            page.vbo = gfx.alloc_buffer(gexxo::buffers::vertex, RSCA::ReadOnly);
            page.ebo =
                gfx.alloc_buffer(gexxo::buffers::element, RSCA::ReadOnly);
            page.vao->alloc();
            page.vbo->alloc();
            page.ebo->alloc();
            page.vbo->commit(
                gsl::span<gx_vertex const>(
                    page.verts.data(), page.verts.size()));
            page.ebo->commit(
                gsl::span<u16 const>(page.indices.data(), page.indices.size()));
            page.vao->add({
                .index = 0,
                .role  = gexxo::vertex_attribute::position,
                .value = {.offset = 0, .stride = sizeof(gx_vertex), .count = 3},
            });
            page.vao->add({
                .index = 1,
                .role  = gexxo::vertex_attribute::texcoord0, // diffuse UV
                .value =
                    {.offset = offsetof(gx_vertex, duv),
                     .stride = sizeof(gx_vertex),
                     .count  = 2},
            });
            page.vao->add({
                .index = 2,
                .role  = static_cast<gexxo::vertex_attribute::role_t>(
                    gexxo::vertex_attribute::texcoord0 + 1), // lightmap UV
                .value =
                    {.offset = offsetof(gx_vertex, luv),
                     .stride = sizeof(gx_vertex),
                     .count  = 2},
            });
            page.vao->add({
                .index = 3,
                .role  = static_cast<gexxo::vertex_attribute::role_t>(
                    gexxo::vertex_attribute::texcoord0 + 2), // detail UV
                .value =
                    {.offset = offsetof(gx_vertex, detuv),
                     .stride = sizeof(gx_vertex),
                     .count  = 2},
            });
            page.vao->set_buffer(gexxo::buffers::vertex, page.vbo, 0);
            page.vao->set_buffer(gexxo::buffers::element, page.ebo);

            page.lightmap = g_page_bitmaps[pi].image
                                ? load_texture(
                                      *g_page_bitmaps[pi].image,
                                      g_page_bitmaps[pi].pix_magic)
                                : nullptr;
            for(auto& sm : page.submeshes)
            {
                sm.diffuse = load_diffuse(sm.diffuse_img);
                sm.detail  = load_diffuse(sm.detail_img);
                if(sm.diffuse)
                    diffuse_count++;
            }
        }
        LOG("gpu: %u submeshes with diffuse, %u unique diffuse tex",
            diffuse_count,
            static_cast<unsigned>(g_diffuse_cache.size()));

        // Model parts -> GPU (position + normal + diffuse UV; strip indices).
        for(auto& [id, model] : g_models)
            for(auto& part : model.parts)
            {
                if(part.verts.empty() || part.indices.size() < 3)
                    continue;
                part.vao = gfx.alloc_vertex_array();
                part.vbo =
                    gfx.alloc_buffer(gexxo::buffers::vertex, RSCA::ReadOnly);
                part.ebo =
                    gfx.alloc_buffer(gexxo::buffers::element, RSCA::ReadOnly);
                part.vao->alloc();
                part.vbo->alloc();
                part.ebo->alloc();
                part.vbo->commit(
                    gsl::span<mdl_vertex const>(
                        part.verts.data(), part.verts.size()));
                part.ebo->commit(
                    gsl::span<u16 const>(
                        part.indices.data(), part.indices.size()));
                // Canonical GX order: POS, NRM, TEX0
                part.vao->add({
                    .index = 0,
                    .role  = gexxo::vertex_attribute::position,
                    .value =
                        {.offset = 0, .stride = sizeof(mdl_vertex), .count = 3},
                });
                part.vao->add({
                    .index = 1,
                    .role  = gexxo::vertex_attribute::normal,
                    .value =
                        {.offset = offsetof(mdl_vertex, nrm),
                         .stride = sizeof(mdl_vertex),
                         .count  = 3,
                         .type   = semantic::type_t::i8,
                         .frac   = 6},
                });
                part.vao->add({
                    .index = 2,
                    .role  = gexxo::vertex_attribute::texcoord0,
                    .value =
                        {.offset = offsetof(mdl_vertex, uv),
                         .stride = sizeof(mdl_vertex),
                         .count  = 2},
                });
                part.vao->set_buffer(gexxo::buffers::vertex, part.vbo, 0);
                part.vao->set_buffer(gexxo::buffers::element, part.ebo);
                part.diffuse = load_diffuse(part.diffuse_img);
                mdl_parts++;
            }
    } catch(std::bad_alloc const&)
    {
        // Out of MEM1: render whatever resources were built (the rest stay
        // null and are simply skipped). Big maps don't fully fit in 24MB.
        LOG("gpu: out of memory, rendering partial scene");
    }
    LOG("gpu: %u object instances, %u model parts",
        static_cast<unsigned>(g_objects.size()),
        mdl_parts);
    LOG("tex mem: lightmap(RGB565)=%u KiB, diffuse/detail(CMPR)=%u KiB",
        g_lm_bytes / 1024,
        g_dxt_bytes / 1024);
    LOG("mem free after load: %u KiB", free_mem() / 1024);

#if defined(COFFEE_GEKKO)
    platform::file::gekko::vmem::unmap(g_map_window);
    LOG("mem free after unmap: %u KiB", free_mem() / 1024);
#endif
    LOG("load took %u ms",
        static_cast<unsigned>(ticks_to_millisecs(gettime() - load_start)));

    using stage_t   = gexxo::program_t::stage_t;
    using channel_t = gexxo::program_t::channel_t;

    // diffuse * lightmap: stage0 = diffuse (tex0@tc0), stage1 = *lightmap
    // (tex1@tc1)
    auto prog_diff_lm      = gfx.alloc_program();
    prog_diff_lm->channels = {channel_t{.channel = channel_t::color0a0}};
    prog_diff_lm->stages   = {
        stage_t{
              .stage    = stage_t::stage0,
              .op       = stage_t::replace,
              .texcoord = stage_t::texcoord0,
              .texmap   = stage_t::texmap0},
        stage_t{
              .stage    = stage_t::stage1,
              .op       = stage_t::modulate_prev,
              .texcoord = stage_t::texcoord1,
              .texmap   = stage_t::texmap1},
    };
    // diffuse * detail(2x) * lightmap (senv with a detail map)
    auto prog_diff_det_lm      = gfx.alloc_program();
    prog_diff_det_lm->channels = {channel_t{.channel = channel_t::color0a0}};
    prog_diff_det_lm->stages   = {
        stage_t{
              .stage    = stage_t::stage0,
              .op       = stage_t::replace,
              .texcoord = stage_t::texcoord0,
              .texmap   = stage_t::texmap0},
        stage_t{
              .stage    = stage_t::stage1,
              .op       = stage_t::modulate_prev_2x,
              .texcoord = stage_t::texcoord2,
              .texmap   = stage_t::texmap2},
        stage_t{
              .stage    = stage_t::stage2,
              .op       = stage_t::modulate_prev,
              .texcoord = stage_t::texcoord1,
              .texmap   = stage_t::texmap1},
    };
    // diffuse only (page has no lightmap)
    auto prog_diff      = gfx.alloc_program();
    prog_diff->channels = {channel_t{.channel = channel_t::color0a0}};
    prog_diff->stages   = {stage_t{
          .stage    = stage_t::stage0,
          .op       = stage_t::replace,
          .texcoord = stage_t::texcoord0,
          .texmap   = stage_t::texmap0}};
    // lightmap only (material has no diffuse): with one tex-gen only texcoord0
    // is generated, so output the lightmap UV (vertex TEX1) into texcoord0.
    auto prog_lm      = gfx.alloc_program();
    prog_lm->channels = {channel_t{.channel = channel_t::color0a0}};
    prog_lm->stages   = {stage_t{
          .stage    = stage_t::stage0,
          .op       = stage_t::replace,
          .texcoord = stage_t::texcoord0,
          .texmap   = stage_t::texmap0}};
    // Objects: diffuse * (ambient + sun), or just the lit material colour
    channel_t const lit{
        .channel          = channel_t::color0a0,
        .lighting         = true,
        .ambient_src      = channel_t::src_reg,
        .diffuse_src      = channel_t::src_reg,
        .light_mask       = GX_LIGHT0,
        .diffuse_function = channel_t::diffuse_clamp,
    };
    auto prog_obj           = gfx.alloc_program();
    prog_obj->channels      = {lit};
    prog_obj->stages        = {stage_t{
               .stage    = stage_t::stage0,
               .op       = stage_t::modulate,
               .texcoord = stage_t::texcoord0,
               .texmap   = stage_t::texmap0}};
    auto prog_obj_flat      = gfx.alloc_program();
    prog_obj_flat->channels = {lit};
    prog_obj_flat->stages   = {
        stage_t{.stage = stage_t::stage0, .op = stage_t::pass}};

    using bind_t = gexxo::texture_binding_t;

    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

#if defined(COFFEE_GEKKO)
    PAD_Init();
    start_network();
#endif

    u64 window_start = gettime();
    u64 last_t       = gettime();
    for(int i = 0;; i++)
    {
        u64 const now_t = gettime();
        f32       dt    = ticks_to_microsecs(now_t - last_t) / 1e6f;
        last_t          = now_t;
        if(dt <= 0.f || dt > 0.2f)
            dt = 1.f / 50.f; // clamp first/stalled frames
        update_camera(dt);
#if defined(COFFEE_GEKKO)
        poll_network(static_cast<u32>(i));
#endif

        rt->clear(Vecf4{0.06f, 0.06f, 0.09f, 1.f}, 1.f, 0);
        Matf4 const     mv     = make_modelview(0.f);
        Matf4 const     mvs[1] = {mv};
        frustum_t const fr     = make_frustum(g_proj * mv);
        u32             drawn = 0, culled = 0;

        for(auto& page : g_pages)
        {
            if(!page.vao || !page.vao->m_element_buffer)
                continue; // skip incomplete (e.g. OOM-interrupted) builds
            for(auto& sm : page.submeshes)
            {
                if(!sphere_visible(fr, sm.center, sm.radius))
                {
                    culled++;
                    continue;
                }
                drawn++;
                gexxo::draw_command cmd{
                    .call =
                        {.indexed   = true,
                         .instanced = true,
                         .mode      = gexxo::drawing::primitive::triangle},
                    .data =
                        {
                            .elements =
                                {.count  = sm.index_count,
                                 .offset = sm.index_offset,
                                 .type   = semantic::type_t::u16},
                            .arrays =
                                {.count = static_cast<u32>(page.verts.size())},
                            .instances = {.count = 1},
                        },
                    .vertices            = page.vao,
                    .instance_transforms = gsl::span<Matf4 const>(mvs, 1),
                };
                if(sm.diffuse && sm.detail && page.lightmap)
                {
                    cmd.program  = prog_diff_det_lm;
                    cmd.textures = {
                        bind_t{
                            .texture  = sm.diffuse,
                            .texmap   = stage_t::texmap0,
                            .texcoord = stage_t::texcoord0,
                            .gen_src  = bind_t::src_tex0},
                        bind_t{
                            .texture  = sm.detail,
                            .texmap   = stage_t::texmap2,
                            .texcoord = stage_t::texcoord2,
                            .gen_src  = bind_t::src_tex2},
                        bind_t{
                            .texture  = page.lightmap,
                            .texmap   = stage_t::texmap1,
                            .texcoord = stage_t::texcoord1,
                            .gen_src  = bind_t::src_tex1},
                    };
                } else if(sm.diffuse && page.lightmap)
                {
                    cmd.program  = prog_diff_lm;
                    cmd.textures = {
                        bind_t{
                            .texture  = sm.diffuse,
                            .texmap   = stage_t::texmap0,
                            .texcoord = stage_t::texcoord0,
                            .gen_src  = bind_t::src_tex0},
                        bind_t{
                            .texture  = page.lightmap,
                            .texmap   = stage_t::texmap1,
                            .texcoord = stage_t::texcoord1,
                            .gen_src  = bind_t::src_tex1},
                    };
                } else if(sm.diffuse)
                {
                    cmd.program  = prog_diff;
                    cmd.textures = {bind_t{.texture = sm.diffuse}};
                } else if(page.lightmap)
                {
                    cmd.program  = prog_lm;
                    cmd.textures = {bind_t{
                        .texture  = page.lightmap,
                        .texmap   = stage_t::texmap0,
                        .texcoord = stage_t::texcoord0,
                        .gen_src  = bind_t::src_tex1}};
                } else
                    continue;
                gfx.submit(cmd, gexxo::cull_state{.front_face = false});
            }
        }

        // Objects: each instance's model parts at camera * model transform.
        load_sun(mv);
        for(auto const& inst : g_objects)
        {
            if(!sphere_visible(fr, inst.center, inst.radius))
            {
                culled++;
                continue;
            }
            auto it = g_models.find(inst.model_id);
            if(it == g_models.end())
                continue;
            drawn++;
            Matf4 const smv     = mv * inst.xform;
            Matf4 const smvs[1] = {smv};
            for(auto& part : it->second.parts)
            {
                if(!part.vao || !part.vao->m_element_buffer)
                    continue;
                gexxo::draw_command cmd{
                    .call =
                        {.indexed   = true,
                         .instanced = true,
                         .mode = gexxo::drawing::primitive::triangle_strip},
                    .data =
                        {
                            .elements =
                                {.count = static_cast<u32>(part.indices.size()),
                                 .type  = semantic::type_t::u16},
                            .arrays =
                                {.count = static_cast<u32>(part.verts.size())},
                            .instances = {.count = 1},
                        },
                    .program  = part.diffuse ? prog_obj : prog_obj_flat,
                    .vertices = part.vao,
                    .instance_transforms = gsl::span<Matf4 const>(smvs, 1),
                    .material_color      = Vecf4{1.f},
                };
                if(part.diffuse)
                    cmd.textures = {bind_t{.texture = part.diffuse}};
                // Model base map alpha is a mask (change colour, specular),
                // not coverage
                gfx.submit(
                    cmd,
                    gexxo::cull_state{.front_face = false},
                    gexxo::blend_state{.opaque = true});
            }
        }
        gexxo::swapBuffers();

        if((i % 60) == 59)
        {
            u64 const now  = gettime();
            f32 const secs = ticks_to_microsecs(now - window_start) / 1e6f;
            LOG("fps=%d.%02d  drawn=%u culled=%u",
                static_cast<int>(60.f / secs),
                static_cast<int>(6000.f / secs) % 100,
                drawn,
                culled);
            window_start = now;
        }
    }
#endif
    return stage == 0 ? 0 : 1;
}

COFFEE_APPLICATION_MAIN(coffee_main)
