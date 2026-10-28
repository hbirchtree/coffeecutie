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
#include <set>
#include <vector>

#include <blam/common/endian.h>
#include <blam/common/vertex_pack.h>
#include <blam/gx/quantize.h>
#include <blam/gx/tiling.h>
#include <blam/volta/blam_antr.h>
#include <blam/volta/blam_antr_sample.h>
#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_bsp_structures.h>
#include <blam/volta/blam_globals.h>
#include <blam/volta/blam_mod2.h>
#include <blam/volta/blam_scenario.h>
#include <blam/volta/blam_shaders.h>
#include <blam/volta/blam_stl.h>
#include <blam/common/swizzle.h>
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

#include "effects.h"
#include "loading_screen.h"
#include "net_client.h"
#include "reflection.h"
#include "skinning.h"
#include "sotr.h"
#endif

using namespace Coffee;
using blam::from_le;
using typing::vector_types::Matf4;
using typing::vector_types::Vecf2;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;

static constexpr char const* MAP_NAME = "bloodgulch.map";

// One below the highest model LOD, as textures skip a mip
static constexpr auto kModelLod        = blam::mod2::lod_high;
static u32            g_models_lowered = 0;

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
 * (diffuse UV pre-scaled by the shader's detail map scale). The lightmap UV
 * is S16 with 15 fraction bits, which keeps this as small as an Xbox BSP
 * vertex, so the transcoder can write it in place. */
struct gx_vertex
{
    f32 pos[3];
    f32 duv[2];
    i16 luv[2];
    f32 detuv[2];
};

static_assert(sizeof(gx_vertex) == 32);

/* A run of triangles in a page sharing one diffuse texture. */
struct submesh_t
{
    u32                        index_offset{0};
    u32                        index_count{0};
    Vecf3                      center{};  // world-space bounding sphere
    f32                        radius{0}; // for frustum culling
    blam::bitm::image_t const* diffuse_img{nullptr};
    blam::bitm::image_t const* detail_img{nullptr};

    /* shader_water: drawn after everything opaque, see water_info */
    bool                       water{false};
    blam::bitm::image_t const* ripple_img{nullptr};
    f32                        ripple_scale{1.f};
    Vecf2                      ripple_flow{0.f};
    Vecf3                      water_tint{1.f};
    blam::bitm::image_t const* water_reflection_img{nullptr};
    Vecf3                      water_steep{0.f}, water_grazing{0.f};
#if defined(FEATURE_ENABLE_Gexxo)
    std::shared_ptr<gexxo::texture_t> diffuse;
    std::shared_ptr<gexxo::texture_t> detail;
    std::shared_ptr<gexxo::texture_t> ripple;
    std::shared_ptr<gexxo::texture_t> water_sky; // see bake_hemisphere()
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
/* As MapTranscode writes them (mod2_gx_skinned_vertex) */
struct mdl_vertex
{
    f32 pos[3];
    i8  nrm[3];
    u8  node0; // the nodes it hangs off; node1 0xFF = none
    f32 uv[2];
    u8  node1;
    u8  weight; // node0's, of 255
    u8  pad[2];
};

static_assert(sizeof(mdl_vertex) == 28);

struct reflection_t;

/* One map of a shader_transparent_chicago; its colour and alpha functions
 * combine the result so far with the next map */
struct chicago_layer
{
    blam::bitm::image_t const* image{nullptr};
    Vecf2                      scale{1.f}, offset{0.f};
    u16                        color_function{0}, alpha_function{0};
};

/* How a part or material goes onto the framebuffer */
enum class blend_t
{
    opaque,
    alpha,
    add,
    multiply,
};

/* shader_transparent_generic, for gx_sotr: its maps and how they scroll, its
 * combiner stages and their constants (RGBA) */
struct sotr_info
{
    struct uv_anim_t
    {
        u16 function{0};
        f32 period{0.f}, scale{1.f};
    };

    struct map_t
    {
        blam::bitm::image_t const* image{nullptr};
        Vecf2                      scale{1.f}, offset{0.f};
        uv_anim_t                  u, v;
    };

    struct constants_t
    {
        Vecf4 lower{0.f}, upper{0.f}, color1{0.f};
        u16   function{0};
        f32   period{0.f};
        bool  object_driven{false}; // by an object function we don't have
    };

    std::vector<map_t>                 maps;
    std::vector<gx_sotr::stage_desc_t> stages;
    std::vector<constants_t>           constants;
};

struct mdl_part
{
    std::vector<mdl_vertex>    verts;
    std::vector<u16>           indices; // triangle strip
    blam::bitm::image_t const* diffuse_img{nullptr};
    blend_t                    blend{blend_t::opaque};
    blam::bitm::image_t const* reflection_img{nullptr};
    blam::bitm::image_t const* multipurpose_img{nullptr};
    Vecf3                      reflection_tint{0.f};
    std::vector<chicago_layer> chicago;
    std::shared_ptr<sotr_info const> sotr;
    bool                             hidden{false}; // see shader_info::hidden
#if defined(FEATURE_ENABLE_Gexxo)
    std::shared_ptr<gexxo::vertex_array_t> vao;
    std::shared_ptr<gexxo::buffer_t>       vbo, ebo;
    std::shared_ptr<gexxo::texture_t>      diffuse;
    std::shared_ptr<reflection_t>          reflection;
    std::shared_ptr<gexxo::texture_t>      multipurpose;
    std::shared_ptr<gexxo::program_t>      chicago_program;
    std::vector<gexxo::texture_binding_t>  chicago_maps;
    std::vector<Matf4>                     chicago_uv;
    std::shared_ptr<gexxo::program_t>      sotr_program;
    std::vector<gexxo::texture_binding_t>  sotr_maps;
    std::vector<gx_sotr::konst_recipe_t>   sotr_konst;

    /* Posed: the part's triangles in batches of a palette each */
    struct batch_t
    {
        std::shared_ptr<gexxo::vertex_array_t> vao;
        std::shared_ptr<gexxo::buffer_t>       vbo, ebo;
        u32                                    index_count{0};
        std::vector<gx_skin::key_t>            keys;
    };

    std::vector<batch_t> batches;
#endif
};

struct model_t
{
    std::vector<mdl_part> parts;
    Vecf3                 center{}; // local-space bounding sphere (for culling)
    f32                   radius{0};
    gx_skin::skeleton_t   skeleton; // bipeds', for posing them
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
    blam::tag_index<> model_id;
    Matf4 xform;
    Vecf3 center{}; // world-space bounding sphere
    f32   radius{0};
    u32   net_id{0};
    bool              hidden{false}; // despawned by the server
    bool              offline{false}; // only shown without a server
};

static std::map<blam::tag_index<>, model_t> g_models; // by mod2 tag id
static std::vector<object_inst> g_objects; // placed instances
static u32                      g_kind_count[kind_count] = {};
// mod2 of the unit players spawn as
static blam::tag_index<> g_player_model = blam::tag_index<>::none();

/* What the server can name at run time, prepared while the map is mapped:
 * the map is let go after loading */
struct catalog_entry
{
    blam::tag_index<> model{blam::tag_index<>::none()};
    blam::tag_index<> projectile{blam::tag_index<>::none()}; // weap: trigger 0
    f32               speed{0.f};                            // proj: wu/s
    f32               gravity{0.f}; // proj: air gravity scale
    std::string       label;        // weap: picks the holder's animations
    std::optional<u32> volume;       // proj: the glow it flies in, mgs2
    std::optional<u32> detonation;   // proj: effe where it ends
    f32                fuse{0.f}; // proj: seconds to detonation, 0 = on impact
};

static std::map<blam::tag_index<>, catalog_entry> g_catalog;
static gx_effects::library_t                      g_fx_lib;
static Matf4 g_hand{1.f}; // the player biped's "right hand", model space
static i32   g_hand_node = -1; // the node it hangs off

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

/* Logs the time since the previous phase ended */
static void load_phase(char const* name)
{
#if defined(COFFEE_GEKKO)
    static u64 last = gettime();
    u64 const  now  = gettime();
    LOG("phase %s: %u ms",
        name,
        static_cast<unsigned>(ticks_to_millisecs(now - last)));
    last = now;
#else
    (void)name;
#endif
}

#if defined(COFFEE_GEKKO)
static gx_loading::screen_t g_loading;

/* The loading still, made from loading.png by the resource build */
static void start_loading_screen(gexxo::api& gfx)
{
    namespace dvd = platform::file::gekko::dvd;
    auto const* e = dvd::detail::find("loading.tpl");
    if(!e)
        return;
    std::vector<u8> tpl(e->size);
    if(!dvd::detail::read_at(tpl.data(), e->size, e->off))
        return;
    g_loading.begin(gfx, tpl);
    platform::file::gekko::vmem::set_stage_progress(
        [](u32 done, u32 total) { g_loading.step(done, total); });
}
#endif

/* Share [from, to] of the load bar between `total` steps */
static void loading_phase(f32 from, f32 to)
{
#if defined(COFFEE_GEKKO)
    g_loading.phase(from, to);
#else
    (void)from, (void)to;
#endif
}

static void loading_step(u32 done, u32 total)
{
#if defined(COFFEE_GEKKO)
    g_loading.step(done, total);
#else
    (void)done, (void)total;
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

/* --- tag/shader/bitmap resolution ----------------------------------------- */

template<typename V>
static blam::bitm::header_t const* resolve_bitm(
    blam::map_container<V> const& c, blam::tagref_t const& ref)
{
    auto const* t = blam::tag_at(c, ref.tag_id);
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
    blend_t                    blend{blend_t::opaque};

    /* shader_model: reflection cube, its mask (multipurpose map), tint */
    blam::bitm::image_t const* reflection{nullptr};
    blam::bitm::image_t const* multipurpose{nullptr};
    Vecf3                      reflection_tint{0.f};

    /* shader_water: its ripple map, how it tiles and flows, its colour */
    struct water_info
    {
        blam::bitm::image_t const* ripple{nullptr};
        f32                        scale{1.f};
        Vecf2                      flow{0.f}; // per second
        Vecf3                      tint{1.f};
        /* Its reflection cube, from looking down to along the surface */
        blam::bitm::image_t const* reflection{nullptr};
        Vecf3                      steep{0.f}, grazing{0.f};
    };

    std::optional<water_info> water;

    std::vector<chicago_layer> chicago; // up to four

    std::shared_ptr<sotr_info const> sotr;

    /* Plasma, glass and meter shaders: transparent effects not drawn yet,
     * better left out than drawn as an opaque shape */
    bool hidden{false};
};

static blend_t framebuffer_blend(blam::shader::chicago::framebuffer_blending f)
{
    using fb = blam::shader::chicago::framebuffer_blending;
    switch(f)
    {
    case fb::add:
    case fb::component_max: // no max on GX; brightens like it
        return blend_t::add;
    case fb::multiply:
    case fb::double_multiply:
        return blend_t::multiply;
    default:
        return blend_t::alpha;
    }
}

/* Material shader -> base diffuse + (primary) detail map. shader_environment
 * (BSP) + shader_model (objects) are handled, chicago shaders (skies, glows)
 * by their first map and framebuffer blend; other classes are untextured. */
template<typename V>
static shader_info resolve_shader(
    blam::map_container<V> const& c, blam::tagref_t const& shader_ref)
{
    shader_info out;
    auto const* st = blam::tag_at(c, shader_ref.tag_id);
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
        auto const& r    = sh.value()->reflection;
        out.reflection   = first_image(c, resolve_bitm(c, r.reflection));
        out.multipurpose =
            first_image(c, resolve_bitm(c, sh.value()->maps.multipurpose));
        out.reflection_tint =
            Vecf3{
                from_le(r.perpendicular_tint.x),
                from_le(r.perpendicular_tint.y),
                from_le(r.perpendicular_tint.z)} *
            from_le(r.perpendicular_brightness);
    } else if(st->matches(blam::tag_class_t::swat))
    {
        auto sh = st->template data<blam::shader::shader_water>(c.magic);
        if(sh.has_error())
            return out;
        auto const& w     = *sh.value();
        f32 const   angle = from_le(w.ripple.anim_angle);
        f32 const   speed = from_le(w.ripple.anim_velocity);
        auto        view  = [](auto const& props) {
            auto const& t = props.tint_color;
            return Vecf3{from_le(t.x), from_le(t.y), from_le(t.z)} *
                   from_le(props.brightness);
        };
        out.water = shader_info::water_info{
            .ripple     = first_image(c, resolve_bitm(c, w.ripple.map)),
            .scale      = from_le(w.ripple.scale),
            .flow       = Vecf2{std::cos(angle), std::sin(angle)} * speed,
            .tint       = view(w.perpendicular),
            .reflection = first_image(c, resolve_bitm(c, w.reflection_map)),
            .steep      = view(w.perpendicular),
            .grazing    = view(w.parallel),
        };
        if(out.water->scale == 0.f)
            out.water->scale = 1.f;
    } else if(st->matches(blam::tag_class_t::schi))
    {
        auto sh = st->template data<blam::shader::shader_chicago<V>>(c.magic);
        if(sh.has_error())
            return out;
        if(auto maps = sh.value()->maps.data(c.magic);
           !maps.has_error() && !maps.value().empty())
        {
            out.diffuse =
                first_image(c, resolve_bitm(c, maps.value()[0].map.map));
            for(auto const& m : maps.value())
            {
                if(out.chicago.size() == 4)
                    break;
                Vecf2 const scale{
                    from_le(m.map.uv_scale.x), from_le(m.map.uv_scale.y)};
                out.chicago.push_back({
                    .image = first_image(c, resolve_bitm(c, m.map.map)),
                    .scale =
                        {scale.x == 0.f ? 1.f : scale.x,
                         scale.y == 0.f ? 1.f : scale.y},
                    .offset =
                        {from_le(m.map.uv_offset.x),
                         from_le(m.map.uv_offset.y)},
                    .color_function = static_cast<u16>(from_le(m.color_func)),
                    .alpha_function = static_cast<u16>(from_le(m.alpha_func)),
                });
            }
        }
        out.blend =
            framebuffer_blend(from_le(sh.value()->transparent.blend_function));
    } else if(
        st->matches(blam::tag_class_t::spla) ||
        st->matches(blam::tag_class_t::sgla) ||
        st->matches(blam::tag_class_t::smet))
    {
        out.hidden = true;
    } else if(st->matches(blam::tag_class_t::sotr))
    {
        using sotr_t = blam::shader::shader_transparent;
        auto sh      = st->template data<sotr_t>(c.magic);
        if(sh.has_error())
            return out;
        auto info = std::make_shared<sotr_info>();
        auto anim = [](blam::shader::texture_property_anim const& a) {
            return sotr_info::uv_anim_t{
                static_cast<u16>(from_le(a.function)),
                from_le(a.period),
                from_le(a.scale)};
        };
        if(auto maps = sh.value()->maps.data(c.magic); !maps.has_error())
            for(auto const& m : maps.value())
            {
                if(info->maps.size() == 4)
                    break;
                Vecf2 const scale{
                    from_le(m.map.uv_scale.x), from_le(m.map.uv_scale.y)};
                info->maps.push_back({
                    .image = first_image(c, resolve_bitm(c, m.map.map)),
                    .scale =
                        {scale.x == 0.f ? 1.f : scale.x,
                         scale.y == 0.f ? 1.f : scale.y},
                    .offset =
                        {from_le(m.map.uv_offset.x),
                         from_le(m.map.uv_offset.y)},
                    .u = anim(m.animation.u),
                    .v = anim(m.animation.v),
                });
            }
        // Tag colours are ARGB
        auto rgba = [](Vecf4 const& argb) {
            return Vecf4{
                from_le(argb.y),
                from_le(argb.z),
                from_le(argb.w),
                from_le(argb.x)};
        };
        if(auto stages = sh.value()->stages.data(c.magic); !stages.has_error())
            for(auto const& st_ : stages.value())
            {
                auto half = [](auto const& m,
                               auto        ab,
                               auto        cd,
                               auto        sum,
                               auto        map,
                               bool        mux) {
                    gx_sotr::half_t h;
                    h.input = {
                        static_cast<u16>(from_le(m.a_input)),
                        static_cast<u16>(from_le(m.b_input)),
                        static_cast<u16>(from_le(m.c_input)),
                        static_cast<u16>(from_le(m.d_input))};
                    h.mapping = {
                        static_cast<u16>(from_le(m.a_mapping)),
                        static_cast<u16>(from_le(m.b_mapping)),
                        static_cast<u16>(from_le(m.c_mapping)),
                        static_cast<u16>(from_le(m.d_mapping))};
                    h.ab             = static_cast<u16>(from_le(ab));
                    h.cd             = static_cast<u16>(from_le(cd));
                    h.sum            = static_cast<u16>(from_le(sum));
                    h.output_mapping = static_cast<u16>(from_le(map));
                    h.mux            = mux;
                    return h;
                };
                u16 const flags = static_cast<u16>(from_le(st_.flags));
                info->stages.push_back({
                    .color = half(
                        st_.color,
                        st_.color.ab_output,
                        st_.color.cd_output,
                        st_.color.ab_cd_mux_sum,
                        st_.color.output_map,
                        flags & 0x1),
                    .alpha = half(
                        st_.alpha,
                        st_.alpha.ab_output,
                        st_.alpha.cd_output,
                        st_.alpha.ab_cd_mux_sum,
                        st_.alpha.output_map,
                        flags & 0x2),
                });
                info->constants.push_back({
                    .lower    = rgba(st_.color0_lower),
                    .upper    = rgba(st_.color0_upper),
                    .color1   = rgba(st_.color1),
                    .function = static_cast<u16>(from_le(st_.color0_func)),
                    .period   = from_le(st_.color0_period),
                    .object_driven =
                        (flags & 0x4) || from_le(st_.color0_source) !=
                                             blam::shader::animation_src::none,
                });
            }
        if(!info->maps.empty())
            out.diffuse = info->maps[0].image; // without stages, or the TEV
        out.blend =
            framebuffer_blend(from_le(sh.value()->transparent.blend_function));
        out.sotr = std::move(info);
    }
    if(out.detail_scale == 0.f)
        out.detail_scale = 1.f;
    return out;
}

/* --- cluster lookup (RAM copy of the collision tree) ---------------------- */

// Per-object cluster_for_point thrashes the paged map (random collision-tree
// faults). Copy the small collision arrays into RAM once (sequential read =
// each page faulted once), then query against RAM with no further paging.
static std::vector<blam::collision::bsp_3d> g_coll_nodes;
static std::vector<blam::collision::plane>  g_coll_planes;
static std::vector<i16>                     g_leaf_cluster;

/* The scenario's skies: drawn around the camera, with their fog and light.
 * Which one shows is the camera's cluster's choice. */
struct sky_t
{
    blam::tag_index<> model{blam::tag_index<>::none()};
    Vecf3             fog_color{};
    f32               fog_density{0.f};
    f32               fog_start{0.f}, fog_opaque{0.f};
    Vecf3             ambient{0.35f};
    Vecf3             sun_color{0.8f};
    Vecf3             sun_dir{-0.4f, -0.3f, -0.85f}; // the way the light goes
};

static std::vector<sky_t> g_skies;
static std::vector<i16>   g_cluster_sky; // of the loaded BSP's clusters

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
    g_coll_nodes.assign(nodes.value().begin(), nodes.value().end());
    g_coll_planes.assign(planes.value().begin(), planes.value().end());
    for(auto const& l : leaves.value())
        g_leaf_cluster.push_back(from_le(l.cluster));
}

// Cluster index containing point, or nullopt for solid/outside-the-BSP points.
static std::optional<u32> cluster_at(Vecf3 const& pt)
{
    auto const leaf = blam::collision_bsp::find_leaf<blam::collision::traits>(
        g_coll_nodes, g_coll_planes, pt, g_coll_nodes.size());
    if(!leaf || *leaf >= g_leaf_cluster.size())
        return std::nullopt;
    i16 const c = g_leaf_cluster[*leaf];
    return c < 0 ? std::nullopt : std::optional<u32>(c);
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

/* kModelLod, or the highest where a permutation has no such level, or where
 * it is a stand-in (powerups' lower levels are a pyramid or a cube) */
template<typename V, typename Magic>
static blam::mod2::mod2_lod model_lod(
    blam::mod2::header<V> const& hdr, Magic const& magic)
{
    auto regions = hdr.regions.data(magic);
    auto geoms   = hdr.geometries.data(magic);
    if(!regions.has_value() || !geoms.has_value())
        return blam::mod2::lod_high_ext;
    auto verts = [&](u16 geometry) {
        u32  n     = 0;
        auto parts = geoms.value()[geometry].meshes(magic);
        for(auto const& p : parts)
            n += from_le(p.data.vert_count);
        return n;
    };
    u32 lower = 0, highest = 0;
    for(auto const& reg : regions.value())
    {
        auto perms = reg.permutations.data(magic);
        if(!perms.has_value())
            return blam::mod2::lod_high_ext;
        // Indices run from the lowest level up
        for(auto const& per : perms.value())
        {
            u16 const want = from_le(per.meshindex_lod.at(4u - kModelLod));
            u16 const top  = from_le(per.meshindex_lod.at(4u));
            if(want >= geoms.value().size() || top >= geoms.value().size())
                return blam::mod2::lod_high_ext;
            lower += verts(want);
            highest += verts(top);
        }
    }
    if(lower == highest)
        return blam::mod2::lod_high_ext;
    if(lower * 5 < highest * 2)
        return blam::mod2::lod_high_ext;
    g_models_lowered++;
    return kModelLod;
}

template<typename V>
static void load_model_cpu(
    blam::map_container<V> const& c, blam::tag_index<> mod2_id)
{
    using namespace blam;
    auto const* t = blam::tag_at(c, mod2_id);
    if(!t)
        return;
    auto hdr = t->template data<mod2::header<V>>(c.magic);
    if(hdr.has_error())
        return;
    std::optional<mod2::model_data_t> md;
    try
    {
        md = hdr.value()->model_at(model_lod(*hdr.value(), c.magic), c.magic);
    } catch(std::exception const&)
    {
    }
    if(!md)
        return;
    Vecf2 const uvscale{
        from_le(hdr.value()->uvscale.x), from_le(hdr.value()->uvscale.y)};
    auto shaders = hdr.value()->shaders.data(c.magic);
    auto vmag    = c.tags->vertex_magic(c.magic);

    model_t    model;
    Vecf3      mn{1e9f, 1e9f, 1e9f}, mx{-1e9f, -1e9f, -1e9f};
    bool const local_nodes = static_cast<u16>(from_le(hdr.value()->flags)) &
                             static_cast<u16>(mod2::model_flags_t::local_nodes);
    for(mod2::part const* p : md->parts)
    {
        mdl_part part;
        try
        {
            auto const vref = p->vertex_segment(*c.tags, c.magic);
            // PC parts may index a table of the model's nodes
            auto local_node = [&](u32 node) -> u32 {
                if constexpr(!std::is_same_v<V, xbox_version_t>)
                {
                    auto const* wrap =
                        reinterpret_cast<mod2::part_wrap_header<V> const*>(p);
                    if(local_nodes && node < wrap->unknown_2[3])
                        return wrap->unknown_2[4 + node];
                }
                return node;
            };
            auto       iseg = p->index_segment(*c.tags).data(vmag);
            if(iseg.has_error())
                continue;
            auto const vtype = from_le(p->vertex_type);
            if(vtype == vert::vertex_type_t::mod2_gx_skinned_vertex ||
               vtype == vert::vertex_type_t::mod2_gx_vertex)
            {
                // Transcoded offline: already GX arrays, big-endian. The
                // older ones lack the nodes, so they stay unposed
                bool const nodes =
                    vtype == vert::vertex_type_t::mod2_gx_skinned_vertex;
                u32 const stride = nodes ? sizeof(mdl_vertex) : 24;
                u32 const n      = from_le(vref.count);
                auto      raw    = *vref.template as<u8>();
                raw.count        = to_le<u32>(n * stride);
                auto bytes       = raw.data(vmag);
                if(bytes.has_error())
                    continue;
                part.verts.resize(n);
                for(u32 i = 0; i < n; i++)
                {
                    auto& v = part.verts[i];
                    std::memcpy(&v, bytes.value().data() + i * stride, stride);
                    if(!nodes)
                    {
                        v.node0  = 0;
                        v.node1  = 0xFF;
                        v.weight = 255;
                    }
                }
                for(auto const& v : part.verts)
                {
                    Vecf3 const p3{v.pos[0], v.pos[1], v.pos[2]};
                    mn = glm::min(mn, p3);
                    mx = glm::max(mx, p3);
                }
                part.indices.assign(iseg.value().begin(), iseg.value().end());
            } else if(auto vseg = vref.data(vmag); !vseg.has_error())
            {
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
                        n       = blam::vertex_pack::unpack_11_11_10(rd<u32>(raw + 12));
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
                        v.nrm[k] = blam::gx::to_s8_frac6(n[k]);
                    f32 weight;
                    u32 node0, node1;
                    if constexpr(std::is_same_v<V, xbox_version_t>)
                    {
                        // Stored times three, a matrix row each
                        node0  = raw[28] / 3u;
                        node1  = raw[29] / 3u;
                        weight = rd<u16>(raw + 30) / 32767.f;
                    } else
                    {
                        node0  = local_node(rd<u16>(raw + 56));
                        node1  = local_node(rd<u16>(raw + 58));
                        weight = rd<f32>(raw + 60);
                    }
                    weight   = std::clamp(weight, 0.f, 1.f);
                    v.node0  = static_cast<u8>(std::min(node0, 0xFEu));
                    v.node1  = weight < 1.f
                                   ? static_cast<u8>(std::min(node1, 0xFEu))
                                   : u8(0xFF);
                    v.weight = static_cast<u8>(weight * 255.f + 0.5f);
                    v.pad[0] = v.pad[1] = 0;
                    part.verts.push_back(v);
                    Vecf3 const p3{v.pos[0], v.pos[1], v.pos[2]};
                    mn = glm::min(mn, p3);
                    mx = glm::max(mx, p3);
                }
                for(u16 idx : iseg.value())
                    part.indices.push_back(from_le(idx));
            }
        } catch(std::exception const&)
        {
            continue;
        }
        if(!shaders.has_error())
        {
            u16 const si = from_le(p->shader_idx);
            if(si < shaders.value().size())
            {
                auto const shader = resolve_shader(c, shaders.value()[si].ref);
                part.diffuse_img  = shader.diffuse;
                part.blend        = shader.blend;
                part.reflection_img   = shader.reflection;
                part.multipurpose_img = shader.multipurpose;
                part.reflection_tint  = shader.reflection_tint;
                part.chicago          = shader.chicago;
                part.sotr             = shader.sotr;
                part.hidden           = shader.hidden;
            }
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
    blam::tag_index<>             object_tag_id,
    Matf4 const&                  xform,
    object_kind                   kind,
    u32                           net_id = 0)
{
    auto const* t = blam::tag_at(c, object_tag_id);
    if(!t)
        return false;
    auto obj = t->template data<blam::scn::object>(c.magic);
    if(obj.has_error())
        return false;
    auto const mod2_id = obj.value()->model.tag_id;
    if(!blam::tag_at(c, mod2_id))
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
    for(auto const& placed : blam::scn::placements(group, c.magic))
    {
        Spawn const& s = placed.placement;
        Vecf3 const  pos{from_le(s.pos.x), from_le(s.pos.y), from_le(s.pos.z)};
        if(!in_active_section(pos, static_cast<u16>(from_le(s.bsp_flags))))
            continue;
        if(!placed.object)
            continue;
        Vecf3 const rot{from_le(s.rot.x), from_le(s.rot.y), from_le(s.rot.z)};
        place_object(
            c,
            placed.object->tag_id,
            spawn_transform(pos, rot),
            kind,
            net_id_of(kind, static_cast<u32>(placed.index)));
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
        auto const* coll_tag = blam::tag_at(c, entry.item.tag_id);
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
            items.value()[0].item.tag_id,
            spawn_transform(pos, {from_le(entry.facing), 0.f, 0.f}),
            kind_netgame_item,
            net_id_of(kind_netgame_item, e));
    }
}

/* The unit players spawn as (globals: multiplayer, else singleplayer) */
template<typename V>
static blam::tag_index<> player_biped_id(blam::map_container<V> const& c)
{
    u32 const   cnt = from_le(c.tags->tag_count);
    auto const* tg  = c.tags->tags(c.map);
    for(u32 i = 0; i < cnt; i++)
    {
        if(!tg[i].matches(blam::tag_class_t::matg))
            continue;
        auto g = tg[i].template data<blam::globals::globals>(c.magic);
        if(g.has_error())
            return blam::tag_index<>::none();
        if(auto mp = g.value()->multiplayer.data(c.magic);
           !mp.has_error() && !mp.value().empty())
            return mp.value()[0].unit.tag_id;
        if(auto sp = g.value()->player.data(c.magic);
           !sp.has_error() && !sp.value().empty())
            return sp.value()[0].unit.tag_id;
        return blam::tag_index<>::none();
    }
    return blam::tag_index<>::none();
}

template<typename V>
static void build_catalog(blam::map_container<V> const& c)
{
    u32 const   cnt = from_le(c.tags->tag_count);
    auto const* tg  = c.tags->tags(c.map);
    for(u32 i = 0; i < cnt; i++)
    {
        auto const& t    = tg[i];
        bool const  weap = t.matches(blam::tag_class_t::weap);
        bool const  proj = t.matches(blam::tag_class_t::proj);
        // What a server may spawn; scenery stays the map's
        if(!weap && !proj && !t.matches(blam::tag_class_t::eqip) &&
           !t.matches(blam::tag_class_t::garb) &&
           !t.matches(blam::tag_class_t::vehi) &&
           !t.matches(blam::tag_class_t::bipd))
            continue;
        auto obj = t.template data<blam::scn::object>(c.magic);
        if(obj.has_error())
            continue;
        catalog_entry e;
        e.model = obj.value()->model.tag_id;
        if(blam::tag_at(c, e.model) && !g_models.contains(e.model))
            load_model_cpu(c, e.model);
        if(weap)
            if(auto w = t.template data<blam::scn::weapon>(c.magic);
               !w.has_error())
            {
                if(auto tr = w.value()->triggers.data(c.magic);
                   !tr.has_error() && !tr.value().empty())
                    e.projectile = tr.value()[0].projectile.tag_id;
                e.label = std::string(w.value()->label.str());
            }
        if(proj)
            if(auto pr = t.template data<blam::scn::projectile>(c.magic);
               !pr.has_error())
            {
                auto const& p = *pr.value();
                e.speed       = from_le(p.initial_velocity) * 30.f;
                e.gravity     = from_le(p.air_gravity_scale);
                e.fuse        = from_le(p.timer.x);
                if(auto as = p.attachments.data(c.magic); !as.has_error())
                    for(auto const& a : as.value())
                        if(!e.volume)
                            e.volume = g_fx_lib.volume(c, a.type);
                if(auto ws = p.widgets.data(c.magic); !ws.has_error())
                    for(auto const& w : ws.value())
                        if(!e.volume)
                            e.volume = g_fx_lib.volume(c, w.widget);
                e.detonation = g_fx_lib.effect(c, p.detonation_effect);
                // Else what it does to the ground
                if(auto rs = p.material_responses.data(c.magic);
                   !e.detonation && !rs.has_error())
                    for(auto const& r : rs.value())
                        if(!e.detonation)
                            e.detonation = g_fx_lib.effect(c, r.default_effect);
            }
        g_catalog[t.tag_id] = e;
    }
    LOG("catalog: %u vehicles, bipeds, weapons, equipment and projectiles",
        static_cast<unsigned>(g_catalog.size()));
    LOG("effects: %u effects, %u particles, %u light volumes, %u frames",
        static_cast<unsigned>(g_fx_lib.effects.size()),
        static_cast<unsigned>(g_fx_lib.particles.size()),
        static_cast<unsigned>(g_fx_lib.volumes.size()),
        static_cast<unsigned>(g_fx_lib.frames.size()));
}

/* Where a biped's "right hand" marker sits in its bind pose: the marker in
 * its node, up the node's parents */
template<typename V>
static void load_hand_marker(
    blam::map_container<V> const& c, blam::tag_index<> mod2_id)
{
    auto const* t = blam::tag_at(c, mod2_id);
    if(!t)
        return;
    auto hdr = t->template data<blam::mod2::header<V>>(c.magic);
    if(hdr.has_error())
        return;
    auto markers = hdr.value()->markers.data(c.magic);
    auto bones   = hdr.value()->bones.data(c.magic);
    if(markers.has_error() || bones.has_error())
        return;
    auto v3 = [](Vecf3 const& v) {
        return Vecf3{from_le(v.x), from_le(v.y), from_le(v.z)};
    };
    // mod2 rotations are stored conjugated
    auto rotation = [](f32 x, f32 y, f32 z, f32 w) {
        return glm::mat4_cast(
            glm::conjugate(
                Quatf(from_le(w), from_le(x), from_le(y), from_le(z))));
    };
    for(auto const& m : markers.value())
    {
        std::string_view const name(
            m.name.data.data(),
            strnlen(m.name.data.data(), m.name.data.size()));
        if(name != "right hand")
            continue;
        auto inst = m.instances.data(c.magic);
        if(inst.has_error() || inst.value().empty())
            return;
        auto const& in = inst.value()[0];
        Matf4       xf =
            glm::translate(Matf4{1.f}, v3(in.position)) *
            rotation(
                in.rotation.x, in.rotation.y, in.rotation.z, in.rotation.w);
        for(i32 node = in.node_idx;
            node >= 0 && static_cast<u32>(node) < bones.value().size();)
        {
            auto const& b = bones.value()[node];
            xf            = glm::translate(Matf4{1.f}, v3(b.translation)) *
                 rotation(
                     b.rotation.x, b.rotation.y, b.rotation.z, b.rotation.w) *
                 xf;
            u16 const parent = from_le(b.parent);
            node = parent == blam::mod2::bone::invalid_bone ? -1 : parent;
        }
        g_hand      = xf;
        g_hand_node = in.node_idx;
        LOG("hand marker at %d,%d,%d cm",
            int(xf[3].x * 100),
            int(xf[3].y * 100),
            int(xf[3].z * 100));
        return;
    }
}

/* A model's nodes in their bind pose, for posing it */
template<typename V>
static void load_skeleton(
    blam::map_container<V> const& c, blam::tag_index<> mod2_id)
{
    auto const* t  = blam::tag_at(c, mod2_id);
    auto        it = g_models.find(mod2_id);
    if(!t || it == g_models.end())
        return;
    auto hdr = t->template data<blam::mod2::header<V>>(c.magic);
    if(hdr.has_error())
        return;
    auto bones = hdr.value()->bones.data(c.magic);
    if(bones.has_error())
        return;
    auto& skeleton = it->second.skeleton;
    for(auto const& b : bones.value())
    {
        u16 const parent = from_le(b.parent);
        skeleton.bones.push_back({
            .parent = parent == blam::mod2::bone::invalid_bone
                          ? i16(-1)
                          : static_cast<i16>(parent),
            .translation =
                {from_le(b.translation.x),
                 from_le(b.translation.y),
                 from_le(b.translation.z)},
            // mod2 rotations are stored conjugated
            .rotation = glm::conjugate(Quatf(
                from_le(b.rotation.w),
                from_le(b.rotation.x),
                from_le(b.rotation.y),
                from_le(b.rotation.z))),
        });
    }
    gx_skin::bind(skeleton);
}

/* The player biped's clips, copied out while the map is still here */
struct clip_t
{
    blam::antr::animation header; // as stored, little-endian
    std::vector<u8>       defaults, frames;
};

/* A stance's clips by unit_weapon::animation_idx_t slot, -1 = none */
struct stance_t
{
    i32 idle{-1}, front{-1}, back{-1}, left{-1}, right{-1}, airborne{-1};
};

static std::vector<clip_t>             g_clips;
static std::map<std::string, stance_t> g_stances; // by weapon label

static constexpr char const* kUnarmed = "unarmed";

/* A clip's bytes into g_clips, as the map is let go after loading */
template<typename V>
static i32 copy_clip(
    blam::map_container<V> const& c, blam::antr::animation const& a)
{
    clip_t out{.header = a};
    if(auto d = a.default_data.data(c.magic); d.has_value())
        out.defaults.assign(d.value().begin(), d.value().end());
    if(auto f = a.frame_data.data(c.magic); f.has_value())
        out.frames.assign(f.value().begin(), f.value().end());
    g_clips.push_back(std::move(out));
    return static_cast<i32>(g_clips.size() - 1);
}

/* Standing, with each weapon the catalogue has: like BlamGraphics, a weapon
 * picks its unit_weapon by name or by one of its types' labels */
template<typename V>
static void load_biped_clips(
    blam::map_container<V> const& c, blam::tag_index<> biped)
{
    using blam::antr::unit_weapon;
    auto const* t = blam::tag_at(c, biped);
    if(!t)
        return;
    auto obj = t->template data<blam::scn::object>(c.magic);
    if(obj.has_error())
        return;
    auto const* gt = blam::tag_at(c, obj.value()->anim_graph.tag_id);
    if(!gt)
        return;
    auto graph = gt->template data<blam::antr::header>(c.magic);
    if(graph.has_error())
        return;
    auto units = graph.value()->units.data(c.magic);
    auto anims = graph.value()->animations.data(c.magic);
    if(!units.has_value() || !anims.has_value())
        return;
    blam::antr::unit const* stand = nullptr;
    for(auto const& u : units.value())
        if(u.label.str() == "stand")
            stand = &u;
    if(!stand)
        return;
    auto weapons = stand->weapons.data(c.magic);
    if(!weapons.has_value() || weapons.value().empty())
        return;

    std::map<i16, i32> copied; // animation index -> g_clips
    auto               clip = [&](i16 index) -> i32 {
        if(index < 0 || static_cast<size_t>(index) >= anims.value().size())
            return -1;
        if(auto it = copied.find(index); it != copied.end())
            return it->second;
        return copied[index] = copy_clip(c, anims.value()[index]);
    };

    std::set<std::string> labels{kUnarmed};
    for(auto const& [_, entry] : g_catalog)
        if(!entry.label.empty())
            labels.insert(entry.label);
    for(auto const& label : labels)
    {
        unit_weapon const* held = &weapons.value()[0];
        for(auto const& w : weapons.value())
        {
            if(w.name.str() == label)
                held = &w;
            if(auto types = w.weapon_types.data(c.magic); types.has_value())
                for(auto const& type : types.value())
                    if(type.label.str() == label)
                        held = &w;
        }
        auto refs = held->animations.data(c.magic);
        if(!refs.has_value())
            continue;
        auto slot = [&](unit_weapon::animation_idx_t i) {
            return i < refs.value().size()
                       ? clip(from_le(refs.value()[i].animation))
                       : -1;
        };
        g_stances[label] = {
            .idle     = slot(unit_weapon::idle),
            .front    = slot(unit_weapon::move_front),
            .back     = slot(unit_weapon::move_back),
            .left     = slot(unit_weapon::move_left),
            .right    = slot(unit_weapon::move_right),
            .airborne = slot(unit_weapon::airborne),
        };
    }
    size_t bytes = 0;
    for(auto const& cl : g_clips)
        bytes += cl.defaults.size() + cl.frames.size();
    LOG("biped: %u stances, %u clips, %u KiB",
        static_cast<unsigned>(g_stances.size()),
        static_cast<unsigned>(g_clips.size()),
        static_cast<unsigned>(bytes / 1024));
}

/* First person: the hands, and each weapon's view model, both posed by the
 * weapon's first-person graph, whose nodes they name */
struct first_person_t
{
    blam::tag_index<>            model{blam::tag_index<>::none()};
    std::vector<gx_skin::bone_t> nodes;         // the graph's, for parents
    std::vector<i16>             hands, weapon; // model bone -> graph node
    i32                          idle{-1}, fire{-1};
};

static blam::tag_index<> g_hands = blam::tag_index<>::none();
static std::map<blam::tag_index<>, first_person_t> g_first_person; // by weap

template<typename V>
static void load_first_person(blam::map_container<V> const& c)
{
    u32 const   cnt = from_le(c.tags->tag_count);
    auto const* tg  = c.tags->tags(c.map);
    for(u32 i = 0; i < cnt && !g_hands.valid(); i++)
        if(tg[i].matches(blam::tag_class_t::matg))
            if(auto g = tg[i].template data<blam::globals::globals>(c.magic);
               !g.has_error())
                if(auto fp = g.value()->first_person.data(c.magic);
                   !fp.has_error() && !fp.value().empty())
                    g_hands = fp.value()[0].hands.tag_id;
    auto const* hands_tag = blam::tag_at(c, g_hands);
    if(!hands_tag)
        return;
    auto load = [&c](blam::tag_index<> id) {
        if(!g_models.contains(id) && blam::tag_at(c, id))
        {
            load_model_cpu(c, id);
            load_skeleton(c, id);
        }
    };
    load(g_hands);

    // Each of a model's bones as one of the graph's nodes, by name
    auto map_bones =
        [&c](blam::tag_index<> id, std::vector<std::string> const& names) {
            std::vector<i16> out;
            auto const*      t = blam::tag_at(c, id);
            if(!t)
                return out;
            auto hdr = t->template data<blam::mod2::header<V>>(c.magic);
            if(hdr.has_error())
                return out;
            auto bones = hdr.value()->bones.data(c.magic);
            if(bones.has_error())
                return out;
            for(auto const& b : bones.value())
            {
                auto it = std::find(names.begin(), names.end(), b.name.str());
                if(it == names.end())
                    return std::vector<i16>{};
                out.push_back(static_cast<i16>(it - names.begin()));
            }
            return out;
        };

    using blam::antr::first_person_weapon;
    for(auto const& [id, entry] : g_catalog)
    {
        auto const* t = blam::tag_at(c, id);
        if(!t || !t->matches(blam::tag_class_t::weap))
            continue;
        auto w = t->template data<blam::scn::weapon>(c.magic);
        if(w.has_error())
            continue;
        auto const* gt =
            blam::tag_at(c, w.value()->first_person_animations.tag_id);
        if(!gt)
            continue;
        auto graph = gt->template data<blam::antr::header>(c.magic);
        if(graph.has_error())
            continue;
        auto nodes = graph.value()->nodes.data(c.magic);
        auto anims = graph.value()->animations.data(c.magic);
        auto fpw   = graph.value()->first_person_weapons.data(c.magic);
        if(!nodes.has_value() || !anims.has_value() || !fpw.has_value() ||
           fpw.value().empty())
            continue;
        auto refs = fpw.value()[0].animations.data(c.magic);
        if(!refs.has_value())
            continue;
        auto slot = [&](first_person_weapon::animation_idx_t s) -> i32 {
            if(s >= refs.value().size())
                return -1;
            i16 const a = from_le(refs.value()[s].animation);
            return a >= 0 && static_cast<size_t>(a) < anims.value().size() ? a
                                                                           : -1;
        };
        i32 const idle = slot(first_person_weapon::idle);
        if(idle < 0)
            continue;

        first_person_t           fp;
        std::vector<std::string> names;
        for(auto const& n : nodes.value())
        {
            names.emplace_back(n.name.str());
            fp.nodes.push_back({.parent = from_le(n.parent)});
        }
        fp.hands = map_bones(g_hands, names);
        if(fp.hands.empty())
            continue;
        fp.model  = w.value()->first_person_model.tag_id;
        fp.weapon = map_bones(fp.model, names);
        if(!fp.weapon.empty())
            load(fp.model);
        fp.idle = copy_clip(c, anims.value()[idle]);
        if(i32 const fire = slot(first_person_weapon::fire_1); fire >= 0)
            fp.fire = copy_clip(c, anims.value()[fire]);
        g_first_person[id] = std::move(fp);
    }
    LOG("first person: %u weapons",
        static_cast<unsigned>(g_first_person.size()));
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

    if(auto const biped = player_biped_id(c); biped.valid())
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
                {
                    g_objects.back().offline = true;
                    n++;
                }
            }
        }
    if(auto const  biped = player_biped_id(c);
       auto const* t     = blam::tag_at(c, biped))
        if(auto unit = t->template data<blam::scn::object>(c.magic);
           !unit.has_error())
        {
            g_player_model = unit.value()->model.tag_id;
            if(g_models.find(g_player_model) == g_models.end() &&
               blam::tag_at(c, g_player_model))
                load_model_cpu(c, g_player_model);
            load_hand_marker(c, g_player_model);
            load_skeleton(c, g_player_model);
        }
    build_catalog(c);
    load_biped_clips(c, player_biped_id(c));
    load_first_person(c);

    u32 verts = 0;
    for(auto const& [id, m] : g_models)
        for(auto const& p : m.parts)
            verts += static_cast<u32>(p.verts.size());
    for(u32 k = 0; k < kind_count; k++)
        if(g_kind_count[k])
            LOG("objects: %u %s", g_kind_count[k], g_kind_names[k]);
    LOG("objects: %u placed, %u models (%u a LOD down), %u verts (%u KiB as "
        "GX vertices)",
        static_cast<unsigned>(g_objects.size()),
        static_cast<unsigned>(g_models.size()),
        g_models_lowered,
        verts,
        static_cast<unsigned>(verts * sizeof(mdl_vertex) / 1024));
}

