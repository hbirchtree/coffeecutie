/* Trimmed BlamGraphics alternative for the GameCube (gexxo): load a PC Halo map
 * (demand-paged), walk its BSP geometry, and draw it diffuse * lightmap with the
 * GX fixed-function pipeline.
 *
 * Per BSP `lightmap` struct = one page (geometry sharing a lightmap bitmap).
 * Each material in a page = a submesh with its own diffuse texture (from the
 * shader's base map). Vertices carry both the diffuse UV (pc_vertex.texcoord)
 * and the lightmap UV (light_vertex.texcoord). Lightmaps are RGB565, diffuse
 * maps are DXT1 -> both decoded into GX tiled formats. Two TEV stages multiply
 * diffuse by lightmap. Everything is byte-swapped to host (big-endian) order. */

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
#include <blam/volta/blam_mod2.h>
#include <blam/volta/blam_scenario.h>
#include <blam/volta/blam_shaders.h>
#include <blam/volta/blam_stl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#if defined(FEATURE_ENABLE_Gexxo)
#include <coffee/graphics/apis/gexxo/rhi_resources.h>
#include <coffee/gexxo/gexxo_api.h>
#include <coffee/graphics/apis/CGexxo>
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
#endif

using namespace Coffee;
using blam::from_le;
using typing::vector_types::Matf4;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;

using version_t      = blam::pc_version_t;
using pc_vertex_t    = blam::vert::vertex<blam::vert::uncompressed>;
using light_vertex_t = blam::vert::light_vertex<blam::vert::uncompressed>;

static constexpr char const* MAP_NAME = "bloodgulch.map";

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

/* One lightmap page: geometry + its lightmap bitmap + per-material submeshes. */
struct page_t
{
    std::vector<gx_vertex>  verts;
    std::vector<u16>        indices;
    std::vector<submesh_t>  submeshes;
    i16                     lm_idx{-1};

#if defined(FEATURE_ENABLE_Gexxo)
    std::shared_ptr<gexxo::vertex_array_t> vao;
    std::shared_ptr<gexxo::buffer_t>       vbo, ebo;
    std::shared_ptr<gexxo::texture_t>      lightmap;
#endif
};

/* Scenery: placed mod2 model instances. Geometry is per unique model (deduped
 * by mod2 tag id); each instance just carries a world transform. */
struct scn_vertex
{
    f32 pos[3];
    f32 uv[2];
};
struct scn_part
{
    std::vector<scn_vertex>    verts;
    std::vector<u16>           indices; // triangle strip
    blam::bitm::image_t const* diffuse_img{nullptr};
#if defined(FEATURE_ENABLE_Gexxo)
    std::shared_ptr<gexxo::vertex_array_t> vao;
    std::shared_ptr<gexxo::buffer_t>       vbo, ebo;
    std::shared_ptr<gexxo::texture_t>      diffuse;
#endif
};
struct scenery_model
{
    std::vector<scn_part> parts;
    Vecf3                 center{}; // local-space bounding sphere (for culling)
    f32                   radius{0};
};
struct scenery_inst
{
    u32   model_id;
    Matf4 xform;
    Vecf3 center{}; // world-space bounding sphere
    f32   radius{0};
};
static std::map<u32, scenery_model> g_models;  // by mod2 tag id
static std::vector<scenery_inst>    g_scenery; // placed instances

static std::vector<page_t> g_pages;
static Vecf3               g_bb_min{1e9f, 1e9f, 1e9f};
static Vecf3               g_bb_max{-1e9f, -1e9f, -1e9f};
static int                 g_err = -1;

// First player spawn (Halo world space, Z up) for a first-person camera.
static bool  g_have_spawn = false;
static Vecf3 g_spawn_pos{};
static f32   g_spawn_rot = 0.f;
static i16   g_spawn_bsp = 0; // which structure BSP the spawn is in

// Active (spawn) BSP header + its magic.
static blam::bsp::header const* g_active_hdr = nullptr;
static blam::map_ptr            g_active_bm;

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

/* --- tag/shader/bitmap resolution ----------------------------------------- */