/* --- geometry ------------------------------------------------------------- */

static void set_water(submesh_t& sm, shader_info const& si)
{
    if(!si.water)
        return;
    sm.water        = true;
    sm.ripple_img   = si.water->ripple;
    sm.ripple_scale = si.water->scale;
    sm.ripple_flow  = si.water->flow;
    sm.water_tint   = si.water->tint;
    sm.water_reflection_img = si.water->reflection;
    sm.water_steep          = si.water->steep;
    sm.water_grazing        = si.water->grazing;
}

/* Transcoded offline: the material's vertices are GX arrays already and its
 * indices big-endian */
static void append_gx_material(
    page_t&                    page,
    blam::bsp::material const& mat,
    blam::bsp::header const&   hdr,
    blam::bsp_ptr const&       bm,
    shader_info const&         si)
{
    u32 const count = from_le(mat.pc.count);
    auto      ref   = mat.vertices();
    ref.count       = blam::to_le<u32>(count * sizeof(gx_vertex));
    ref.offset      = mat.pc.compressed_vertices.offset;
    auto verts      = ref.data(bm);
    auto faces      = mat.indices(hdr).data(bm);
    if(!count || verts.has_error() || faces.has_error())
        return;
    u16 const vert_base = static_cast<u16>(page.verts.size());
    page.verts.resize(vert_base + count);
    std::memcpy(
        page.verts.data() + vert_base,
        verts.value().data(),
        count * sizeof(gx_vertex));
    Vecf3 mn{1e9f, 1e9f, 1e9f}, mx{-1e9f, -1e9f, -1e9f};
    for(u32 i = vert_base; i < page.verts.size(); i++)
    {
        auto const& v = page.verts[i];
        Vecf3 const p{v.pos[0], v.pos[1], v.pos[2]};
        mn = glm::min(mn, p);
        mx = glm::max(mx, p);
    }
    g_bb_min            = glm::min(g_bb_min, mn);
    g_bb_max            = glm::max(g_bb_max, mx);
    u32 const idx_start = static_cast<u32>(page.indices.size());
    for(auto const& f : faces.value())
    {
        page.indices.push_back(vert_base + f.x);
        page.indices.push_back(vert_base + f.y);
        page.indices.push_back(vert_base + f.z);
    }
    Vecf3 const center = (mn + mx) * 0.5f;
    page.submeshes.push_back(
        {idx_start,
         static_cast<u32>(page.indices.size()) - idx_start,
         center,
         glm::length(mx - center),
         si.diffuse,
         si.detail});
    set_water(page.submeshes.back(), si);
}