static blam::bitm::header_t const* resolve_bitm(
    blam::map_container<version_t> const& c, blam::tagref_t const& ref)
{
    u32 const cnt = from_le(c.tags->tag_count);
    u32 const tid = from_le(ref.tag_id);
    u32 const idx = tid & 0xFFFF; // salt is the high 16 bits; index is the low
    if(tid == 0xFFFFFFFF || idx >= cnt)
        return nullptr;
    auto hr = c.tags->tags(c.map)[idx].data<blam::bitm::header_t>(c.magic);
    return hr.has_error() ? nullptr : hr.value();
}

static blam::bitm::image_t const* first_image(
    blam::map_container<version_t> const& c, blam::bitm::header_t const* bitm)
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
 * (BSP) + shader_model (scenery) are handled; other classes are untextured. */
static shader_info resolve_shader(
    blam::map_container<version_t> const& c, blam::tagref_t const& shader_ref)
{
    shader_info out;
    u32 const   cnt = from_le(c.tags->tag_count);
    u32 const   sid = from_le(shader_ref.tag_id);
    if(sid == 0xFFFFFFFF || (sid & 0xFFFF) >= cnt)
        return out;
    blam::tag_t const& st = c.tags->tags(c.map)[sid & 0xFFFF];
    if(st.matches(blam::tag_class_t::shader_environment))
    {
        auto sh = st.data<blam::shader::shader_env>(c.magic);
        if(sh.has_error())
            return out;
        out.diffuse      = first_image(c, resolve_bitm(c, sh.value()->diffuse.base));
        out.detail       = first_image(c, resolve_bitm(c, sh.value()->diffuse.primary.map));
        out.detail_scale = from_le(sh.value()->diffuse.primary.scale);
    } else if(st.matches(blam::tag_class_t::shader_model))
    {
        auto sh = st.data<blam::shader::shader_model>(c.magic);
        if(sh.has_error())
            return out;
        out.diffuse      = first_image(c, resolve_bitm(c, sh.value()->maps.base));
        out.detail       = first_image(c, resolve_bitm(c, sh.value()->maps.detail.map));
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
    blam::bsp::header const& hdr, blam::map_ptr const& bm)
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
        node = glm::dot(pl.n, pt) >= pl.d ? n.front : n.back;
    }
    return std::nullopt;
}

/* --- scenery (mod2 models) ------------------------------------------------ */