template<typename V>
static void append_material(
    blam::map_container<V> const& c,
    page_t&                       page,
    blam::bsp::material const&    mat,
    blam::bsp::header const&      hdr,
    blam::bsp_ptr const&          bm)
{
    namespace vert = blam::vert;
    if(from_le(mat.pc.type) == vert::vertex_type_t::sbsp_gx_vertex)
        return append_gx_material(
            page, mat, hdr, bm, resolve_shader(c, mat.shader));
    auto verts  = mat.vertices().data(bm);
    auto lverts = mat.light_verts().data(bm);
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
            gv.luv[0] = rd<i16>(lv + 4);
            gv.luv[1] = rd<i16>(lv + 6);
        } else // normal f32x3, then f32x2
        {
            gv.luv[0] = blam::gx::to_s16_frac15(rd<f32>(lv + 12));
            gv.luv[1] = blam::gx::to_s16_frac15(rd<f32>(lv + 16));
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
        set_water(page.submeshes.back(), si);
    }
}

template<typename V>
static void load_skies(
    blam::map_container<V> const& c, blam::scn::scenario<V> const* scn)
{
    auto skies = scn->info.skyboxes.data(c.magic);
    if(skies.has_error())
        return;
    auto v3 = [](Vecf3 const& v) {
        return Vecf3{from_le(v.x), from_le(v.y), from_le(v.z)};
    };
    for(auto const& ref : skies.value())
    {
        sky_t       sky;
        auto const* t = blam::tag_at(c, ref.tag_id);
        auto        sb =
            t ? std::optional(t->template data<blam::scn::skybox>(c.magic))
                     : std::nullopt;
        if(sb && !sb->has_error())
        {
            auto const& s   = *sb->value();
            sky.model       = s.model.tag_id;
            sky.fog_color   = v3(s.outdoor_fog.color);
            sky.fog_density = from_le(s.outdoor_fog.density);
            sky.fog_start   = from_le(s.outdoor_fog.start_distance);
            sky.fog_opaque  = from_le(s.outdoor_fog.opaque_distance);
            sky.ambient =
                v3(s.outdoor_ambient.color) * from_le(s.outdoor_ambient.power);
            if(auto lights = s.lights.data(c.magic);
               !lights.has_error() && !lights.value().empty())
            {
                auto const& r     = lights.value()[0].radiosity;
                f32 const   yaw   = from_le(r.direction.x);
                f32 const   pitch = from_le(r.direction.y);
                sky.sun_color     = v3(r.color) * from_le(r.power);
                sky.sun_dir       = -Vecf3{
                    std::cos(pitch) * std::cos(yaw),
                    std::cos(pitch) * std::sin(yaw),
                    std::sin(pitch)};
            }
            if(blam::tag_at(c, sky.model) &&
               g_models.find(sky.model) == g_models.end())
                load_model_cpu(c, sky.model);
        }
        g_skies.push_back(sky);
    }
    LOG("skies: %u", static_cast<unsigned>(g_skies.size()));
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
                           c.magic.base + name - c.magic.virtual_base))
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
    load_phase("map + stage tags");

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
    load_phase("parse");
    g_fingerprint = fingerprint_of(container);
    load_phase("fingerprint");
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
        loading_phase(0.3f, 0.4f);
        stage_hot_range("bsp", from_le(bi.offset), from_le(bi.size));
        auto const bm = bi.bsp_magic(container.magic);
        auto hdr_r    = bi.to_bsp(bm).to_header().data(bm, blam::single_value);
        if(hdr_r.has_error())
            continue;
        auto const* hdr = hdr_r.value();
        build_cluster_lookup(*hdr, bm);
        if(auto cls = hdr->clusters.data(bm); !cls.has_error())
            for(auto const& cl : cls.value())
                g_cluster_sky.push_back(from_le(cl.sky));
        LOG("cluster lookup: %u nodes, %u leaves",
            static_cast<unsigned>(g_coll_nodes.size()),
            static_cast<unsigned>(g_leaf_cluster.size()));
        auto const* bitm = resolve_bitm(container, hdr->lightmap_);

        auto lms = hdr->lightmaps.data(bm);
        if(lms.has_error())
            continue;
        loading_phase(0.4f, 0.6f);
        for(auto const& lm : lms.value())
        {
            loading_step(
                static_cast<u32>(&lm - lms.value().data()),
                static_cast<u32>(lms.value().size()));
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
    load_phase("bsp");

    loading_phase(0.6f, 0.7f);
    load_skies(container, *scn);
    build_objects(container, *scn);
    load_phase("objects");

    return g_pages.empty() ? 5 : 0;
}

static int extract_bsp()
{
    LOG("loading map bytes...");
    load_phase("start");
    loading_phase(0.f, 0.3f);
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
        blam::reference<libc_types::u8>{img.size, img.offset}
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
        blam::gx::tile_rgb565(
            reinterpret_cast<u16 const*>(pixels.data()),
            reinterpret_cast<u16*>(tiled),
            w,
            h,
            true);
    } else // BC1/BC2/BC3 -> GX_TF_CMPR (re-tile the DXT1 colour block)
    {
        bool const bc1 = fmt == fmt_t::BC1;
        blam::gx::dxt_to_cmpr(
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

/* A reflection cube's sphere map, baked again every frame */
struct reflection_t
{
    gx_reflection::cube_t             cube;
    u16*                              texels{nullptr};
    std::shared_ptr<gexxo::texture_t> sphere;
};

static constexpr u32 kSphereSize = 32;
static std::map<blam::bitm::image_t const*, std::shared_ptr<reflection_t>>
    g_reflections;

/* A cube bitmap's faces, shrunk to sample on the CPU */
static std::optional<gx_reflection::cube_t> load_cube(
    blam::bitm::image_t const* img)
{
    using fmt_t = blam::bitm::format_t;
    using sf    = gx_reflection::source_format;
    if(!img || from_le(img->type) != blam::bitm::type_t::tex_cube)
        return std::nullopt;
    u32 const w     = static_cast<u32>(from_le(img->isize.x));
    sf        fmt   = sf::bc1;
    u32       block = 0, bpp = 0;
    switch(from_le(img->format))
    {
    case fmt_t::BC1:
        block = 8;
        break;
    case fmt_t::BC2:
    case fmt_t::BC3:
        fmt   = sf::bc2_bc3;
        block = 16;
        break;
    case fmt_t::R5G6B5:
        fmt = sf::rgb565;
        bpp = 2;
        break;
    case fmt_t::ARGB8:
    case fmt_t::XRGB8:
        fmt = sf::bgra8;
        bpp = 4;
        break;
    default:
        return std::nullopt;
    }
    auto level_bytes = [&](u32 lw) {
        lw = std::max(lw, 1u);
        return block ? ((lw + 3) / 4) * ((lw + 3) / 4) * block : lw * lw * bpp;
    };
    std::vector<libc_types::u8> px;
    if(!w || !read_bitmap_pixels(*img, g_internal_magic, px))
        return std::nullopt;
    u32 const levels = std::max<u32>(from_le(img->mipmaps), 1);
    // Faces follow blam's bitm::image_t::cube_face_offset()
    auto face_offset = [&](u32 face) -> u32 {
        if(g_xbox)
        {
            u32 chain = 0;
            for(u32 i = 0; i < levels; i++)
                chain += level_bytes(w >> i);
            return face * ((chain + 127) & ~127u);
        }
        constexpr u32 stored[6] = {0, 2, 1, 3, 4, 5};
        return stored[face] * level_bytes(w);
    };
    bool const swizzled =
        !block && (static_cast<u16>(from_le(img->flags)) &
                   static_cast<u16>(blam::bitm::flags_t::swizzled));

    gx_reflection::cube_t cube;
    cube.size = std::min(w, 16u);
    for(u32 f = 0; f < 6; f++)
    {
        u32 const off = face_offset(f), n = level_bytes(w);
        if(off + n > px.size())
            return std::nullopt;
        u8 const*                   src = px.data() + off;
        std::vector<libc_types::u8> linear;
        if(swizzled)
        {
            linear.resize(n);
            blam::swizzle::deswizzle_bytes(
                semantic::Span<const u8>(src, n),
                semantic::Span<u8>(linear.data(), n),
                w,
                w,
                bpp);
            src = linear.data();
        }
        cube.faces[f] = gx_reflection::decode_face(src, fmt, w, cube.size);
    }
    return cube;
}

static std::shared_ptr<reflection_t> load_reflection(
    blam::bitm::image_t const* img)
{
    if(!img)
        return nullptr;
    if(auto it = g_reflections.find(img); it != g_reflections.end())
        return it->second;
    auto& slot = g_reflections[img];
    auto  cube = load_cube(img);
    if(!cube)
        return nullptr;
    auto r    = std::make_shared<reflection_t>();
    r->cube   = std::move(*cube);
    r->texels = static_cast<u16*>(memalign(32, kSphereSize * kSphereSize * 2));
    if(!r->texels)
        return nullptr;
    r->sphere = std::make_shared<gexxo::texture_t>(
        gexxo::textures::type::d2, typing::pixels::PixDesc{}, 1);
    r->sphere->init_raw(
        r->texels, kSphereSize, kSphereSize, GX_TF_RGB565, GX_CLAMP, GX_CLAMP);
    slot = r;
    return r;
}

/* Water's sky, see gx_reflection::bake_hemisphere(); one per water shader */
static constexpr u32 kWaterSkySize = 64;
static constexpr f32 kWaterSkyExtent =
    4.f; // tan of 76 degrees from straight up

static std::shared_ptr<gexxo::texture_t> load_water_sky(submesh_t const& sm)
{
    static std::map<
        std::tuple<blam::bitm::image_t const*, f32, f32>,
        std::shared_ptr<gexxo::texture_t>>
          skies;
    auto& slot = skies[{
        sm.water_reflection_img,
        glm::length(sm.water_steep),
        glm::length(sm.water_grazing)}];
    if(slot)
        return slot;
    auto cube = load_cube(sm.water_reflection_img);
    if(!cube)
        return nullptr;
    auto tex = std::make_shared<gexxo::texture_t>(
        gexxo::textures::type::d2,
        typing::pixels::PixDesc{typing::pixels::pix_fmt::RGB565},
        1);
    tex->alloc({kWaterSkySize, kWaterSkySize, 1});
    if(!tex->m_storage)
        return nullptr;
    gx_reflection::bake_hemisphere(
        *cube,
        kWaterSkySize,
        kWaterSkyExtent,
        sm.water_steep,
        sm.water_grazing,
        static_cast<u16*>(tex->m_storage));
    DCFlushRange(tex->m_storage, kWaterSkySize * kWaterSkySize * 2);
    slot = tex;
    return tex;
}

/* A few sphere maps per frame, in turn, for this frame's camera: at this
 * size a few frames of lag don't show */
static void bake_reflections(Matf4 const& view)
{
    static auto const dirs      = gx_reflection::eye_directions(kSphereSize);
    static u32        cursor    = 0;
    constexpr u32     per_frame = 1;
    if(g_reflections.empty())
        return;
    u32 const count = static_cast<u32>(g_reflections.size());
    auto      it    = g_reflections.begin();
    std::advance(it, cursor % count);
    for(u32 n = 0; n < std::min(per_frame, count); n++)
    {
        if(auto& r = it->second)
        {
            gx_reflection::bake(r->cube, glm::mat3(view), dirs, r->texels);
            DCFlushRange(r->texels, kSphereSize * kSphereSize * 2);
        }
        if(++it == g_reflections.end())
            it = g_reflections.begin();
    }
    cursor += per_frame;
    GX_InvalidateTexAll();
}

/* A chicago colour or alpha function as a TEV combiner, which works out
 * d op ((1 - c) * a + c * b): "current" is the result so far, "next map"
 * this stage's texture */
static gexxo::program_t::stage_t::combiner_t chicago_combiner(
    u16 function, bool alpha)
{
    using fn            = blam::shader::chicago::color_function;
    u8 const cur        = alpha ? GX_CA_APREV : GX_CC_CPREV;
    u8 const next       = alpha ? GX_CA_TEXA : GX_CC_TEXC;
    u8 const zero       = alpha ? GX_CA_ZERO : GX_CC_ZERO;
    u8 const cur_alpha  = alpha ? GX_CA_APREV : GX_CC_APREV;
    u8 const next_alpha = alpha ? GX_CA_TEXA : GX_CC_TEXA;
    gexxo::program_t::stage_t::combiner_t c{zero, zero, zero, cur};
    switch(static_cast<fn>(function))
    {
    case fn::current:
        break;
    case fn::next_map:
        c.d = next;
        break;
    case fn::double_multiply:
        c.scale = GX_CS_SCALE_2;
        [[fallthrough]];
    case fn::multiply:
        c.b = cur;
        c.c = next;
        c.d = zero;
        break;
    case fn::add_signed_current:
    case fn::add_signed_next_map:
        c.bias = GX_TB_SUBHALF;
        [[fallthrough]];
    case fn::add:
        c.a = next;
        break;
    case fn::subtract_current:
        c.a  = next;
        c.op = GX_TEV_SUB;
        break;
    case fn::subtract_next_map:
        c.a  = cur;
        c.d  = next;
        c.op = GX_TEV_SUB;
        break;
    case fn::blend_current_alpha:
        c = {next, cur, cur_alpha, zero};
        break;
    case fn::blend_current_alpha_inverse:
        c = {cur, next, cur_alpha, zero};
        break;
    case fn::blend_next_map_alpha:
        c = {cur, next, next_alpha, zero};
        break;
    case fn::blend_next_map_alpha_inverse:
        c = {next, cur, next_alpha, zero};
        break;
    }
    return c;
}

/* A stage per map: the first is taken as is, the rest combined into it */
static void build_chicago(gexxo::api& gfx, mdl_part& part)
{
    using stage_t   = gexxo::program_t::stage_t;
    using channel_t = gexxo::program_t::channel_t;
    using bind_t    = gexxo::texture_binding_t;
    auto prog       = gfx.alloc_program();
    prog->channels  = {channel_t{.channel = channel_t::color0a0}};
    for(u32 i = 0; i < part.chicago.size(); i++)
    {
        auto const& layer = part.chicago[i];
        auto        tex   = load_diffuse(layer.image);
        if(!tex)
            return;
        auto const tc =
            static_cast<stage_t::texcoord_t>(stage_t::texcoord0 + i);
        auto const tm = static_cast<stage_t::texmap_t>(stage_t::texmap0 + i);
        part.chicago_maps.push_back(
            bind_t{
                .texture  = tex,
                .texmap   = tm,
                .texcoord = tc,
                .gen_mtx  = static_cast<u32>(GX_TEXMTX0 + i * 3)});
        part.chicago_uv.push_back(
            glm::scale(
                glm::translate(Matf4{1.f}, Vecf3{layer.offset, 0.f}),
                Vecf3{layer.scale, 1.f}));
        stage_t stage{
            .stage    = static_cast<stage_t::id_t>(stage_t::stage0 + i),
            .op       = stage_t::custom,
            .texcoord = tc,
            .texmap   = tm,
            .color    = channel_t::color_null};
        if(i == 0)
        {
            stage.color_combiner = {
                GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC};
            stage.alpha_combiner = {
                GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA};
        } else
        {
            // A map's functions say how it combines with the map after it
            auto const& prev     = part.chicago[i - 1];
            stage.color_combiner = chicago_combiner(prev.color_function, false);
            stage.alpha_combiner = chicago_combiner(prev.alpha_function, true);
        }
        prog->stages.push_back(stage);
    }
    part.chicago_program = std::move(prog);
}

/* sotr parts compiled for the TEV; the rest keep their first map */
static u32 g_sotr_built = 0, g_sotr_failed = 0;

static void build_sotr(gexxo::api& gfx, mdl_part& part)
{
    using stage_t    = gexxo::program_t::stage_t;
    using bind_t     = gexxo::texture_binding_t;
    auto const& info = *part.sotr;
    if(info.stages.empty())
        return;
    auto compiled = gx_sotr::compile(info.stages);
    for(auto const& st : compiled.stages)
        if(st.texmap != stage_t::texmap_null &&
           st.texmap - stage_t::texmap0 >= static_cast<i32>(info.maps.size()))
            compiled.ok = false;
    if(!compiled.ok)
    {
        g_sotr_failed++;
        return;
    }
    std::vector<bind_t> maps;
    for(u32 i = 0; i < info.maps.size(); i++)
    {
        auto tex = load_diffuse(info.maps[i].image);
        if(!tex)
        {
            g_sotr_failed++;
            return;
        }
        maps.push_back(
            bind_t{
                .texture = tex,
                .texmap  = static_cast<stage_t::texmap_t>(stage_t::texmap0 + i),
                .texcoord =
                    static_cast<stage_t::texcoord_t>(stage_t::texcoord0 + i),
                .gen_mtx = static_cast<u32>(GX_TEXMTX0 + i * 3)});
    }
    auto prog         = gfx.alloc_program();
    prog->channels    = {gexxo::program_t::channel_t{}};
    prog->stages      = std::move(compiled.stages);
    part.sotr_program = std::move(prog);
    part.sotr_maps    = std::move(maps);
    part.sotr_konst   = std::move(compiled.konst);
    g_sotr_built++;
}

/* A posable part's batches, see gx_skin::split() */
static u32 g_batches = 0;

static void build_batches(gexxo::api& gfx, mdl_part& part)
{
    using vertex_t   = gx_skin::batch_vertex<mdl_vertex>;
    using attr_t     = gexxo::vertex_attribute;
    constexpr u64 at = offsetof(vertex_t, v);
    for(auto& b : gx_skin::split(part.verts, part.indices))
    {
        mdl_part::batch_t out;
        out.vao = gfx.alloc_vertex_array();
        out.vbo = gfx.alloc_buffer(gexxo::buffers::vertex, RSCA::ReadOnly);
        out.ebo = gfx.alloc_buffer(gexxo::buffers::element, RSCA::ReadOnly);
        out.index_count = static_cast<u32>(b.triangles.size());
        out.keys        = std::move(b.keys);
        out.vbo->commit(
            gsl::span<vertex_t const>(b.verts.data(), b.verts.size()));
        out.ebo->commit(
            gsl::span<u16 const>(b.triangles.data(), b.triangles.size()));
        // GX order: matrix index, POS, NRM, TEX0
        out.vao->add({
            .index = 0,
            .role  = attr_t::matrix_index,
            .value =
                {.offset = 0,
                 .stride = sizeof(vertex_t),
                 .count  = 1,
                 .type   = semantic::type_t::u8},
        });
        out.vao->add({
            .index = 1,
            .role  = attr_t::position,
            .value = {.offset = at, .stride = sizeof(vertex_t), .count = 3},
        });
        out.vao->add({
            .index = 2,
            .role  = attr_t::normal,
            .value =
                {.offset = at + offsetof(mdl_vertex, nrm),
                 .stride = sizeof(vertex_t),
                 .count  = 3,
                 .type   = semantic::type_t::i8,
                 .frac   = 6},
        });
        out.vao->add({
            .index = 3,
            .role  = attr_t::texcoord0,
            .value =
                {.offset = at + offsetof(mdl_vertex, uv),
                 .stride = sizeof(vertex_t),
                 .count  = 2},
        });
        out.vao->set_buffer(gexxo::buffers::vertex, out.vbo, 0);
        out.vao->set_buffer(gexxo::buffers::element, out.ebo);
        part.batches.push_back(std::move(out));
        g_batches++;
    }
}

/* An animation function over its period, 0..1 */
static f32 sotr_wave(u16 function, f32 period, f32 t)
{
    using fn = blam::shader::animation_function;
    switch(static_cast<fn>(function))
    {
    case fn::one:
        return 1.f;
    case fn::zero:
        return 0.f;
    default:
        break;
    }
    if(period == 0.f)
        return 0.5f;
    f32 const phase = glm::fract(t / period);
    switch(static_cast<fn>(function))
    {
    case fn::slide:
    case fn::slide_variable:
        return 1.f - std::abs(2.f * phase - 1.f);
    case fn::cosine:
    case fn::cosine_variable:
        return std::cos(phase * glm::two_pi<f32>()) * 0.5f + 0.5f;
    default:
        return 0.5f;
    }
}

/* Constant 0's colour is stage 0's, its alpha each stage's own; unset
 * constants are white, so they multiply to nothing */
static Vecf4 sotr_konst(
    sotr_info const& info, gx_sotr::konst_recipe_t const& r, f32 t)
{
    auto unset = [](Vecf4 const& a, Vecf4 const& b) {
        return a == Vecf4{0.f} && b == Vecf4{0.f};
    };
    auto const& own = info.constants[r.stage];
    if(r.which == 1)
        return unset(own.color1, own.color1) ? Vecf4{1.f} : own.color1;
    auto value = [&](sotr_info::constants_t const& k) {
        if(unset(k.lower, k.upper))
            return Vecf4{1.f};
        f32 const f =
            k.object_driven ? 0.f : sotr_wave(k.function, k.period, t);
        return glm::mix(k.lower, k.upper, f);
    };
    auto const& s0  = info.constants[0];
    Vecf3       rgb = Vecf3(value(s0));
    if(s0.object_driven && !unset(s0.lower, s0.upper))
        rgb = glm::mix(rgb, Vecf3{1.f}, 0.35f); // pale, not fully saturated
    return Vecf4(rgb, value(own).a);
}

static Matf4 sotr_uv(sotr_info::map_t const& m, f32 t)
{
    using fn    = blam::shader::animation_function;
    auto offset = [t](sotr_info::uv_anim_t const& a) {
        if(a.period == 0.f)
            return 0.f;
        f32 const phase = glm::fract(t / a.period);
        switch(static_cast<fn>(a.function))
        {
        case fn::slide:
        case fn::slide_variable:
            return phase * a.scale;
        case fn::cosine:
        case fn::cosine_variable:
            return std::cos(phase * glm::two_pi<f32>()) * a.scale;
        default:
            return 0.f;
        }
    };
    Vecf2 const scroll{offset(m.u), offset(m.v)};
    return glm::scale(
        glm::translate(Matf4{1.f}, Vecf3{m.offset + scroll, 0.f}),
        Vecf3{m.scale, 1.f});
}

// Fly-cam state (Halo world space, Z up), driven by the GameCube pad.
static bool  g_cam_ready = false;
static f32   g_secs      = 0.f; // since the first frame, for animated shaders
static Vecf3 g_cam_pos{};
static f32   g_cam_yaw   = 0.f; // around +Z
static f32   g_cam_pitch = 0.f; // up/down

/* Walking: the server moves the camera with its biped, from our controls */
static bool  g_walking     = false;
static bool  g_walk_toggle = false;
static Vecf2 g_walk_move{}; // forward, right
static u32   g_walk_intents = 0; // InputIntent

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
 * look, A/B = up/down, R = speed boost, Z = walk/fly when joined. Walking, A
 * jumps. dt in seconds. No-op without a pad (headless), so the camera just
 * sits at the spawn. */
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

    u16 const btn = PAD_ButtonsHeld(0);
    if(PAD_ButtonsDown(0) & PAD_TRIGGER_Z)
        g_walk_toggle = true;
    if(g_walking)
    {
        g_walk_move = {ly, lx};
        // A reloads and picks up alike
        // Triggers by how far they're pulled, before the click
        g_walk_intents =
            (PAD_TriggerR(0) > 100 ? std::to_underlying(InputIntent::fire)
                                   : 0u) |
            (PAD_TriggerL(0) > 100 ? std::to_underlying(InputIntent::grenade)
                                   : 0u);
        for(auto [button, intent] : {
                std::pair{PAD_BUTTON_X, InputIntent::jump},
                std::pair{PAD_BUTTON_B, InputIntent::melee},
                std::pair{PAD_BUTTON_A, InputIntent::use | InputIntent::reload},
                std::pair{PAD_BUTTON_Y, InputIntent::swap},
            })
            if(btn & button)
                g_walk_intents |= std::to_underlying(intent);
        return;
    }
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

static GXColor to_gx(Vecf3 const& c, f32 scale = 255.f)
{
    auto ch = [scale](f32 v) {
        return static_cast<u8>(std::clamp(v * scale, 0.f, 255.f));
    };
    return {ch(c.r), ch(c.g), ch(c.b), 0xFF};
}

/* The sky's sun for the objects, in eye space (GX lights are) */
static void load_sun(Matf4 const& view, sky_t const& sky)
{
    Vecf3 const eye = glm::mat3(view) * glm::normalize(sky.sun_dir);
    GXLightObj  sun;
    // A far point light is how GX does a directional one
    GX_InitLightPos(&sun, -eye.x * 1e5f, -eye.y * 1e5f, -eye.z * 1e5f);
    GX_InitLightColor(&sun, to_gx(sky.sun_color));
    GX_LoadLightObj(&sun, GX_LIGHT0);
    GX_SetChanAmbColor(GX_COLOR0A0, to_gx(sky.ambient));
}

/* The sky of the camera's cluster; indoors (no sky) gets none */
static sky_t const* active_sky()
{
    static sky_t const fallback{};
    if(g_skies.empty())
        return &fallback;
    auto const cluster = cluster_at(g_cam_pos);
    if(!cluster)
        return &g_skies[0]; // outside the BSP: keep it simple
    if(*cluster >= g_cluster_sky.size())
        return &g_skies[0];
    i16 const idx = g_cluster_sky[*cluster];
    return idx >= 0 && static_cast<u32>(idx) < g_skies.size() ? &g_skies[idx]
                                                              : nullptr;
}

/* Halo fog thickens to `density` at the opaque distance; GX fog is linear
 * to full colour at its end, so the end moves out for thin fog. The clip
 * plane is at 100 units, which the fog hides */
static void set_fog(sky_t const* sky)
{
    if(!sky || sky->fog_density <= 0.f || sky->fog_opaque <= sky->fog_start)
    {
        GX_SetFog(GX_FOG_NONE, 0, 1, 0.1f, 100.f, GXColor{0, 0, 0, 0xFF});
        return;
    }
    f32 const density = std::min(sky->fog_density, 1.f);
    f32 const end =
        sky->fog_start + (sky->fog_opaque - sky->fog_start) / density;
    GX_SetFog(
        GX_FOG_LIN,
        sky->fog_start,
        std::min(end, 100.f),
        0.1f,
        100.f,
        to_gx(sky->fog_color));
}
#endif

#if defined(COFFEE_GEKKO)
/* --- network play through BlamGCGateway ----------------------------------- */

static gx_net::client_t      g_net;
static std::map<u32, size_t> g_net_objects;    // net id -> g_objects
static std::map<u32, size_t> g_remote_players; // player idx -> g_objects

/* What a remote player's biped is doing, from where the server puts it */
struct posed_t
{
    Vecf3              last{0.f}; // feet
    f32                last_time{-1.f};
    Vecf3              velocity{0.f};
    Vecf3              facing{1.f, 0.f, 0.f};
    i32                clip{-1};
    f32                frame{0.f};
    std::vector<Matf4> skin;
    i32                skinned_clip{-1}; // what `skin` is a pose of
    u32                skinned_frame{0};
};

static std::map<size_t, posed_t> g_posed; // by g_objects index

/* Weapons in players' hands, and shots in flight, as the server reports */
static std::map<u32, blam::tag_index<>> g_held; // player idx -> weap
static std::map<u32, Vecf3>             g_aim;  // player idx -> look direction

struct projectile_t
{
    Vecf3             pos, vel;
    Vecf3             prev; // last frame's position, the streak's tail
    f32               gravity{0.f};
    blam::tag_index<> tag, model;
    u32               shooter{0};
    f32               age{0.f};
    bool              resting{false}; // stopped by the structure
};

static std::vector<projectile_t> g_projectiles;
static constexpr f32             kGravity        = 3.27f; // wu/s^2, 9.8 m/s^2
static constexpr f32             kProjectileLife = 3.f;   // s, if nothing hits
static gx_effects::system_t      g_fx{g_fx_lib};
static gx_sprites::batcher_t     g_sprites;
static constexpr f32         kEyeHeight = 0.62f;
/* From the bind pose's hanging hand up to where an aiming one is */
static Vecf4 const kAimingHand{0.1f, 0.f, 0.15f, 0.f};

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

/* Bullets without a model: a streak over the last frame's flight, as a quad
 * facing the eye. Built in eye space and cut at the near plane, as muzzles
 * sit by the camera */
static void draw_tracers(Matf4 const& mv)
{
    constexpr f32 near = 0.2f, half_width = 0.02f;

    struct streak_t
    {
        Vecf3 head, tail;
    };

    std::vector<streak_t> streaks;
    for(auto const& p : g_projectiles)
    {
        auto const entry = g_catalog.find(p.tag);
        if(g_models.contains(p.model) || p.resting ||
           (entry != g_catalog.end() && entry->second.volume))
            continue;
        Vecf3 head = Vecf3(mv * Vecf4(p.pos, 1.f));
        Vecf3 tail = Vecf3(mv * Vecf4(p.prev, 1.f));
        if(head.z > -near && tail.z > -near)
            continue;
        if(head.z > -near)
            head = glm::mix(tail, head, (-near - tail.z) / (head.z - tail.z));
        else if(tail.z > -near)
            tail = glm::mix(head, tail, (-near - head.z) / (tail.z - head.z));
        streaks.push_back({head, tail});
    }
    if(streaks.empty())
        return;
    Mtx id;
    guMtxIdentity(id);
    GX_LoadPosMtxImm(id, GX_PNMTX0);
    GX_SetCurrentMtx(GX_PNMTX0);
    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetNumChans(1);
    GX_SetChanCtrl(
        GX_COLOR0A0,
        GX_DISABLE,
        GX_SRC_VTX,
        GX_SRC_VTX,
        0,
        GX_DF_NONE,
        GX_AF_NONE);
    GX_SetNumTexGens(0);
    GX_SetNumTevStages(1);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_CLEAR);
    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    GX_SetCullMode(GX_CULL_NONE);
    GX_Begin(GX_QUADS, GX_VTXFMT0, static_cast<u16>(streaks.size() * 4));
    for(auto const& s : streaks)
    {
        // Widen across the streak, as seen from the eye
        Vecf3 side = glm::cross(s.head - s.tail, s.head);
        side = glm::length(side) > 1e-6f ? glm::normalize(side) * half_width
                                         : Vecf3{half_width, 0, 0};
        Vecf3 const v[4] = {
            s.head - side, s.head + side, s.tail + side, s.tail - side};
        for(u32 i = 0; i < 4; i++)
        {
            GX_Position3f32(v[i].x, v[i].y, v[i].z);
            if(i < 2)
                GX_Color4u8(0xFF, 0xE8, 0x90, 0xFF);
            else
                GX_Color4u8(0xFF, 0xA0, 0x40, 0x00);
        }
    }
    GX_End();
    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_SetCullMode(GX_CULL_BACK);
}

/* Where a flight from a to b first meets the structure, if it does */
static std::optional<Vecf3> structure_hit(Vecf3 const& a, Vecf3 const& b)
{
    if(g_coll_nodes.empty())
        return std::nullopt;
    u32 const steps =
        std::clamp<u32>(static_cast<u32>(glm::distance(a, b) / 0.5f), 1, 16);
    Vecf3 last = a;
    for(u32 i = 1; i <= steps; i++)
    {
        Vecf3 const at = glm::mix(a, b, static_cast<f32>(i) / steps);
        if(!cluster_at(at))
            return last;
        last = at;
    }
    return std::nullopt;
}

/* Held weapons on remote bipeds, then shots in flight */
template<typename DrawModel>
static void draw_combat(Matf4 const& mv, f32 dt, DrawModel&& draw_model)
{
    for(auto const& [player, weapon] : g_held)
    {
        auto biped = g_remote_players.find(player);
        auto entry = g_catalog.find(weapon);
        if(biped == g_remote_players.end() || entry == g_catalog.end() ||
           !g_models.contains(entry->second.model))
            continue;
        Matf4 const& body = g_objects[biped->second].xform;
        // In the posed hand
        if(auto pose = g_posed.find(biped->second);
           pose != g_posed.end() && g_hand_node >= 0 &&
           static_cast<size_t>(g_hand_node) < pose->second.skin.size())
        {
            draw_model(
                g_models[entry->second.model],
                mv * body * pose->second.skin[g_hand_node] * g_hand,
                true);
            continue;
        }
        // Unposed: at the hand, along the aim
        auto const   aim  = g_aim.find(player);
        Vecf3 const  x    = aim != g_aim.end() ? aim->second : Vecf3(body[0]);
        Vecf3 const  y    = glm::normalize(glm::cross(Vecf3{0, 0, 1}, x));
        Vecf3 const  hand = Vecf3(body * (g_hand[3] + kAimingHand));
        draw_model(
            g_models[entry->second.model],
            mv *
                Matf4{
                    Vecf4(x, 0),
                    Vecf4(y, 0),
                    Vecf4(glm::cross(x, y), 0),
                    Vecf4(hand, 1)},
            true);
    }

    for(auto& p : g_projectiles)
    {
        p.age += dt;
        p.prev = p.pos;
        if(p.resting)
            continue;
        p.vel.z -= kGravity * p.gravity * dt;
        Vecf3 const next = p.pos + p.vel * dt;
        auto const  hit  = structure_hit(p.pos, next);
        p.pos            = hit.value_or(next);
        p.resting        = hit.has_value();
    }
    // Shots end on the structure, grenades when their fuse runs out
    std::erase_if(g_projectiles, [](projectile_t const& p) {
        auto const entry = g_catalog.find(p.tag);
        f32 const  fuse  = entry != g_catalog.end() ? entry->second.fuse : 0.f;
        bool const done =
            fuse > 0.f ? p.age >= fuse : p.resting || p.age > kProjectileLife;
        if(done && entry != g_catalog.end() && entry->second.detonation &&
           (p.resting || fuse > 0.f))
            g_fx.spawn(
                *entry->second.detonation,
                p.pos,
                glm::length(p.vel) > 1e-3f ? -p.vel : Vecf3{0, 0, 1});
        return done;
    });
    g_fx.step(dt);
    g_fx.draw(g_sprites);
    for(auto const& p : g_projectiles)
    {
        auto const entry = g_catalog.find(p.tag);
        if(entry != g_catalog.end() && entry->second.volume && !p.resting &&
           glm::length(p.vel) > 1e-3f)
            g_fx.volume(
                g_sprites, *entry->second.volume, p.pos, glm::normalize(p.vel));
        if(!g_models.contains(p.model))
            continue;
        // Models face +X; point that along the flight
        Vecf3 const x = glm::normalize(p.vel);
        Vecf3 const up =
            std::abs(x.z) > 0.99f ? Vecf3{1, 0, 0} : Vecf3{0, 0, 1};
        Vecf3 const y = glm::normalize(glm::cross(up, x));
        Matf4       m{
            Vecf4(x, 0.f),
            Vecf4(y, 0.f),
            Vecf4(glm::cross(x, y), 0.f),
            Vecf4(p.pos, 1.f)};
        draw_model(g_models[p.model], mv * m, true);
    }
    draw_tracers(mv);
}

static bool sample_clip(
    clip_t const&       cl,
    u32                 frame,
    std::vector<Quatf>& rot,
    std::vector<Vecf3>& trans)
{
    auto const& h     = cl.header;
    u32 const   nodes = static_cast<u32>(from_le(h.node_count));
    semantic::Span<const u8> const bytes(cl.frames.data(), cl.frames.size());
    semantic::Span<const u8> const defaults(
        cl.defaults.data(), cl.defaults.size());
    return h.is_compressed()
               ? blam::antr::sample_compressed(
                     h, bytes, frame, nodes, rot, trans)
               : blam::antr::sample_uncompressed(
                     h, defaults, bytes, frame, nodes, rot, trans);
}

/* Advance a clip's playhead; false once a clip that doesn't loop is over */
static bool advance_clip(clip_t const& cl, f32& frame, f32 dt, bool loop)
{
    auto const& h      = cl.header;
    i32 const   frames = from_le(h.frame_count);
    if(frames <= 0)
        return false;
    frame += dt * h.frame_rate();
    if(frame < frames)
        return true;
    if(!loop)
    {
        frame = static_cast<f32>(frames - 1);
        return false;
    }
    f32 const looped =
        std::max(static_cast<f32>(frames - h.loop_start_frame()), 1.f);
    while(frame >= frames)
        frame -= looped;
    return true;
}

/* Remote bipeds walk the way they move, holding what they hold */
static void update_poses(f32 dt, frustum_t const& view_frustum)
{
    auto model = g_models.find(g_player_model);
    if(model == g_models.end() || model->second.skeleton.bones.empty() ||
       g_stances.empty())
        return;
    auto const&               skeleton = model->second.skeleton;
    static std::vector<Quatf> rot;
    static std::vector<Vecf3> trans;
    // Players by their object, and the bipeds placed on the spawns
    constexpr u32 placed = ~0u;
    // Bipeds on the same frame of a clip share its pose
    std::map<std::pair<i32, u32>, std::vector<Matf4> const*> shared;
    std::map<size_t, u32>                                    bipeds;
    for(size_t i = 0; i < g_objects.size(); i++)
        if(g_objects[i].model_id == g_player_model)
            bipeds[i] = placed;
    for(auto const& [player, index] : g_remote_players)
        bipeds[index] = player;
    for(auto const& [index, player] : bipeds)
    {
        auto& pose = g_posed[index];
        // The server only says when it moves
        if(g_secs - pose.last_time > 0.25f)
            pose.velocity = Vecf3{0.f};
        std::string label = kUnarmed;
        if(auto held = g_held.find(player); held != g_held.end())
            if(auto e = g_catalog.find(held->second);
               e != g_catalog.end() && !e->second.label.empty())
                label = e->second.label;
        auto st = g_stances.find(label);
        if(st == g_stances.end())
            st = g_stances.find(kUnarmed);
        if(st == g_stances.end())
            st = g_stances.begin();
        stance_t const& s = st->second;

        // Its move in its own frame: facing +X, right -Y
        Vecf2 const v{pose.velocity.x, pose.velocity.y};
        Vecf2 const f{pose.facing.x, pose.facing.y};
        Vecf2 const r{f.y, -f.x};
        f32 const   along = glm::dot(v, f), across = glm::dot(v, r);
        i32         clip = s.idle;
        if(glm::length(v) > 0.4f)
        {
            if(std::abs(along) >= std::abs(across))
                clip = along > 0.f ? s.front : s.back;
            else
                clip = across > 0.f ? s.right : s.left;
        }
        if(clip < 0)
            clip = s.idle;
        if(clip < 0)
            continue;
        if(clip != pose.clip)
        {
            pose.clip  = clip;
            pose.frame = 0.f;
        }
        if(!advance_clip(g_clips[clip], pose.frame, dt, true))
            continue;
        u32 const frame = static_cast<u32>(pose.frame);
        // Clips run at 30 Hz: most frames, the pose is still the last one
        if(pose.skinned_clip == clip && pose.skinned_frame == frame &&
           !pose.skin.empty())
            continue;
        // Out of view, it can catch up when it's back
        auto const& obj = g_objects[index];
        if(!sphere_visible(view_frustum, obj.center, obj.radius))
            continue;
        pose.skinned_clip  = clip;
        pose.skinned_frame = frame;
        if(auto same = shared.find({clip, frame}); same != shared.end())
        {
            pose.skin = *same->second;
            continue;
        }
        if(sample_clip(g_clips[clip], frame, rot, trans))
        {
            pose.skin             = gx_skin::skin(skeleton, rot, trans);
            shared[{clip, frame}] = &pose.skin;
        }
    }
}

/* Our own arms and weapon, while walking */
struct first_person_pose_t
{
    blam::tag_index<>  weapon{blam::tag_index<>::none()};
    i32                clip{-1};
    f32                frame{0.f};
    bool               fired{false}; // a shot of ours since the last frame
    std::vector<Matf4> hands, held;  // skin matrices
};

static first_person_pose_t g_fp_pose;

/* First-person models are eye space with +X ahead, +Y left, +Z up */
static Matf4 const kViewFromFirstPerson{
    Vecf4{0.f, 0.f, -1.f, 0.f},
    Vecf4{-1.f, 0.f, 0.f, 0.f},
    Vecf4{0.f, 1.f, 0.f, 0.f},
    Vecf4{0.f, 0.f, 0.f, 1.f}};

using lens_t = gexxo::draw_command::perspective_t;

/* Halo's 70 degrees across, and a near plane the hands are behind */
static lens_t const kFirstPersonLens{
    .fov_y = 55.f, .aspect = 4.f / 3.f, .z_near = 0.01f, .z_far = 10.f};

static first_person_t const* update_first_person(f32 dt)
{
    auto held  = g_held.find(g_net.player_idx());
    auto fp    = held != g_held.end() ? g_first_person.find(held->second)
                                      : g_first_person.end();
    auto hands = g_models.find(g_hands);
    if(fp == g_first_person.end() || hands == g_models.end())
        return nullptr;
    auto& pose = g_fp_pose;
    if(pose.weapon != fp->first)
        pose = {.weapon = fp->first, .clip = fp->second.idle};
    if(std::exchange(pose.fired, false) && fp->second.fire >= 0)
    {
        pose.clip  = fp->second.fire;
        pose.frame = 0.f;
    }
    bool const idle = pose.clip == fp->second.idle;
    if(!advance_clip(g_clips[pose.clip], pose.frame, dt, idle) && !idle)
    {
        pose.clip  = fp->second.idle;
        pose.frame = 0.f;
    }
    static std::vector<Quatf> rot;
    static std::vector<Vecf3> trans;
    if(!sample_clip(
           g_clips[pose.clip], static_cast<u32>(pose.frame), rot, trans))
        return nullptr;
    auto const world = gx_skin::model_space(fp->second.nodes, &rot, &trans);
    auto       skin  = [&world](
                    gx_skin::skeleton_t const& s,
                    std::vector<i16> const&    nodes,
                    std::vector<Matf4>&        out) {
        out.resize(nodes.size());
        for(size_t b = 0; b < nodes.size() && b < s.bind_inverse.size(); b++)
            out[b] = static_cast<size_t>(nodes[b]) < world.size()
                         ? world[nodes[b]] * s.bind_inverse[b]
                         : Matf4{1.f};
    };
    skin(hands->second.skeleton, fp->second.hands, pose.hands);
    if(auto m = g_models.find(fp->second.model); m != g_models.end())
        skin(m->second.skeleton, fp->second.weapon, pose.held);
    return &fp->second;
}

static void poll_network(u32 frame)
{
    gx_net::fingerprint_t const fp{
        {g_fingerprint.tag_hash[0], g_fingerprint.tag_hash[1]},
        g_fingerprint.tag_count,
        g_fingerprint.base_tag};
    gx_net::handlers_t on;
    /* Joined, the server places what is networked; scenery, lights and
     * devices stay as the map has them. Its spawns show them again */
    on.game_join = [] {
        u32 hidden = 0;
        for(auto& obj : g_objects)
        {
            u32 const group = obj.net_id >> 24;
            if(obj.offline || (group >= g_kind_net_group[kind_vehicle] &&
                               group <= g_kind_net_group[kind_netgame_item]))
            {
                obj.hidden = true;
                hidden++;
            }
        }
        LOG("net: %u objects wait for the server", hidden);
    };
    on.object = [](gx_net::object_sync_t const& o) {
        static std::set<u32> unknown_ids;
        auto                 it = g_net_objects.find(o.net_id);
        if(it == g_net_objects.end())
        {
            if(unknown_ids.insert(o.net_id).second)
                LOG("net: no object %08x to move", o.net_id);
            return;
        }
        place_instance(
            g_objects[it->second],
            glm::translate(Matf4{1.f}, o.position) *
                glm::mat4_cast(o.rotation));
    };
    on.spawn = [](gx_net::entity_spawn_t const& e) {
        auto it = g_net_objects.find(e.net_id);
        if(it == g_net_objects.end())
        {
            // Only what the catalogue has: the map is let go by now
            auto entry = g_catalog.find(e.tag_id);
            if(entry == g_catalog.end() ||
               !g_models.contains(entry->second.model))
            {
                LOG("net: no model for spawned object %08x (tag %08x)",
                    e.net_id,
                    e.tag_id.value());
                return;
            }
            g_objects.push_back(
                {entry->second.model,
                 Matf4{1.f},
                 {},
                 g_models[entry->second.model].radius,
                 e.net_id});
            it = g_net_objects.emplace(e.net_id, g_objects.size() - 1).first;
        }
        g_objects[it->second].hidden = false;
        place_instance(
            g_objects[it->second],
            glm::translate(Matf4{1.f}, e.position) *
                glm::mat4_cast(e.rotation));
    };
    // What the server names that the catalogue lacks, said once each
    static std::set<u32> unknown;
    auto                 known = [](blam::tag_index<> tag, char const* what) {
        auto it = g_catalog.find(tag);
        if(it != g_catalog.end() && g_models.contains(it->second.model))
            return true;
        if(unknown.insert(tag.value()).second)
            LOG("net: no model for %s %08x", what, tag.value());
        return it != g_catalog.end();
    };
    on.equipment = [known](gx_net::equipment_t const& e) {
        if(g_held[e.player_idx] != e.held && e.held.valid())
        {
            LOG("net: player %u holds %08x", e.player_idx, e.held.value());
            known(e.held, "held weapon");
        }
        g_held[e.player_idx] = e.held;
    };
    on.shot = [known](gx_net::shot_t const& s) {
        auto weapon = g_catalog.find(s.weapon);
        if(!known(s.weapon, s.thrown ? "thrown projectile" : "fired weapon") ||
           weapon == g_catalog.end())
            return;
        // A throw names its projectile, a shot its weapon
        blam::tag_index<> const proj =
            s.thrown ? s.weapon : weapon->second.projectile;
        if(s.player_idx == g_net.player_idx() && !s.thrown)
            g_fp_pose.fired = true;
        auto p = g_catalog.find(proj);
        if(p == g_catalog.end())
            return;
        f32 const speed = std::max(p->second.speed, 1.f);
        g_projectiles.push_back({
            .pos     = s.position,
            .vel     = glm::normalize(s.direction) * speed,
            .prev    = s.position,
            .gravity = p->second.gravity,
            .tag     = proj,
            .model   = p->second.model,
            .shooter = s.player_idx,
        });
    };
    on.hit = [](gx_net::hit_t const& h) {
        auto it = std::find_if(
            g_projectiles.begin(), g_projectiles.end(), [&](auto const& p) {
                return p.shooter == h.shooter_player && p.tag == h.projectile;
            });
        if(it == g_projectiles.end())
            return;
        if(auto e = g_catalog.find(it->tag);
           e != g_catalog.end() && e->second.detonation)
            g_fx.spawn(*e->second.detonation, h.point, -it->vel);
        g_projectiles.erase(it);
    };
    on.despawn = [](u32 net_id) {
        if(auto it = g_net_objects.find(net_id); it != g_net_objects.end())
            g_objects[it->second].hidden = true;
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
        if(glm::length(c.forward) > 0.f)
            g_aim[c.player_idx] = glm::normalize(c.forward);
        // Its speed, from successive positions
        auto&       pose = g_posed[it->second];
        Vecf3 const feet = c.position - Vecf3{0.f, 0.f, kEyeHeight};
        f32 const   gap  = g_secs - pose.last_time;
        if(pose.last_time < 0.f || gap > 0.5f)
        {
            pose.last      = feet;
            pose.last_time = g_secs;
        } else if(gap > 0.03f)
        {
            pose.velocity =
                glm::mix(pose.velocity, (feet - pose.last) / gap, 0.5f);
            pose.last      = feet;
            pose.last_time = g_secs;
        }
        pose.facing = {std::cos(yaw), std::sin(yaw), 0.f};
        place_instance(
            g_objects[it->second],
            glm::rotate(
                glm::translate(
                    Matf4{1.f}, c.position - Vecf3{0.f, 0.f, kEyeHeight}),
                yaw,
                Vecf3{0.f, 0.f, 1.f}));
    };
    on.teleport = [](gx_net::camera_sync_t const& c) {
        if(g_walking)
        {
            g_cam_pos = c.position; // where our body took us; the look is ours
            return;
        }
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
    bool const joined = g_net.state() == gx_net::client_t::state_t::joined;
    if(std::exchange(g_walk_toggle, false) && joined)
    {
        g_walking = !g_walking;
        LOG("net: %s", g_walking ? "walking" : "flying");
        g_net.send_camera(g_cam_pos, cam_forward(), g_walking);
    }
    g_walking &= joined;
    g_net.poll(fp, on, frame);
    if(g_walking)
        g_net.send_input(
            g_walk_move,
            cam_forward(),
            static_cast<InputIntent>(g_walk_intents));
    else if(frame % 2 == 0)
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
#if defined(COFFEE_GEKKO)
    start_loading_screen(gfx);
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
    // TODO: Geometry stays resident twice, in page/part vectors and in the
    // buffers' copies (~1-2MB on b30). Drop the vectors after commit, keep
    // only the counts the draws need; reserve() them to skip growth slack.
    u32 diffuse_count = 0;
    u32 mdl_parts     = 0;
    try
    {
        loading_phase(0.7f, 1.f);
        u32 const gpu_steps =
            static_cast<u32>(g_pages.size() + g_models.size());
        u32 gpu_step = 0;
        for(u32 pi = 0; pi < g_pages.size(); pi++)
        {
            loading_step(gpu_step++, gpu_steps);
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
                     .count  = 2,
                     .type   = semantic::type_t::i16,
                     .frac   = 15},
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
                sm.ripple  = load_diffuse(sm.ripple_img);
                if(sm.water)
                    sm.water_sky = load_water_sky(sm);
                if(sm.diffuse)
                    diffuse_count++;
            }
        }
        LOG("gpu: %u submeshes with diffuse, %u unique diffuse tex",
            diffuse_count,
            static_cast<unsigned>(g_diffuse_cache.size()));

        // Model parts -> GPU (position + normal + diffuse UV; strip indices).
        for(auto& [id, model] : g_models)
        {
            loading_step(gpu_step++, gpu_steps);
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
                part.diffuse    = load_diffuse(part.diffuse_img);
                part.reflection = load_reflection(part.reflection_img);
                if(part.reflection)
                    part.multipurpose = load_diffuse(part.multipurpose_img);
                if(part.chicago.size() > 1)
                    build_chicago(gfx, part);
                if(part.sotr)
                    build_sotr(gfx, part);
                if(!model.skeleton.bones.empty())
                    build_batches(gfx, part);
                mdl_parts++;
            }
        }
        for(u32 i = 0; i < g_fx_lib.images.size(); i++)
            g_fx_lib.frames[i].texture = g_fx_lib.images[i]
                                             ? load_diffuse(g_fx_lib.images[i])
                                             : g_sprites.glow();
        g_fx_lib.images.clear();
    } catch(std::bad_alloc const&)
    {
        // Out of MEM1: render whatever resources were built (the rest stay
        // null and are simply skipped). Big maps don't fully fit in 24MB.
        LOG("gpu: out of memory, rendering partial scene");
    }
    load_phase("gpu");