static void load_model_cpu(blam::map_container<version_t> const& c, u32 mod2_id)
{
    using namespace blam;
    u32 const cnt = from_le(c.tags->tag_count);
    if((mod2_id & 0xFFFF) >= cnt)
        return;
    // NB: inside `using namespace blam`, the file-local `version_t` alias
    // collides with the enum blam::version_t, so spell out pc_version_t.
    auto hdr =
        c.tags->tags(c.map)[mod2_id & 0xFFFF].data<mod2::header<pc_version_t>>(
            c.magic);
    if(hdr.has_error())
        return;
    auto md = hdr.value()->model_at(mod2::lod_high_ext, c.magic);
    if(!md)
        return;
    Vecf2 const uvscale{
        from_le(hdr.value()->uvscale.x), from_le(hdr.value()->uvscale.y)};
    auto shaders = hdr.value()->shaders.data(c.magic);
    auto vmag    = c.tags->vertex_magic(c.magic);

    scenery_model model;
    Vecf3         mn{1e9f, 1e9f, 1e9f}, mx{-1e9f, -1e9f, -1e9f};
    for(mod2::part const* p : md->parts)
    {
        auto vseg = p->vertex_segment(*c.tags, c.magic).data(vmag);
        auto iseg = p->index_segment(*c.tags).data(vmag);
        if(vseg.has_error() || iseg.has_error())
            continue;
        scn_part part;
        for(auto const& mv : vseg.value()) // mod2_vertex<uncompressed>
        {
            scn_vertex  v;
            Vecf3 const p3{
                from_le(mv.position.x),
                from_le(mv.position.y),
                from_le(mv.position.z)};
            v.pos[0] = p3.x;
            v.pos[1] = p3.y;
            v.pos[2] = p3.z;
            v.uv[0]  = from_le(mv.texcoord.x) * uvscale.x;
            v.uv[1]  = from_le(mv.texcoord.y) * uvscale.y;
            part.verts.push_back(v);
            mn = glm::min(mn, p3);
            mx = glm::max(mx, p3);
        }
        for(u16 idx : iseg.value())
            part.indices.push_back(from_le(idx));
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

static void build_scenery(
    blam::map_container<version_t> const& c,
    blam::scn::scenario<version_t> const* scn)
{
    using namespace blam;
    u32 const cnt   = from_le(c.tags->tag_count);
    auto      insts = scn->objects.scenery.instances.data(c.magic);
    auto      pal   = scn->objects.scenery.palette.data(c.magic);
    if(insts.has_error() || pal.has_error())
        return;
    u32 skipped = 0;
    for(scn::scenery_spawn const& s : insts.value())
    {
        Vecf3 const pos{from_le(s.pos.x), from_le(s.pos.y), from_le(s.pos.z)};

        // Only load objects that resolve to a cluster of the active BSP
        // (cluster_at uses the RAM collision copy -> no paging). Points in
        // another section / solid space return nullopt.
        if(!cluster_at(pos))
        {
            skipped++;
            continue;
        }

        i16 const ref = from_le(s.ref);
        if(ref < 0 || static_cast<size_t>(ref) >= pal.value().size())
            continue;
        u32 const scen_id = from_le(pal.value()[ref][0].tag_id);
        if(scen_id == 0xFFFFFFFF || (scen_id & 0xFFFF) >= cnt)
            continue;
        auto scen =
            c.tags->tags(c.map)[scen_id & 0xFFFF].data<scn::scenery>(c.magic);
        if(scen.has_error())
            continue;
        u32 const mod2_id = from_le(scen.value()->model.tag_id);
        if(mod2_id == 0xFFFFFFFF || (mod2_id & 0xFFFF) >= cnt)
            continue;

        Vecf3 const rot{from_le(s.rot.x), from_le(s.rot.y), from_le(s.rot.z)};
        Matf4       m = glm::translate(Matf4{1.f}, pos);
        m             = glm::rotate(m, rot.x, Vecf3{0, 0, 1}); // yaw (Z up)
        m             = glm::rotate(m, rot.y, Vecf3{0, 1, 0}); // pitch
        m             = glm::rotate(m, rot.z, Vecf3{1, 0, 0}); // roll

        if(g_models.find(mod2_id) == g_models.end())
            load_model_cpu(c, mod2_id);

        // World-space bounding sphere for frustum culling.
        auto const& mdl = g_models[mod2_id];
        Vecf3 const wc  = Vecf3(m * Vecf4(mdl.center, 1.f));
        g_scenery.push_back({mod2_id, m, wc, mdl.radius});
    }
    LOG("scenery: %u instances, %u models (%u off-section skipped)",
        static_cast<unsigned>(g_scenery.size()),
        static_cast<unsigned>(g_models.size()),
        skipped);
}

/* --- geometry ------------------------------------------------------------- */

static void append_material(
    blam::map_container<version_t> const& c,
    page_t&                               page,
    blam::bsp::material const&            mat,
    blam::bsp::header const&              hdr,
    blam::map_ptr const&                  bm)
{
    auto verts  = mat.vertices().data(bm);
    auto lverts = mat.light_verts().data(bm);
    if(verts.has_error() || lverts.has_error())
        return;
    auto vspan  = verts.value();
    auto lspan  = lverts.value();
    u32  nverts = static_cast<u32>(vspan.size_bytes() / sizeof(pc_vertex_t));
    u32  nlight = static_cast<u32>(lspan.size_bytes() / sizeof(light_vertex_t));
    if(nverts == 0 || nlight < nverts)
        return;

    shader_info const si = resolve_shader(c, mat.shader);

    Vecf3     mn{1e9f, 1e9f, 1e9f}, mx{-1e9f, -1e9f, -1e9f};
    u16 const vert_base = static_cast<u16>(page.verts.size());
    for(u32 i = 0; i < nverts; i++)
    {
        auto const* pv = reinterpret_cast<pc_vertex_t const*>(
            vspan.data() + i * sizeof(pc_vertex_t));
        auto const* lv = reinterpret_cast<light_vertex_t const*>(
            lspan.data() + i * sizeof(light_vertex_t));
        Vecf3 const p{
            from_le(pv->position.x),
            from_le(pv->position.y),
            from_le(pv->position.z)};
        f32 const du = from_le(pv->texcoord.x);
        f32 const dv = from_le(pv->texcoord.y);
        gx_vertex gv;
        gv.pos[0]   = p.x;
        gv.pos[1]   = p.y;
        gv.pos[2]   = p.z;
        gv.duv[0]   = du;
        gv.duv[1]   = dv;
        gv.luv[0]   = from_le(lv->texcoord.x);
        gv.luv[1]   = from_le(lv->texcoord.y);
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

static int extract_bsp()
{
    LOG("loading map bytes...");
    auto bytes = load_map_bytes();
    LOG("map bytes = %u", static_cast<unsigned>(bytes.size));
    if(!bytes.size)
        return 1;

    auto map = blam::map_container<version_t>::from_bytes(
        bytes, version_t{}, [](std::string_view, libc_types::i16) {});
    if(map.has_error())
    {
        g_err = static_cast<int>(map.error());
        LOG("map parse failed, error=%d", g_err);
        return 2;
    }
    auto const& container = map.value();
    g_internal_magic      = container.magic.ptr_only();

    auto scn = container.tags->scenario(container.map, container.magic);
    if(!scn)
        return 3;

    // Grab the first player spawn for the camera.
    if(auto locs = (*scn)->player_start.locations.data(container.magic);
       !locs.has_error() && !locs.value().empty())
    {
        auto const& l = locs.value()[0];
        g_spawn_pos  = {from_le(l.pos.x), from_le(l.pos.y), from_le(l.pos.z)};
        g_spawn_rot  = from_le(l.rot);
        g_spawn_bsp  = from_le(l.bsp_index);
        g_have_spawn = true;
        LOG("spawn at %d,%d,%d rot=%d",
            (int)g_spawn_pos.x,
            (int)g_spawn_pos.y,
            (int)g_spawn_pos.z,
            (int)(g_spawn_rot * 57.3f));
    }

    auto bsps = (*scn)->bsp_info.data(container.magic);
    if(bsps.has_error())
        return 4;
    LOG("bsp_info count=%u spawn_bsp=%d",
        static_cast<unsigned>(bsps.value().size()),
        (int)g_spawn_bsp);

    // Only the spawn's structure BSP -- campaign maps have several BSPs that
    // overlap in world space (Halo loads one at a time), and rendering them all
    // would also blow the 24MB budget.
    u32 bsp_idx = 0;
    for(auto const& bi : bsps.value())
    {
        if(bsp_idx++ != static_cast<u32>(g_spawn_bsp))
            continue;
        auto const bm    = bi.bsp_magic(container.magic);
        auto       hdr_r = bi.to_bsp(bm).to_header().data(bm, blam::single_value);
        if(hdr_r.has_error())
            continue;
        auto const* hdr = hdr_r.value();
        g_active_hdr    = hdr;
        g_active_bm     = bm;
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

    build_scenery(container, *scn);

    return g_pages.empty() ? 5 : 0;
}

#if defined(FEATURE_ENABLE_Gexxo)
/* --- texture decode (kept-alive tiled buffers + diffuse dedup) ------------- */

u32                       g_lm_bytes  = 0;
u32                       g_dxt_bytes = 0;
static std::vector<void*> g_tiled_bufs;

// Cap total texture memory so a big map (e.g. b30) degrades to fewer textures
// instead of OOMing; the rest fall back to lightmap/untextured.
static u32 constexpr kTexBudget = 9u * 1024 * 1024;
static u32           g_tex_used = 0;

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
 * computed on the big-endian CPU, e.g. DXT1-decompressed pixels). */
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
    u32       o  = 0;
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
                            b, src + (by * bw + bx) * block_size + color_off, 8);
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
    u32 const  size   = from_le(img.size);
    bool const shared = (static_cast<u16>(from_le(img.flags)) &
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
    auto pix = blam::reference<libc_types::u8>{
        .count = img.size, .offset = img.offset}
                   .data(internal_magic);
    if(pix.has_error())
        return false;
    std::copy(pix.value().begin(), pix.value().end(), out.begin());
    return true;
}

/* Decode a bitmap image (RGB565 or DXT1) into a GX texture. wrap = GX_CLAMP for
 * [0,1] lightmaps, GX_REPEAT for tiling diffuse maps. */
static std::shared_ptr<gexxo::texture_t> load_texture(
    blam::bitm::image_t const& img, blam::map_ptr const& magic, u8 wrap = GX_CLAMP)
{
    auto const fmt = from_le(img.format);
    i16 const  w   = from_le(img.isize.x);
    i16 const  h   = from_le(img.isize.y);
    if(w <= 0 || h <= 0)
        return nullptr;

    // Offline-transcoded (map-transcode tool): pixel data is already in a GX
    // tiled layout -> upload directly, no decode/re-tile.
    if(gexxo::native::is_native(fmt))
    {
        u8 gxfmt;
        switch(gexxo::native::code_of(fmt))
        {
        case gexxo::native::format::cmpr:   gxfmt = GX_TF_CMPR;   break;
        case gexxo::native::format::rgb565: gxfmt = GX_TF_RGB565; break;
        case gexxo::native::format::i8:     gxfmt = GX_TF_I8;     break;
        case gexxo::native::format::ia8:    gxfmt = GX_TF_IA8;    break;
        default: return nullptr;
        }
        u32 const total = from_le(img.size);
        if(!total)
            return nullptr;
        // img.mipmaps = GX maxlod; pixel data holds levels 0..maxlod packed
        // consecutively. Skip the top g_mip_skip levels (keep >= 1) so GX's base
        // is a lower-res mip: advance past their tiled bytes + halve the dims.
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
        tex->init_raw(
            tiled, bw, bh, gxfmt, wrap, wrap,
            static_cast<u8>(maxlod - skip));
        return tex;
    }

    bool const is_bcn =
        fmt == blam::bitm::format_t::BC1 || fmt == blam::bitm::format_t::BC2 ||
        fmt == blam::bitm::format_t::BC3;
    if(fmt != blam::bitm::format_t::R5G6B5 && !is_bcn)
        return nullptr; // uncompressed/palettised not handled yet

    std::vector<libc_types::u8> pixels;
    if(!read_bitmap_pixels(img, magic, pixels))
        return nullptr;

    // Lightmaps (RGB565 source) tile 4x4 -> GX_TF_RGB565 (2 B/px); diffuse/detail
    // (DXT source) re-tile -> GX_TF_CMPR (0.5 B/px, 4x smaller than expanding).
    bool const is_565 = fmt == blam::bitm::format_t::R5G6B5;
    u16 const  tw     = is_565 ? ((w + 3) & ~3) : ((w + 7) & ~7);
    u16 const  th     = is_565 ? ((h + 3) & ~3) : ((h + 7) & ~7);
    u32 const  tbytes = is_565 ? (static_cast<u32>(tw) * th * 2)
                               : (static_cast<u32>(tw) * th / 2);
    u8 const   gxfmt  = is_565 ? GX_TF_RGB565 : GX_TF_CMPR;

    if(g_tex_used + tbytes > kTexBudget)
        return nullptr;
    void* tiled = memalign(32, (tbytes + 31) & ~31u);
    if(!tiled)
        return nullptr;
    g_tex_used += tbytes;
    extern u32 g_lm_bytes, g_dxt_bytes;
    (is_565 ? g_lm_bytes : g_dxt_bytes) += tbytes;

    if(is_565)
    {
        if(pixels.size() < static_cast<size_t>(w) * h * 2)
        {
            free(tiled);
            return nullptr;
        }
        // Map RGB565 is little-endian -> swap to big-endian for GX.
        tile_rgb565(
            reinterpret_cast<u16 const*>(pixels.data()),
            reinterpret_cast<u16*>(tiled),
            w,
            h,
            true);
    } else // BC1/BC2/BC3 -> GX_TF_CMPR (re-tile the DXT1 colour block)
    {
        bool const bc1 = fmt == blam::bitm::format_t::BC1;
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
    auto tex            = load_texture(*img, g_internal_magic, GX_REPEAT);
    g_diffuse_cache[img] = tex;
    return tex;
}

// Fly-cam state (Halo world space, Z up), driven by the GameCube pad.
static bool  g_cam_ready = false;
static Vecf3 g_cam_pos{};
static f32   g_cam_yaw   = 0.f; // around +Z
static f32   g_cam_pitch = 0.f; // up/down

/* View-frustum culling: 6 planes from the view-projection matrix (normalized),
 * sphere-vs-frustum test. Matches the gexxo-fixed 90deg/4:3/0.1-100 projection. */
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
        std::cos(g_cam_yaw) * cp, std::sin(g_cam_yaw) * cp, std::sin(g_cam_pitch)};
}

/* Update the fly-cam from the controller: left stick = move/strafe, C-stick =
 * look, A/B = up/down, R = speed boost. dt in seconds. No-op without a pad
 * (headless), so the camera just sits at the spawn. */
static void update_camera(f32 dt)
{
    if(!g_cam_ready)
    {
        g_cam_pos   = g_spawn_pos + Vecf3{0.f, 0.f, 0.65f};
        g_cam_yaw   = g_spawn_rot;
        g_cam_pitch = 0.f;
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

    Vecf3 const fwd = cam_forward();
    Vecf3 const right =
        glm::normalize(glm::cross(fwd, Vecf3{0.f, 0.f, 1.f}));
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
#endif

i32 coffee_main(i32, cstring_w*)
{
#if defined(FEATURE_ENABLE_Gexxo)
    gexxo::api gfx;
    gfx.load({});
    auto rt = gfx.default_rendertarget();
    auto free_mem = [] {
        return static_cast<u32>(reinterpret_cast<uintptr_t>(SYS_GetArena1Hi()) -
                                reinterpret_cast<uintptr_t>(SYS_GetArena1Lo()));
    };
    LOG("mem free at start: %u KiB", free_mem() / 1024);
#endif

    LOG("start");
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

#if defined(FEATURE_ENABLE_Gexxo)
    static Vecf4 const fail_colors[] = {
        {0.f, 0.7f, 0.f, 1.f}, {0.7f, 0.f, 0.7f, 1.f}, {0.7f, 0.f, 0.f, 1.f},
        {0.9f, 0.5f, 0.f, 1.f}, {0.8f, 0.8f, 0.f, 1.f}, {0.f, 0.f, 0.8f, 1.f},
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
    u32 scn_parts     = 0;
    try
    {
    for(u32 pi = 0; pi < g_pages.size(); pi++)
    {
        auto& page = g_pages[pi];
        if(page.verts.empty() || page.indices.empty())
            continue;
        page.vao = gfx.alloc_vertex_array();
        page.vbo = gfx.alloc_buffer(gexxo::buffers::vertex, RSCA::ReadOnly);
        page.ebo = gfx.alloc_buffer(gexxo::buffers::element, RSCA::ReadOnly);
        page.vao->alloc();
        page.vbo->alloc();
        page.ebo->alloc();
        page.vbo->commit(
            gsl::span<gx_vertex const>(page.verts.data(), page.verts.size()));
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
            .value = {.offset = offsetof(gx_vertex, duv),
                      .stride = sizeof(gx_vertex),
                      .count  = 2},
        });
        page.vao->add({
            .index = 2,
            .role  = static_cast<gexxo::vertex_attribute::role_t>(
                gexxo::vertex_attribute::texcoord0 + 1), // lightmap UV
            .value = {.offset = offsetof(gx_vertex, luv),
                      .stride = sizeof(gx_vertex),
                      .count  = 2},
        });
        page.vao->add({
            .index = 3,
            .role  = static_cast<gexxo::vertex_attribute::role_t>(
                gexxo::vertex_attribute::texcoord0 + 2), // detail UV
            .value = {.offset = offsetof(gx_vertex, detuv),
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

    // Scenery model parts -> GPU (position + diffuse UV; strip indices).
    for(auto& [id, model] : g_models)
        for(auto& part : model.parts)
        {
            if(part.verts.empty() || part.indices.size() < 3)
                continue;
            part.vao = gfx.alloc_vertex_array();
            part.vbo = gfx.alloc_buffer(gexxo::buffers::vertex, RSCA::ReadOnly);
            part.ebo = gfx.alloc_buffer(gexxo::buffers::element, RSCA::ReadOnly);
            part.vao->alloc();
            part.vbo->alloc();
            part.ebo->alloc();
            part.vbo->commit(gsl::span<scn_vertex const>(
                part.verts.data(), part.verts.size()));
            part.ebo->commit(
                gsl::span<u16 const>(part.indices.data(), part.indices.size()));
            part.vao->add({
                .index = 0,
                .role  = gexxo::vertex_attribute::position,
                .value = {.offset = 0, .stride = sizeof(scn_vertex), .count = 3},
            });
            part.vao->add({
                .index = 1,
                .role  = gexxo::vertex_attribute::texcoord0,
                .value = {.offset = offsetof(scn_vertex, uv),
                          .stride = sizeof(scn_vertex),
                          .count  = 2},
            });
            part.vao->set_buffer(gexxo::buffers::vertex, part.vbo, 0);
            part.vao->set_buffer(gexxo::buffers::element, part.ebo);
            part.diffuse = load_diffuse(part.diffuse_img);
            scn_parts++;
        }
    } catch(std::bad_alloc const&)
    {
        // Out of MEM1: render whatever resources were built (the rest stay
        // null and are simply skipped). Big maps don't fully fit in 24MB.
        LOG("gpu: out of memory, rendering partial scene");
    }
    LOG("gpu: %u scenery instances, %u model parts",
        static_cast<unsigned>(g_scenery.size()),
        scn_parts);
    {
        extern u32 g_lm_bytes, g_dxt_bytes;
        LOG("tex mem: lightmap(RGB565)=%u KiB, diffuse/detail(CMPR)=%u KiB",
            g_lm_bytes / 1024,
            g_dxt_bytes / 1024);
    }
    LOG("mem free after load: %u KiB", free_mem() / 1024);

#if defined(COFFEE_GEKKO)
    platform::file::gekko::vmem::unmap(g_map_window);
    LOG("mem free after unmap: %u KiB", free_mem() / 1024);
#endif

    using stage_t   = gexxo::program_t::stage_t;
    using channel_t = gexxo::program_t::channel_t;

    // diffuse * lightmap: stage0 = diffuse (tex0@tc0), stage1 = *lightmap (tex1@tc1)
    auto prog_diff_lm      = gfx.alloc_program();
    prog_diff_lm->channels = {channel_t{.channel = channel_t::color0a0}};
    prog_diff_lm->stages   = {
        stage_t{.stage    = stage_t::stage0,
                  .op       = stage_t::replace,
                  .texcoord = stage_t::texcoord0,
                  .texmap   = stage_t::texmap0},
        stage_t{.stage    = stage_t::stage1,
                  .op       = stage_t::modulate_prev,
                  .texcoord = stage_t::texcoord1,
                  .texmap   = stage_t::texmap1},
    };
    // diffuse * detail(2x) * lightmap (senv with a detail map)
    auto prog_diff_det_lm      = gfx.alloc_program();
    prog_diff_det_lm->channels = {channel_t{.channel = channel_t::color0a0}};
    prog_diff_det_lm->stages   = {
        stage_t{.stage    = stage_t::stage0,
                  .op       = stage_t::replace,
                  .texcoord = stage_t::texcoord0,
                  .texmap   = stage_t::texmap0},
        stage_t{.stage    = stage_t::stage1,
                  .op       = stage_t::modulate_prev_2x,
                  .texcoord = stage_t::texcoord2,
                  .texmap   = stage_t::texmap2},
        stage_t{.stage    = stage_t::stage2,
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
    // flat material colour (untextured scenery parts) -> per-instance palette.
    auto prog_flat      = gfx.alloc_program();
    prog_flat->channels = {channel_t{.channel = channel_t::color0a0}};
    prog_flat->stages   = {stage_t{
        .stage = stage_t::stage0, .op = stage_t::pass}};
    // lightmap only (material has no diffuse): with one tex-gen only texcoord0
    // is generated, so output the lightmap UV (vertex TEX1) into texcoord0.
    auto prog_lm      = gfx.alloc_program();
    prog_lm->channels = {channel_t{.channel = channel_t::color0a0}};
    prog_lm->stages   = {stage_t{
        .stage    = stage_t::stage0,
        .op       = stage_t::replace,
        .texcoord = stage_t::texcoord0,
        .texmap   = stage_t::texmap0}};

    using bind_t = gexxo::texture_binding_t;

    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

#if defined(COFFEE_GEKKO)
    PAD_Init();
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
                    .call = {.indexed   = true,
                             .instanced = true,
                             .mode      = gexxo::drawing::primitive::triangle},
                    .data =
                        {
                            .elements = {.count  = sm.index_count,
                                         .offset = sm.index_offset,
                                         .type   = semantic::type_t::u16},
                            .arrays = {.count =
                                           static_cast<u32>(page.verts.size())},
                            .instances = {.count = 1},
                        },
                    .vertices            = page.vao,
                    .instance_transforms = gsl::span<Matf4 const>(mvs, 1),
                };
                if(sm.diffuse && sm.detail && page.lightmap)
                {
                    cmd.program  = prog_diff_det_lm;
                    cmd.textures = {
                        bind_t{.texture  = sm.diffuse,
                               .texmap   = stage_t::texmap0,
                               .texcoord = stage_t::texcoord0,
                               .gen_src  = bind_t::src_tex0},
                        bind_t{.texture  = sm.detail,
                               .texmap   = stage_t::texmap2,
                               .texcoord = stage_t::texcoord2,
                               .gen_src  = bind_t::src_tex2},
                        bind_t{.texture  = page.lightmap,
                               .texmap   = stage_t::texmap1,
                               .texcoord = stage_t::texcoord1,
                               .gen_src  = bind_t::src_tex1},
                    };
                } else if(sm.diffuse && page.lightmap)
                {
                    cmd.program  = prog_diff_lm;
                    cmd.textures = {
                        bind_t{.texture  = sm.diffuse,
                               .texmap   = stage_t::texmap0,
                               .texcoord = stage_t::texcoord0,
                               .gen_src  = bind_t::src_tex0},
                        bind_t{.texture  = page.lightmap,
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
                    cmd.textures = {
                        bind_t{.texture  = page.lightmap,
                               .texmap   = stage_t::texmap0,
                               .texcoord = stage_t::texcoord0,
                               .gen_src  = bind_t::src_tex1}};
                } else
                    continue;
                gfx.submit(cmd, gexxo::cull_state{.front_face = false});
            }
        }

        // Scenery: each instance's model parts at camera * model transform.
        for(auto const& inst : g_scenery)
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
                    .call = {.indexed   = true,
                             .instanced = true,
                             .mode = gexxo::drawing::primitive::triangle_strip},
                    .data =
                        {
                            .elements = {.count = static_cast<u32>(
                                             part.indices.size()),
                                         .type = semantic::type_t::u16},
                            .arrays = {.count =
                                           static_cast<u32>(part.verts.size())},
                            .instances = {.count = 1},
                        },
                    .program             = part.diffuse ? prog_diff : prog_flat,
                    .vertices            = part.vao,
                    .instance_transforms = gsl::span<Matf4 const>(smvs, 1),
                };
                if(part.diffuse)
                    cmd.textures = {bind_t{.texture = part.diffuse}};
                gfx.submit(cmd, gexxo::cull_state{.front_face = false});
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