#if defined(COFFEE_GEKKO)
    g_loading.finish();
    platform::file::gekko::vmem::set_stage_progress(nullptr);
#endif
    LOG("reflections: %u cubes", static_cast<unsigned>(g_reflections.size()));
    LOG("posable: %u batches", g_batches);
    LOG("sotr: %u compiled for the TEV, %u kept their first map",
        g_sotr_built,
        g_sotr_failed);
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

    /* Reflective models: base * light + sphere * tint * mask. The mask is
     * the multipurpose map's red on Xbox; PC keeps it in alpha, which the
     * GX formats drop, so PC parts go unmasked */
    using combiner_t = stage_t::combiner_t;
    GX_SetTevSwapModeTable(
        GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    gx_sotr::load_swap_table();
    stage_t const reflect_base{
        .stage    = stage_t::stage0,
        .op       = stage_t::modulate,
        .texcoord = stage_t::texcoord0,
        .texmap   = stage_t::texmap0};
    stage_t const reflect_sphere{
        .stage    = stage_t::stage1,
        .op       = stage_t::custom,
        .texcoord = stage_t::texcoord1,
        .texmap   = stage_t::texmap2,
        .color    = channel_t::color_null,
        .color_combiner =
            combiner_t{
                .a   = GX_CC_ZERO,
                .b   = GX_CC_TEXC,
                .c   = GX_CC_KONST,
                .d   = GX_CC_ZERO,
                .out = GX_TEVREG0},
        .konst_color = GX_TEV_KCSEL_K0};
    auto prog_reflect      = gfx.alloc_program();
    prog_reflect->channels = {lit};
    prog_reflect->stages   = {
        reflect_base,
        reflect_sphere,
        stage_t{
              .stage    = stage_t::stage2,
              .op       = stage_t::custom,
              .texcoord = stage_t::texcoord0,
              .texmap   = stage_t::texmap1,
              .color    = channel_t::color_null,
              .color_combiner =
                combiner_t{
                      .a = GX_CC_ZERO,
                      .b = GX_CC_C0,
                      .c = GX_CC_TEXC,
                      .d = GX_CC_CPREV},
              .tex_swap = GX_TEV_SWAP1}};
    auto prog_reflect_unmasked      = gfx.alloc_program();
    prog_reflect_unmasked->channels = {lit};
    prog_reflect_unmasked->stages   = {
        reflect_base,
        reflect_sphere,
        stage_t{
              .stage          = stage_t::stage2,
              .op             = stage_t::custom,
              .color          = channel_t::color_null,
              .color_combiner = combiner_t{
                  .a = GX_CC_ZERO,
                  .b = GX_CC_C0,
                  .c = GX_CC_ONE,
                  .d = GX_CC_CPREV}}};

    /* Water: two ripple layers flowing apart, averaged, tinted, with the
     * sky's colour on top; alpha from the material colour */
    combiner_t const keep_alpha{
        GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV};
    auto prog_water      = gfx.alloc_program();
    prog_water->channels = {channel_t{.channel = channel_t::color0a0}};
    prog_water->stages   = {
        stage_t{
              .stage          = stage_t::stage0,
              .op             = stage_t::custom,
              .texcoord       = stage_t::texcoord0,
              .texmap         = stage_t::texmap0,
              .color_combiner = {GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC},
              .alpha_combiner = {GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA}},
        stage_t{
              .stage          = stage_t::stage1,
              .op             = stage_t::custom,
              .texcoord       = stage_t::texcoord1,
              .texmap         = stage_t::texmap0,
              .color          = channel_t::color_null,
              .color_combiner = {GX_CC_CPREV, GX_CC_TEXC, GX_CC_HALF, GX_CC_ZERO},
              .alpha_combiner = keep_alpha},
        stage_t{
              .stage = stage_t::stage2,
              .op    = stage_t::custom,
              .color = channel_t::color_null,
              .color_combiner =
                  {.a     = GX_CC_ZERO,
                   .b     = GX_CC_CPREV,
                   .c     = GX_CC_KONST,
                   .d     = GX_CC_ZERO,
                   .scale = GX_CS_SCALE_2},
              .alpha_combiner = keep_alpha,
              .konst_color    = GX_TEV_KCSEL_K0},
        stage_t{
              .stage = stage_t::stage3,
              .op    = stage_t::custom,
              .color = channel_t::color_null,
              .color_combiner =
                  {GX_CC_ZERO, GX_CC_KONST, GX_CC_HALF, GX_CC_CPREV},
              .alpha_combiner = keep_alpha,
              .konst_color    = GX_TEV_KCSEL_K1},
    };

    /* With its reflection: the sky above added on top instead, its lookup
     * thrown about by the first ripple layer */
    auto prog_water_shiny       = gfx.alloc_program();
    prog_water_shiny->channels  = prog_water->channels;
    prog_water_shiny->stages    = prog_water->stages;
    prog_water_shiny->stages[3] = stage_t{
        .stage          = stage_t::stage3,
        .op             = stage_t::custom,
        .texcoord       = stage_t::texcoord2,
        .texmap         = stage_t::texmap1,
        .color          = channel_t::color_null,
        .color_combiner = {GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV},
        .alpha_combiner = keep_alpha,
        .warp_by        = 0};
    prog_water_shiny->indirect = {
        {.texcoord = stage_t::texcoord0, .texmap = stage_t::texmap0}};
    u64 const water_epoch = gettime();

    using bind_t = gexxo::texture_binding_t;

    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

    g_sprites.begin(gfx);
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
        g_secs += std::clamp(dt, 0.f, 0.2f);
        last_t          = now_t;
        if(dt <= 0.f || dt > 0.2f)
            dt = 1.f / 50.f; // clamp first/stalled frames
        update_camera(dt);
#if defined(COFFEE_GEKKO)
        poll_network(static_cast<u32>(i));
#endif

        sky_t const* sky = active_sky();
        rt->clear(
            sky ? Vecf4(sky->fog_color, 1.f) : Vecf4{0.06f, 0.06f, 0.09f, 1.f},
            1.f,
            0);
        Matf4 const     mv     = make_modelview(0.f);
        Matf4 const     mvs[1] = {mv};
        frustum_t const fr     = make_frustum(g_proj * mv);
        u32             drawn = 0, culled = 0;
#if defined(COFFEE_GEKKO)
        update_poses(dt, fr);
#endif

        /* A model's parts at a modelview; lit models take the sun, chicago
         * parts their own blend */
        auto draw_model = [&](model_t&                  model,
                              Matf4 const&              smv,
                              bool                      lit,
                              std::vector<Matf4> const* skin = nullptr,
                              lens_t const*             lens = nullptr) {
            Matf4 const smvs[1] = {smv};
            for(auto& part : model.parts)
            {
                if(!part.vao || !part.vao->m_element_buffer || part.hidden)
                    continue;
                if(!lit && !part.diffuse)
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
                    .program             = !lit           ? prog_diff
                                           : part.diffuse ? prog_obj
                                                          : prog_obj_flat,
                    .vertices            = part.vao,
                    .instance_transforms = gsl::span<Matf4 const>(smvs, 1),
                    .material_color      = Vecf4{1.f},
                };
                if(lens)
                    cmd.perspective = *lens;
                if(part.diffuse)
                    cmd.textures = {bind_t{.texture = part.diffuse}};
                if(part.chicago_program)
                {
                    cmd.program            = part.chicago_program;
                    cmd.textures           = part.chicago_maps;
                    cmd.texture_transforms = part.chicago_uv;
                }
                Matf4 sotr_uvs[4];
                if(part.sotr_program)
                {
                    auto const& info   = *part.sotr;
                    cmd.program        = part.sotr_program;
                    cmd.textures       = part.sotr_maps;
                    cmd.material_color = gx_sotr::material_color();
                    for(u32 i = 0; i < info.maps.size(); i++)
                        sotr_uvs[i] = sotr_uv(info.maps[i], g_secs);
                    cmd.texture_transforms =
                        gsl::span<Matf4 const>(sotr_uvs, info.maps.size());
                    for(u32 k = 0; k < part.sotr_konst.size(); k++)
                        cmd.konst_colors[k] =
                            sotr_konst(info, part.sotr_konst[k], g_secs);
                }
                Matf4 const reflect_mtx[1] = {gx_reflection::texgen(smv)};
                if(lit && part.diffuse && part.reflection)
                {
                    bool const masked = g_xbox && part.multipurpose;
                    cmd.program = masked ? prog_reflect : prog_reflect_unmasked;
                    cmd.textures.push_back(
                        bind_t{
                            .texture  = part.reflection->sphere,
                            .texmap   = stage_t::texmap2,
                            .texcoord = stage_t::texcoord1,
                            .gen_src  = bind_t::src_normal,
                            .gen_mtx  = GX_TEXMTX0});
                    if(masked)
                        cmd.textures.push_back(
                            bind_t{
                                .texture  = part.multipurpose,
                                .texmap   = stage_t::texmap1,
                                .texcoord = stage_t::texcoord0});
                    cmd.texture_transforms =
                        gsl::span<Matf4 const>(reflect_mtx, 1);
                    cmd.konst_colors[0] = Vecf4(part.reflection_tint, 1.f);
                }
                gexxo::cull_state const cull{.front_face = false};
                auto submit = [&](gexxo::draw_command const& c) {
                    switch(part.blend)
                    {
                    case blend_t::opaque:
                        // Model base map alpha is a mask (change colour,
                        // specular), not coverage
                        gfx.submit(c, cull, gexxo::blend_state{.opaque = true});
                        break;
                    case blend_t::add:
                        gfx.submit(
                            c, cull, gexxo::blend_state{.additive = true});
                        break;
                    case blend_t::multiply:
                        gfx.submit(
                            c, cull, gexxo::blend_state{.multiply = true});
                        break;
                    case blend_t::alpha:
                        gfx.submit(c, cull, gexxo::blend_state{});
                        break;
                    }
                };
                if(!skin || part.batches.empty())
                {
                    submit(cmd);
                    continue;
                }
                // Posed: each batch through its palette, in one submit
                static std::vector<Matf4>                        palettes;
                static std::vector<gexxo::draw_command::batch_t> batches;
                palettes.resize(part.batches.size() * gx_skin::palette_size);
                batches.clear();
                for(u32 i = 0; i < part.batches.size(); i++)
                {
                    auto const& b       = part.batches[i];
                    Matf4*      palette = &palettes[i * gx_skin::palette_size];
                    for(u32 k = 0; k < b.keys.size(); k++)
                        palette[k] = smv * gx_skin::matrix(b.keys[k], *skin);
                    batches.push_back(
                        {b.vao.get(),
                         b.index_count,
                         gsl::span<Matf4 const>(palette, b.keys.size())});
                }
                cmd.call.mode          = gexxo::drawing::primitive::triangle;
                cmd.data.elements.type = semantic::type_t::u16;
                cmd.batches            = batches;
                submit(cmd);
            }
        };

        /* The sky: around the camera, behind everything, out of the fog */
        set_fog(nullptr);
        if(sky && g_models.contains(sky->model))
        {
            auto& model = g_models[sky->model];
            Matf4 view  = mv;
            view[3]     = Vecf4{0.f, 0.f, 0.f, 1.f};
            f32 const extent =
                glm::length(model.center) + std::max(model.radius, 1.f);
            GX_SetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
            draw_model(model, glm::scale(view, Vecf3{50.f / extent}), false);
            GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
        }
        set_fog(sky);
        std::vector<std::pair<page_t*, submesh_t*>> water;

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
                if(sm.water)
                {
                    water.push_back({&page, &sm});
                    continue;
                }
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
        load_sun(mv, sky ? *sky : sky_t{});
        bake_reflections(mv);
        for(auto const& inst : g_objects)
        {
            if(inst.hidden)
                continue;
            if(!sphere_visible(fr, inst.center, inst.radius))
            {
                culled++;
                continue;
            }
            auto it = g_models.find(inst.model_id);
            if(it == g_models.end())
                continue;
            drawn++;
            std::vector<Matf4> const* skin = nullptr;
#if defined(COFFEE_GEKKO)
            if(auto pose = g_posed.find(&inst - g_objects.data());
               pose != g_posed.end() && !pose->second.skin.empty())
                skin = &pose->second.skin;
#endif
            draw_model(it->second, mv * inst.xform, true, skin);
        }
#if defined(COFFEE_GEKKO)
        draw_combat(mv, dt, draw_model);
#endif
        // Water last, over what's beneath it, without writing depth
        f32 const   secs = ticks_to_microsecs(gettime() - water_epoch) / 1e6f;
        Vecf3 const sky_tint =
            sky ? glm::mix(sky->fog_color, Vecf3{1.f}, 0.4f) : Vecf3{0.5f};
        for(auto [page, sm] : water)
        {
            if(!sm->ripple)
                continue;
            Vecf2 const flow  = sm->ripple_flow * secs;
            auto        layer = [&](Vecf2 const& offset, f32 scale) {
                return glm::scale(
                    glm::translate(Matf4{1.f}, Vecf3{offset, 0.f}),
                    Vecf3{scale, scale, 1.f});
            };
            Matf4 const ripples[3] = {
                layer(flow, sm->ripple_scale),
                layer(Vecf2{-flow.y, flow.x} * 0.7f, sm->ripple_scale * 0.83f),
                gx_reflection::hemisphere_texgen(g_cam_pos, kWaterSkyExtent)};
            gexxo::draw_command cmd{
                .call =
                    {.indexed   = true,
                     .instanced = true,
                     .mode      = gexxo::drawing::primitive::triangle},
                .data =
                    {
                        .elements =
                            {.count  = sm->index_count,
                             .offset = sm->index_offset,
                             .type   = semantic::type_t::u16},
                        .arrays =
                            {.count = static_cast<u32>(page->verts.size())},
                        .instances = {.count = 1},
                    },
                .program  = prog_water,
                .vertices = page->vao,
                .textures =
                    {bind_t{
                         .texture  = sm->ripple,
                         .texmap   = stage_t::texmap0,
                         .texcoord = stage_t::texcoord0,
                         .gen_mtx  = GX_TEXMTX0},
                     bind_t{
                         .texture  = sm->ripple,
                         .texmap   = stage_t::texmap0,
                         .texcoord = stage_t::texcoord1,
                         .gen_mtx  = GX_TEXMTX1}},
                .instance_transforms = gsl::span<Matf4 const>(mvs, 1),
                .material_color      = Vecf4{1.f, 1.f, 1.f, 0.7f},
                .konst_colors =
                    {Vecf4(sm->water_tint, 1.f), Vecf4(sky_tint, 1.f)},
                .texture_transforms = gsl::span<Matf4 const>(ripples, 2),
            };
            if(sm->water_sky)
            {
                // Ripple normals' green and blue push the lookup a few texels
                constexpr f32 warp = 0.75f;
                cmd.program        = prog_water_shiny;
                cmd.textures.push_back(
                    bind_t{
                        .texture  = sm->water_sky,
                        .texmap   = stage_t::texmap1,
                        .texcoord = stage_t::texcoord2,
                        .gen_type = bind_t::mtx3x4,
                        .gen_src  = bind_t::src_position,
                        .gen_mtx  = GX_TEXMTX0 + 2 * 3});
                cmd.texture_transforms = gsl::span<Matf4 const>(ripples, 3);
                cmd.indirect_matrix    = gexxo::draw_command::indirect_matrix_t{
                       .rows  = {{{0.f, 0.f, warp}, {0.f, warp, 0.f}}},
                       .scale = -5};
            }
            // Over what's beneath it, without writing depth
            gfx.submit(
                cmd,
                gexxo::cull_state{.front_face = false},
                gexxo::blend_state{},
                gexxo::depth_state{.write = false});
        }
        g_sprites.flush(mv);
#if defined(COFFEE_GEKKO)
        if(g_walking)
            if(auto const* fp = update_first_person(dt))
            {
                // Over everything, in the nearest sliver of depth
                set_fog(nullptr);
                gexxo::detail::depth_range(0.f, 0.05f);
                draw_model(
                    g_models[g_hands],
                    kViewFromFirstPerson,
                    true,
                    &g_fp_pose.hands,
                    &kFirstPersonLens);
                if(auto m = g_models.find(fp->model);
                   m != g_models.end() && !fp->weapon.empty())
                    draw_model(
                        m->second,
                        kViewFromFirstPerson,
                        true,
                        &g_fp_pose.held,
                        &kFirstPersonLens);
                gexxo::detail::depth_range(0.f, 1.f);
            }
#endif
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
