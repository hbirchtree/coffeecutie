#include "h2_map_renderer.h"

#include "components.h"
#include "data.h"
#include "physics.h"
#include "resource_creation.h"
#include "sounds.h"

#include <blam/dimeter/h2_bitm.h>
#include <blam/dimeter/h2_geometry.h>
#include <blam/dimeter/h2_lightmap.h>
#include <blam/dimeter/h2_map.h>
#include <blam/dimeter/h2_material.h>
#include <blam/dimeter/h2_scenario.h>
#include <blam/dimeter/h2_shader.h>
#include <blam/volta/blam_swizzle.h>

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>
#include <coffee/components/subsystem.h>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_buffer.h>
#include <coffee/graphics/apis/gleam/rhi_compat.h>
#include <coffee/graphics/apis/gleam/rhi_draw_command.h>
#include <coffee/graphics/apis/gleam/rhi_program.h>
#include <coffee/graphics/apis/gleam/rhi_submit.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <coffee/graphics/apis/gleam/rhi_texture.h>
#include <coffee/graphics/apis/gleam/rhi_vertex.h>
#include <peripherals/typing/enum/pixels/format.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#if defined(FEATURE_ENABLE_BULLET3)
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#endif

#include <array>
#include <bit>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <set>
#include <unordered_map>

void update_camera_aspect(compo::EntityContainer& e);

namespace h2 {

namespace dim = blam::dimeter;
namespace geo = blam::dimeter::geo;
using blam::from_le;
using Coffee::Resource;
using libc_types::i64;
using libc_types::i8;
using semantic::BytesConst;
using semantic::Span;

struct Vertex
{
    Vecf3 position;
    Vecf2 texcoord;
    Vecf3 normal;
    Vecf3 lightmap{0.f, 0.f, -1.f}; /*!< Atlas u, v, layer; -1 = none */
};

static_assert(sizeof(Vertex) == 44);

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
    count,
};

static constexpr std::string_view variant_files[] = {
    "h2_base",
    "h2_detail",
    "h2_blend",
    "h2_base_illum",
    "h2_detail_illum",
    "h2_blend_illum",
    "h2_sky",
    "h2_sky_mask",
    "h2_water",
};

static_assert(
    std::size(variant_files) == static_cast<size_t>(Variant::count));

/* One shad tag: its diffuse map and every draw range using it */
struct Material
{
    Variant                                variant{Variant::base};
    /* Draw ranges with the structure BSP they belong to; -1 = always */
    struct draw_t
    {
        gfx::draw_command::data_t cmd;
        i32                       section;
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
    bool                                   alpha_test{false};
    bool                                   sky{false}; /*!< follows the camera, unlit */
    bool                                   water{false}; /*!< bump in `texture`, drawn last */
    Vecf2                                  mask_scale{1.f, 1.f};
    std::vector<gfx::draw_command::data_t> draws;
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
    std::string                    name;
    Vecf3                          lo, hi; /*!< World bounds, stand-ins included */
    std::shared_ptr<CollisionMesh> collision;
};

/* Camera inside this volume while in `source` switches to `destination` */
struct SwitchTrigger
{
    Vecf3 lo, hi;
    i32   source, destination;
};

struct SectionStreams
{
    geo::resource_t const* index{nullptr};
    geo::resource_t const* position{nullptr};
    geo::resource_t const* texcoord{nullptr};
    geo::resource_t const* normal{nullptr};
    geo::resource_t const* lightmap{nullptr}; /*!< i16 x2 secondary uv */
    geo::resource_t const* submeshes{nullptr};
};

static SectionStreams find_streams(Span<geo::resource_t const> resources)
{
    SectionStreams out;
    for(auto const& r : resources)
    {
        if(r.type0() == 0 && r.type1() == 72 && !out.submeshes)
            out.submeshes = &r;
        else if(r.type0() == 32 && !out.index)
            out.index = &r;
        else if(r.type0() == 56 && r.type1() == 0 && !out.position)
            out.position = &r;
        else if(r.type0() == 56 && r.type1() == 1 && !out.texcoord)
            out.texcoord = &r;
        else if(r.type0() == 56 && r.type1() == 2 && !out.normal)
            out.normal = &r;
        else if(r.type0() == 56 && r.type1() == 3 && !out.lightmap)
            out.lightmap = &r;
    }
    return out;
}

/* Signed-normalized bitfield */
static f32 snorm_bits(u32 bits, u32 width)
{
    i32 value = static_cast<i32>(bits);
    if(value & (1 << (width - 1)))
        value -= 1 << width;
    return static_cast<f32>(value) / static_cast<f32>((1 << (width - 1)) - 1);
}

/* Packed normal, both field orders the Xbox formats use; which one a map
 * uses is decided against the face normals at load time */
static Vecf3 unpack_11_11_10(u32 v)
{
    return {
        snorm_bits(v & 0x7FF, 11),
        snorm_bits((v >> 11) & 0x7FF, 11),
        snorm_bits((v >> 22) & 0x3FF, 10)};
}

static Vecf3 unpack_10_11_11(u32 v)
{
    return {
        snorm_bits(v & 0x3FF, 10),
        snorm_bits((v >> 10) & 0x7FF, 11),
        snorm_bits((v >> 21) & 0x7FF, 11)};
}

static std::optional<typing::pixels::PixDesc> to_pixdesc(
    blam::bitm::format_t fmt)
{
    using blam::bitm::format_t;
    using typing::pixels::bit_fmt;
    using typing::pixels::comp_flags;
    using typing::pixels::CompFmt;
    using typing::pix_components;
    using typing::pixels::pix_fmt;
    using typing::pixels::PixDesc;
    switch(fmt)
    {
    case format_t::A8:
    case format_t::Y8:
    case format_t::AY8:
        return PixDesc(pix_fmt::R8, bit_fmt::u8, pix_components::R);
    case format_t::A8Y8:
    case format_t::V8U8: /* signed; rebased to unsigned at upload */
        return PixDesc(pix_fmt::RG8, bit_fmt::u8, pix_components::RG);
    case format_t::R5G6B5:
        return PixDesc(pix_fmt::RGB565, bit_fmt::u16_565, pix_components::RGB);
    case format_t::A1RGB5:
        return PixDesc(
            pix_fmt::RGB5A1, bit_fmt::u16_1555, pix_components::RGBA);
    case format_t::ARGB4:
        return PixDesc(pix_fmt::RGBA4, bit_fmt::u16_4444, pix_components::RGBA);
    case format_t::ARGB8:
    case format_t::XRGB8:
        return PixDesc(pix_fmt::RGBA8, bit_fmt::u8, pix_components::RGBA);
    case format_t::BC1:
        return CompFmt(pix_fmt::BCn, comp_flags::BC1);
    case format_t::BC2:
        return CompFmt(pix_fmt::BCn, comp_flags::BC2);
    case format_t::BC3:
        return CompFmt(pix_fmt::BCn, comp_flags::BC3);
    default:
        return std::nullopt;
    }
}

/* Bytes of one mip level; 0 = unhandled format */
static u32 level_bytes(blam::bitm::format_t fmt, u32 w, u32 h)
{
    using blam::bitm::format_t;
    switch(fmt)
    {
    case format_t::BC1:
        return std::max(w / 4, 1u) * std::max(h / 4, 1u) * 8;
    case format_t::BC2:
    case format_t::BC3:
        return std::max(w / 4, 1u) * std::max(h / 4, 1u) * 16;
    case format_t::A8:
    case format_t::Y8:
    case format_t::AY8:
        return w * h;
    case format_t::A8Y8:
    case format_t::V8U8:
    case format_t::R5G6B5:
    case format_t::A1RGB5:
    case format_t::ARGB4:
        return w * h * 2;
    case format_t::ARGB8:
    case format_t::XRGB8:
        return w * h * 4;
    default:
        return 0;
    }
}

static bool is_compressed(blam::bitm::format_t fmt)
{
    using blam::bitm::format_t;
    return fmt == format_t::BC1 || fmt == format_t::BC2 || fmt == format_t::BC3;
}

using H2RendererManifest = compo::SubsystemManifest<
    compo::type_list_t<const PlayerCamera, const PlayerInfo>,
    compo::type_list_t<const BlamResources>,
    compo::empty_list_t>;

struct H2MapRenderer
    : compo::RestrictedSubsystem<H2MapRenderer, H2RendererManifest>
{
    using Proxy = compo::proxy_of<H2RendererManifest>;
    using type  = H2MapRenderer;

    H2MapRenderer(compo::EntityContainer& e)
        : m_container(e)
    {
        /* After MeshRenderer (3072), same slot as the RS2 loader */
        priority = 3080;
    }

    bool main_thread_only() const override
    {
        return true;
    }

    /* True if the file is a Halo 2 cache; it is then loaded (or at least
     * attempted) here and the Halo 1 loader must not see it */
    bool on_map_load(MapLoadEvent const& load)
    {
        if(!load.file)
            return false;
        auto file  = std::make_unique<Resource>(*load.file);
        auto bytes = C_OCAST<BytesConst>(*file);
        /* Xbox and Vista headers share the id/version/footer positions */
        if(!bytes.data || bytes.size < sizeof(dim::file_header_xbox_t) ||
           !reinterpret_cast<dim::file_header_xbox_t const*>(bytes.data)
                ->valid())
        {
            unload();
            return false;
        }

        unload();
        /* Same teardown as open_map(): the Halo 1 world goes away */
        m_container.remove_entity_if([](compo::Entity const& en) {
            return stl_types::any_flag_of(en.tags, ObjectGC);
        });
        if(auto* sound_bus = m_container.service<comp_app::EventBus<SoundEvent>>())
        {
            SoundEvent ev{.type = SoundEvent::clear_all};
            sound_bus->inject(ev, nullptr);
        }
        {
            Physics::Event reset_ev{Physics::Event::Reset};
            Physics::Reset reset{};
            m_container.subsystem_cast<PhysicsBus>().process(reset_ev, &reset);
        }

        /* Bitmap pixels may live in the global cache files next to the map */
        dim::raw_pool pool;
        pool.files[static_cast<u8>(dim::raw_file_t::local)] =
            Span<const byte_t>(bytes.data, bytes.size);
        std::vector<std::unique_ptr<Resource>>            side_files;
        constexpr std::pair<dim::raw_file_t, const char*> side_names[3] = {
            {dim::raw_file_t::mainmenu, "mainmenu.map"},
            {dim::raw_file_t::shared, "shared.map"},
            {dim::raw_file_t::single_player_shared, "single_player_shared.map"},
        };
        auto map_dir = load.file->path().dirname();
        for(auto const& [slot, name] : side_names)
        {
            auto res = std::make_unique<Resource>(
                (map_dir / name).url(load.file->flags));
            auto side = C_OCAST<BytesConst>(*res);
            if(side.data && side.size)
            {
                pool.files[static_cast<u8>(slot)] =
                    Span<const byte_t>(side.data, side.size);
                side_files.push_back(std::move(res));
            }
        }

        ensure_gpu();

        bool loaded = false;
        if(auto map = dim::map_container<dim::xbox_version_t>::from_bytes(
               bytes, dim::xbox_version);
           map.has_value())
            loaded = load_map(map.value(), pool);
        else if(auto map = dim::map_container<dim::vista_version_t>::from_bytes(
                    bytes, dim::vista_version);
                map.has_value())
            loaded = load_map(map.value(), pool);
        else
            cWarning(
                "H2: not a supported Halo 2 cache: error {}",
                static_cast<int>(map.error()));
        if(!loaded)
            cWarning("H2: nothing loaded from {}", load.file->internUrl);
        /* The GPU owns copies of everything; the mappings can go */
        return true;
    }

    void unload()
    {
        if(m_loaded && m_active >= 0)
        {
            Physics::Event       event{.type = Physics::Event::BodyRemoval};
            Physics::BodyRemoval removal{.entity_id = collision_body_id};
            m_container.subsystem_cast<PhysicsBus>().process(event, &removal);
        }
        m_loaded = false;
        m_vertices.clear();
        m_indices.clear();
        m_materials.clear();
        m_material_index.clear();
        m_textures.clear();
        m_sections.clear();
        m_switches.clear();
        m_active = -1;
        m_lightmaps.clear();
        m_lightmap_atlas = {};
        m_lightmap_tex.reset();
        m_lightmap_sampler.reset();
        m_normal_order_logged = false;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        if(!m_loaded)
            return;
        PlayerCamera const* camera{};
        for(auto e : p.select<PlayerCamera, PlayerInfo>())
        {
            auto const& [cam, info] = e.components();
            if(info.seat_idx != 0)
                continue;
            camera = &cam;
            break;
        }
        if(!camera)
            return;

        auto& resources = p.template subsystem<BlamResources>();
        update_section(camera->camera.position);
        /* Sky first (never writes depth), opaque, then water blended over
         * everything */
        for(int pass = 0; pass < 3; pass++)
            for(auto& mat : m_materials)
            {
                int const mat_pass = mat.sky ? 0 : mat.water ? 2 : 1;
                /* Glass, decals: nothing sensible to draw them with yet */
                if(mat_pass != pass || mat.section_draws.empty() ||
                   mat.texture == m_white)
                    continue;
                mat.draws.clear();
                for(auto const& d : mat.section_draws)
                    if(d.section < 0 || d.section == m_active)
                        mat.draws.push_back(d.cmd);
                if(mat.draws.empty())
                    continue;
                Vecf3 offset = mat.sky ? camera->camera.position : Vecf3{};
                Vecf3 const& eye = camera->camera.position;
                f32   scale  = mat.sky ? m_sky_scale : 1.f;
                auto  vert_u = gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Vertex,
                    gfx::uniform_pair<Matf4 const>{
                        {"camera"},
                        semantic::SpanOne<const Matf4>(camera->matrix)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"uv_scale"},
                        semantic::SpanOne<const Vecf2>(mat.uv_scale)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"model_offset"}, semantic::SpanOne<const Vecf3>(offset)},
                    gfx::uniform_pair<const f32>{
                        {"model_scale"}, semantic::SpanOne<const f32>(scale)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"detail_scale"},
                        semantic::SpanOne<const Vecf2>(mat.detail_scale)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"detail2_scale"},
                        semantic::SpanOne<const Vecf2>(mat.detail2_scale)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"illum_scale"},
                        semantic::SpanOne<const Vecf2>(mat.illum_scale)},
                    gfx::uniform_pair<Vecf2 const>{
                        {"mask_scale"},
                        semantic::SpanOne<const Vecf2>(mat.mask_scale)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"camera_position"}, semantic::SpanOne<const Vecf3>(eye)});
                i32  alpha_test = mat.alpha_test ? 1 : 0;
                auto frag_u     = gfx::make_uniform_list(
                    typing::graphics::ShaderStage::Fragment,
                    gfx::uniform_pair<const i32>{
                        {"alpha_test"},
                        semantic::SpanOne<const i32>(alpha_test)},
                    gfx::uniform_pair<Vecf3 const>{
                        {"illum_color"},
                        semantic::SpanOne<const Vecf3>(mat.illum_color)});
                auto lm_sampler = m_lightmap_sampler ? m_lightmap_sampler
                                                     : m_no_lightmaps_sampler;
                auto samplers = gfx::make_sampler_list(
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"diffuse", 0},
                        mat.sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"detail", 1},
                        mat.detail.sampler ? mat.detail.sampler : m_white_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"detail2", 2},
                        mat.detail2.sampler ? mat.detail2.sampler
                                            : m_white_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"illum", 3},
                        mat.illum.sampler ? mat.illum.sampler : m_white_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"lightmaps", 4},
                        lm_sampler,
                    },
                    gfx::sampler_definition_t{
                        typing::graphics::ShaderStage::Fragment,
                        {"mask", 5},
                        mat.mask.sampler ? mat.mask.sampler : m_white_sampler,
                    });
                auto res = m_api->submit(
                    {
                        .program       = m_programs[static_cast<size_t>(mat.variant)],
                        .vertices      = m_vao,
                        .render_target = resources.offscreen,
                        .call          = {.indexed = true, .instanced = true},
                        .data          = mat.draws,
                    },
                    vert_u,
                    frag_u,
                    samplers,
                    gfx::view_state{
                        .view = typing::vector_types::Veci4{
                            0,
                            0,
                            resources.offscreen_size.x,
                            resources.offscreen_size.y},
                        .depth = gfx::depth_state{.reversed = true},
                    },
                    gfx::depth_extended_state{.depth_write = !mat.sky && !mat.water},
                    gfx::blend_state{});
                if(res.has_value() && !m_draw_error_logged)
                {
                    cWarning("H2: failed to draw: {}", std::get<1>(*res));
                    m_draw_error_logged = true;
                }
            }
    }

  private:
    void ensure_gpu()
    {
        if(m_api)
            return;
        m_api = &m_container.subsystem_cast<gfx::system>();
        auto& api = *m_api;

        m_vao = api.alloc_vertex_array();
        m_vao->alloc();
        m_vao->add(
            gfx::vertex_attribute{
                .index = 0,
                .value = {.offset = 0, .stride = sizeof(Vertex), .count = 3},
            });
        m_vao->add(
            gfx::vertex_attribute{
                .index = 1,
                .value =
                    {.offset = offsetof(Vertex, texcoord),
                     .stride = sizeof(Vertex),
                     .count  = 2},
            });
        m_vao->add(
            gfx::vertex_attribute{
                .index = 2,
                .value =
                    {.offset = offsetof(Vertex, normal),
                     .stride = sizeof(Vertex),
                     .count  = 3},
            });
        m_vao->add(
            gfx::vertex_attribute{
                .index = 3,
                .value =
                    {.offset = offsetof(Vertex, lightmap),
                     .stride = sizeof(Vertex),
                     .count  = 3},
            });
        m_vao->set_attribute_names({
            {"position", 0},
            {"texcoord", 1},
            {"normal", 2},
            {"lightmap", 3},
        });

        m_vertex_buf =
            api.alloc_buffer(gfx::buffers::vertex, semantic::RSCA::ReadOnly);
        m_element_buf =
            api.alloc_buffer(gfx::buffers::element, semantic::RSCA::ReadOnly);
        m_vertex_buf->alloc();
        m_element_buf->alloc();
        m_vao->set_buffer(gfx::buffers::vertex, m_vertex_buf, 0);
        m_vao->set_buffer(gfx::buffers::element, m_element_buf);

        /* Stand-in for materials without a usable diffuse map */
        m_white = api.alloc_texture(
            gfx::textures::d2,
            typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8),
            1);
        m_white->alloc(size_3d<u32>{2, 2, 1});
        m_white->upload(std::vector<u8>(16, 0xFF), Veci2{0, 0}, Veci2{2, 2}, 0);
        m_white_sampler = m_white->sampler();
        m_white_sampler->alloc();

        m_no_lightmaps = std::make_shared<gfx::compat::texture_2da_t>(
            &api, typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8), 1);
        m_no_lightmaps->alloc(size_3d<u32>{2, 2, 1});
        m_no_lightmaps->upload(
            std::vector<u8>(16, 0xFF), Veci3{0, 0, 0}, size_3d<i32>{2, 2, 1});
        m_no_lightmaps_sampler = m_no_lightmaps->sampler();
        m_no_lightmaps_sampler->alloc();

        for(size_t i = 0; i < m_programs.size(); i++)
            create_program(
                api,
                shader_pair_t{
                    .vertex_file   = "h2",
                    .fragment_file = variant_files[i],
                    .shader        = m_programs[i],
                });
    }

    template<typename V>
    bool load_map(dim::map_container<V> const& map, dim::raw_pool const& pool)
    {
        auto const* scnr_tag = map.scenario();
        if(!scnr_tag)
            return false;
        auto const* scnr =
            scnr_tag->template data<geo::scnr_bsps_t>(map.magic());
        if(!scnr)
            return false;
        auto bsp_refs = scnr->structure_bsps.data(map.magic());
        if(bsp_refs.has_error())
            return false;

        std::optional<Vecf3> center;
        u32                  bsp_idx = 0;
        for(auto const& ref : bsp_refs.value())
        {
            auto collision = std::make_shared<CollisionMesh>();
            /* The BSP region has its own virtual base */
            auto const* bsp       = ref.body(map.magic());
            auto        bsp_magic = ref.region_magic(map.magic());
            if(!bsp)
                continue;
            if(!center)
                center = Vecf3{
                    (from_le(bsp->x_min) + from_le(bsp->x_max)) / 2.f,
                    (from_le(bsp->y_min) + from_le(bsp->y_max)) / 2.f,
                    (from_le(bsp->z_min) + from_le(bsp->z_max)) / 2.f,
                };

            std::vector<i32> materials;
            if(auto shader_refs = bsp->shaders.data(bsp_magic);
               !shader_refs.has_error())
                for(auto const& sref : shader_refs.value())
                    materials.push_back(
                        material_for(map, pool, sref.shader.tag_id));

            gather_collision(*bsp, bsp_magic, *collision);
            auto const* bsp_tag = map.tag_at(ref.bsp.tag_id);
            m_sections.push_back(Section{
                .name      = bsp_tag ? std::string(map.tag_name(*bsp_tag)) : "",
                .lo        = {from_le(bsp->x_min), from_le(bsp->y_min), from_le(bsp->z_min)},
                .hi        = {from_le(bsp->x_max), from_le(bsp->y_max), from_le(bsp->z_max)},
                .collision = std::move(collision),
            });

            /* Lightmaps: one image per cluster, through the region's ltmp */
            std::vector<LightmapImage const*> cluster_lightmaps;
            if(auto const* region = ref.region(map.magic()))
                if(auto const* ltmp = dim::ltmp::from_region(*region, bsp_magic))
                    cluster_lightmaps =
                        load_lightmaps(map, pool, *ltmp, bsp_magic);

            u32  cluster_count = 0, instance_count = 0, orphan_count = 0;
            u32  sections  = 0;
            auto clusters  = bsp->clusters.data(bsp_magic);
            auto geom      = bsp->sections.data(bsp_magic);
            auto instances = bsp->instances.data(bsp_magic);
            if(!clusters.has_error())
                for(auto const& cluster : clusters.value())
                {
                    LightmapImage const* lightmap =
                        cluster_count < cluster_lightmaps.size()
                            ? cluster_lightmaps[cluster_count]
                            : nullptr;
                    cluster_count++;
                    sections += append_section(
                        pool,
                        bsp_magic,
                        cluster,
                        Matf4(1.f),
                        nullptr,
                        materials,
                        false,
                        lightmap,
                        static_cast<i32>(bsp_idx));
                }
            std::vector<bool> instanced;
            if(!geom.has_error())
                instanced.assign(geom.value().size(), false);
            if(!geom.has_error() && !instances.has_error())
                for(auto const& inst : instances.value())
                {
                    auto sidx = inst.section_index();
                    if(sidx < 0 ||
                       static_cast<size_t>(sidx) >= geom.value().size())
                        continue;
                    instanced[sidx] = true;
                    instance_count++;
                    sections += append_section(
                        pool,
                        bsp_magic,
                        geom.value()[sidx],
                        instance_transform(inst),
                        nullptr,
                        materials,
                        false,
                        nullptr,
                        static_cast<i32>(bsp_idx));
                }
            if(!geom.has_error())
                for(size_t i = 0; i < geom.value().size(); i++)
                    if(!instanced[i])
                    {
                        orphan_count++;
                        sections += append_section(
                            pool,
                            bsp_magic,
                            geom.value()[i],
                            Matf4(1.f),
                            nullptr,
                            materials,
                            false,
                            nullptr,
                            static_cast<i32>(bsp_idx));
                    }

            cDebug(
                "H2: sbsp {} ({}): {} of {} clusters + {} instances + {} "
                "orphan sections emitted, {} materials",
                bsp_idx++,
                bsp_tag ? map.tag_name(*bsp_tag) : std::string_view{},
                sections,
                cluster_count,
                instance_count,
                orphan_count,
                materials.size());
        }

        i32 start_section = 0;
        if(auto const* scenario =
               scnr_tag->template data<dim::scn::scenario_t>(map.magic()))
        {
            load_sky(map, pool, *scenario);
            load_objects(map, pool, *scenario);
            load_switches(map, *scenario);
            if(auto locs = scenario->player_starting_locations.data(map.magic());
               !locs.has_error() && !locs.value().empty())
                start_section = locs.value()[0].bsp_index();
        }

        if(m_vertices.empty())
            return false;

        upload_lightmaps();
        m_vertex_buf->commit(
            Span<const Vertex>(m_vertices.data(), m_vertices.size()));
        m_element_buf->commit(
            Span<const u16>(m_indices.data(), m_indices.size()));
        u32 draws = 0;
        for(auto const& mat : m_materials)
        {
            draws += mat.section_draws.size();
            if(!mat.water)
                continue;
            Vecf3 lo{1e9f}, hi{-1e9f};
            for(auto const& sd : mat.section_draws)
                for(u32 i = 0; i < sd.cmd.elements.count; i++)
                {
                    auto const& d = sd.cmd;
                    auto const& v = m_vertices
                        [d.elements.vertex_offset +
                         m_indices[d.elements.offset / sizeof(u16) + i]];
                    lo = glm::min(lo, v.position);
                    hi = glm::max(hi, v.position);
                }
            cDebug(
                "H2: water {}: {} draws, ({}, {}, {}) .. ({}, {}, {})",
                mat.name,
                mat.section_draws.size(),
                lo.x,
                lo.y,
                lo.z,
                hi.x,
                hi.y,
                hi.z);
        }
        cDebug(
            "H2: {}: {} vertices, {} indices, {} materials, {} draws",
            map.name(),
            m_vertices.size(),
            m_indices.size(),
            m_materials.size(),
            draws);
        m_vertices = std::vector<Vertex>();
        m_indices  = std::vector<u16>();

        Vecf3 eye = *center + Vecf3{0.f, 0.f, 1.f};
        f32   yaw = 0.f;
        if(auto const* spawns =
               scnr_tag->template data<dim::scn::scenario_t>(map.magic()))
            if(auto locs = spawns->player_starting_locations.data(map.magic());
               !locs.has_error() && !locs.value().empty())
            {
                auto const& spawn = locs.value()[0];
                eye               = Vecf3{
                    from_le(spawn.position[0]),
                    from_le(spawn.position[1]),
                    from_le(spawn.position[2]) + 0.6f};
                yaw = glm::radians(from_le(spawn.facing_degrees));
            }
        cDebug("H2: camera at ({}, {}, {}), yaw {}", eye.x, eye.y, eye.z, yaw);
        place_camera(eye, yaw);
        if(start_section < 0 || static_cast<size_t>(start_section) >= m_sections.size())
            start_section = 0;
        set_active(start_section);
        if(auto window = m_container.service<comp_app::WindowInfo>())
            window->setName(fmt::format("Blam! : {}", map.name()));
        m_loaded = true;

        /* The Halo 1 systems key off LoadingStatus and would read their
         * empty container, so only the app-level state says "loaded" (which
         * is what the dummy plug waits for) */
        if(auto* app_info = m_container.service<comp_app::AppInfo>())
            app_info->setState(comp_app::interfaces::AppInfo::loaded);
        return true;
    }

    /* Appends a collision BSP's surfaces as fanned triangles */
    template<dim::heap_t Heap>
    static void append_collision(
        dim::coll::bsp_t<Heap> const&    bsp,
        dim::magic_ptr_base<Heap> const& magic,
        Matf4 const&                     transform,
        CollisionMesh&                   out)
    {
        auto surfaces = bsp.surfaces.data(magic);
        auto edges    = bsp.edges.data(magic);
        auto vertices = bsp.vertices.data(magic);
        if(surfaces.has_error() || edges.has_error() || vertices.has_error())
            return;
        i32 const vbase = static_cast<i32>(out.vertices.size());
        /* The BSP's bounding surfaces sit on vertices at infinity, which a
         * quantized BVH cannot hold */
        std::vector<bool> finite;
        for(auto const& v : vertices.value())
        {
            Vecf3 point{from_le(v.point[0]), from_le(v.point[1]), from_le(v.point[2])};
            finite.push_back(
                std::isfinite(point.x) && std::isfinite(point.y) &&
                std::isfinite(point.z));
            out.vertices.push_back(
                finite.back() ? Vecf3(transform * Vecf4(point, 1.f)) : Vecf3{});
        }
        std::vector<i32> polygon;
        for(u32 si = 0; si < surfaces.value().size(); si++)
        {
            if(surfaces.value()[si].has(dim::coll::surface_flags_t::invalid))
                continue;
            polygon.clear();
            bool usable = true;
            dim::coll::walk_surface(
                surfaces.value(), edges.value(), si, [&](i16 vertex) {
                    if(vertex >= 0 &&
                       static_cast<size_t>(vertex) < vertices.value().size() &&
                       finite[vertex])
                        polygon.push_back(vbase + vertex);
                    else
                        usable = false;
                });
            if(!usable)
                continue;
            for(size_t i = 2; i < polygon.size(); i++)
                out.indices.insert(
                    out.indices.end(), {polygon[0], polygon[i - 1], polygon[i]});
        }
    }

    /* World collision plus every placed instance's definition collision */
    void gather_collision(
        geo::bsp_header_t const& bsp,
        dim::bsp_magic const&    magic,
        CollisionMesh&           out)
    {
        size_t before = out.indices.size();
        if(auto coll = bsp.collision.data(magic);
           !coll.has_error() && !coll.value().empty())
            append_collision(coll.value()[0], magic, Matf4(1.f), out);
        size_t world = out.indices.size();
        auto   geom      = bsp.sections.data(magic);
        auto   instances = bsp.instances.data(magic);
        if(!geom.has_error() && !instances.has_error())
            for(auto const& inst : instances.value())
            {
                auto sidx = inst.section_index();
                if(sidx < 0 || static_cast<size_t>(sidx) >= geom.value().size())
                    continue;
                append_collision(
                    geom.value()[sidx].collision,
                    magic,
                    instance_transform(inst),
                    out);
            }
        Vecf3 lo{1e9f}, hi{-1e9f};
        for(auto const& v : out.vertices)
        {
            lo = glm::min(lo, v);
            hi = glm::max(hi, v);
        }
        cDebug(
            "H2: collision: {} world + {} instance triangles, bounds ({}, {}, "
            "{}) .. ({}, {}, {}); render bounds ({}, {}, {}) .. ({}, {}, {})",
            (world - before) / 3,
            (out.indices.size() - world) / 3,
            lo.x,
            lo.y,
            lo.z,
            hi.x,
            hi.y,
            hi.z,
            from_le(bsp.x_min),
            from_le(bsp.y_min),
            from_le(bsp.z_min),
            from_le(bsp.x_max),
            from_le(bsp.y_max),
            from_le(bsp.z_max));
    }

    /* BSP switch trigger volumes, as axis-aligned boxes */
    template<typename V>
    void load_switches(
        dim::map_container<V> const& map, dim::scn::scenario_t const& scnr)
    {
        auto volumes  = scnr.trigger_volumes.data(map.magic());
        auto switches = scnr.bsp_switch_triggers.data(map.magic());
        if(volumes.has_error() || switches.has_error())
            return;
        for(auto const& sw : switches.value())
        {
            i16 vi = sw.trigger_volume();
            if(vi < 0 || static_cast<size_t>(vi) >= volumes.value().size())
                continue;
            auto const& v = volumes.value()[vi];
            Vecf3       a{from_le(v.position[0]), from_le(v.position[1]), from_le(v.position[2])};
            Vecf3       e{from_le(v.extents[0]), from_le(v.extents[1]), from_le(v.extents[2])};
            m_switches.push_back(SwitchTrigger{
                .lo          = glm::min(a, a + e),
                .hi          = glm::max(a, a + e),
                .source      = sw.source_bsp(),
                .destination = sw.destination_bsp(),
            });
        }
        cDebug("H2: {} bsp switch triggers", m_switches.size());
    }

    /* Switch sections when the camera enters a trigger for the active one.
     * A camera that left the active section without crossing one (freecam,
     * teleport) gets the smallest section whose bounds hold it. */
    void update_section(Vecf3 const& eye)
    {
        auto inside = [&](Vecf3 const& lo, Vecf3 const& hi) {
            return glm::all(glm::greaterThanEqual(eye, lo)) &&
                   glm::all(glm::lessThanEqual(eye, hi));
        };
        for(auto const& sw : m_switches)
            if(sw.source == m_active && sw.destination != m_active &&
               sw.destination >= 0 &&
               static_cast<size_t>(sw.destination) < m_sections.size() &&
               inside(sw.lo, sw.hi))
            {
                set_active(sw.destination);
                return;
            }
        if(m_active >= 0 && inside(m_sections[m_active].lo, m_sections[m_active].hi))
            return;
        i32 best = -1;
        f32 best_volume = 0.f;
        for(size_t i = 0; i < m_sections.size(); i++)
        {
            auto const& sec = m_sections[i];
            if(!inside(sec.lo, sec.hi))
                continue;
            Vecf3 size   = sec.hi - sec.lo;
            f32   volume = size.x * size.y * size.z;
            if(best < 0 || volume < best_volume)
                best = static_cast<i32>(i), best_volume = volume;
        }
        if(best >= 0)
            set_active(best);
    }

    /* The active section's collision is the one static body */
    void set_active(i32 section)
    {
        if(section == m_active || section < 0 ||
           static_cast<size_t>(section) >= m_sections.size())
            return;
        if(m_active >= 0)
        {
            Physics::Event       event{.type = Physics::Event::BodyRemoval};
            Physics::BodyRemoval removal{.entity_id = collision_body_id};
            m_container.subsystem_cast<PhysicsBus>().process(event, &removal);
        }
        m_active = section;
        cDebug("H2: section {} ({}) active", section, m_sections[section].name);
        submit_collision(m_sections[section].collision);
    }

    /* One static body for the active section, like the Halo 1 world mesh */
    void submit_collision(std::shared_ptr<CollisionMesh> mesh)
    {
#if defined(FEATURE_ENABLE_BULLET3)
        if(!mesh || mesh->indices.empty())
            return;
        auto body       = std::make_shared<Physics::BodyCreationPrebuilt>();
        body->entity_id = collision_body_id;
        body->mesh_iface = std::make_unique<btTriangleIndexVertexArray>();
        btIndexedMesh part;
        part.m_numTriangles        = static_cast<int>(mesh->indices.size() / 3);
        part.m_triangleIndexBase   = reinterpret_cast<unsigned char const*>(
            mesh->indices.data());
        part.m_triangleIndexStride = 3 * sizeof(i32);
        part.m_numVertices = static_cast<int>(mesh->vertices.size());
        part.m_vertexBase =
            reinterpret_cast<unsigned char const*>(mesh->vertices.data());
        part.m_vertexStride = sizeof(Vecf3);
        part.m_vertexType   = PHY_FLOAT;
        body->mesh_iface->addIndexedMesh(part, PHY_INTEGER);
        body->shape = std::make_unique<btBvhTriangleMeshShape>(
            body->mesh_iface.get(), true);
        body->keep_alive = mesh;
        Physics::Event event{.type = Physics::Event::BodyCreationPrebuilt};
        m_container.subsystem_cast<PhysicsBus>().process(event, body.get());
#else
        (void)mesh;
#endif
    }

    /*! Instance placement as a matrix: rows 0-2 of the 4x3 are the scaled
     * basis vectors, row 3 the translation */
    static Matf4 instance_transform(geo::bsp_instance_t const& inst)
    {
        f32 scale = from_le(inst.scale);
        f32 m[12];
        for(int i = 0; i < 12; i++)
            m[i] = from_le(inst.transform[i]);
        Matf4 out(1.f);
        out[0] = Vecf4{m[0], m[1], m[2], 0.f} * scale;
        out[1] = Vecf4{m[3], m[4], m[5], 0.f} * scale;
        out[2] = Vecf4{m[6], m[7], m[8], 0.f} * scale;
        out[3] = Vecf4{m[9], m[10], m[11], 1.f};
        return out;
    }

    /* Appends one section's geometry under `transform`. `bounds` selects
     * the i16-compressed model vertex format, null means BSP f32 vertices.
     * Returns 1 if anything was emitted. */
    template<typename Section, dim::heap_t Heap>
    u32 append_section(
        dim::raw_pool const&             pool,
        dim::magic_ptr_base<Heap> const& magic,
        Section const&                   section,
        Matf4 const&               transform,
        geo::model_bounds_t const* bounds,
        std::vector<i32> const&    materials,
        bool                             sky,
        LightmapImage const*             lightmap = nullptr,
        i32                              bsp      = -1)
    {
        u16 vertex_count = section.vertex_count();
        i32 data_size    = section.data_size();
        if(!vertex_count || data_size <= 0)
            return 0;
        auto block = pool.resolve(section.data, static_cast<u32>(data_size));
        if(!block)
            return 0;
        auto resources = section.resources.data(magic);
        if(resources.has_error())
            return 0;
        auto streams = find_streams(resources.value());
        if(!streams.index || !streams.position)
            return 0;

        i32 base        = section.base_address();
        u16 index_count = geo::block_index_count(*block);

        auto in_block = [&](geo::resource_t const* r, i32 size = -1) {
            if(!r || r->offset() < 0)
                return false;
            if(size < 0)
                size = r->size();
            return size >= 0 && static_cast<i64>(base) + r->offset() + size <=
                                    static_cast<i64>(block->size());
        };
        if(!index_count || !in_block(streams.index, index_count * 2) ||
           !in_block(streams.position))
            return 0;
        auto const* bytes = reinterpret_cast<u8 const*>(block->data());

        glm::mat3 const rotation = glm::mat3(transform);
        auto expand = [](i16 value, f32 min, f32 max) {
            f32 n = (static_cast<f32>(value) + 32768.f) / 65535.f;
            return min + n * (max - min);
        };

        u32  vbase   = static_cast<u32>(m_vertices.size());
        u32  pstride = streams.position->size() / vertex_count;
        bool has_uv  = in_block(streams.texcoord);
        u32  ustride = has_uv ? streams.texcoord->size() / vertex_count : 0;
        bool has_n   = in_block(streams.normal);
        u32  nstride = has_n ? streams.normal->size() / vertex_count : 0;
        if(pstride < (bounds ? 6u : 12u))
            return 0;
        /* 4 = packed normal; 12 = packed normal, binormal, tangent */
        if(!m_normal_order_logged && has_n && nstride != 4 && nstride != 12)
        {
            cDebug("H2: unhandled normal stream stride {}", nstride);
            m_normal_order_logged = true;
        }
        bool has_lm = lightmap && in_block(streams.lightmap) &&
                      streams.lightmap->size() / vertex_count == 4;
        Vecf2 const lm_scale{
            static_cast<f32>(lightmap ? lightmap->width : 0) / LightmapAtlas::page,
            static_cast<f32>(lightmap ? lightmap->height : 0) / LightmapAtlas::page};
        Vecf2 const lm_offset{
            static_cast<f32>(lightmap ? lightmap->x : 0) / LightmapAtlas::page,
            static_cast<f32>(lightmap ? lightmap->y : 0) / LightmapAtlas::page};

        std::vector<Vecf3> normals_a, normals_b;
        for(u32 v = 0; v < vertex_count; v++)
        {
            Vertex      out{};
            auto const* p = bytes + base + streams.position->offset() + v * pstride;
            Vecf3       pos;
            if(bounds)
            {
                i16 q[3];
                std::memcpy(q, p, 6);
                pos = {
                    expand(from_le(q[0]), from_le(bounds->x_min), from_le(bounds->x_max)),
                    expand(from_le(q[1]), from_le(bounds->y_min), from_le(bounds->y_max)),
                    expand(from_le(q[2]), from_le(bounds->z_min), from_le(bounds->z_max))};
            } else
            {
                f32 f[3];
                std::memcpy(f, p, 12);
                pos = {from_le(f[0]), from_le(f[1]), from_le(f[2])};
            }
            out.position = Vecf3(transform * Vecf4(pos, 1.f));
            if(has_uv)
            {
                auto const* t = bytes + base + streams.texcoord->offset() + v * ustride;
                if(bounds && ustride >= 4)
                {
                    i16 q[2];
                    std::memcpy(q, t, 4);
                    out.texcoord = {
                        expand(from_le(q[0]), from_le(bounds->u_min), from_le(bounds->u_max)),
                        expand(from_le(q[1]), from_le(bounds->v_min), from_le(bounds->v_max))};
                } else if(!bounds && ustride >= 8)
                {
                    f32 uv[2];
                    std::memcpy(uv, t, 8);
                    out.texcoord = {from_le(uv[0]), from_le(uv[1])};
                }
            }
            if(has_n && (nstride == 4 || nstride == 12))
            {
                u32 packed;
                std::memcpy(
                    &packed,
                    bytes + base + streams.normal->offset() + v * nstride,
                    4);
                packed = from_le(packed);
                normals_a.push_back(glm::normalize(rotation * unpack_11_11_10(packed)));
                normals_b.push_back(glm::normalize(rotation * unpack_10_11_11(packed)));
            }
            if(has_lm)
            {
                i16 q[2];
                std::memcpy(
                    q, bytes + base + streams.lightmap->offset() + v * 4, 4);
                /* SHORT2N: 0..32767 covers the chart */
                Vecf2 uv{from_le(q[0]) / 32767.f, from_le(q[1]) / 32767.f};
                out.lightmap = Vecf3(
                    uv * lm_scale + lm_offset, static_cast<f32>(lightmap->layer));
            }
            m_vertices.push_back(out);
        }

        std::vector<u16> indices(index_count);
        std::memcpy(
            indices.data(), bytes + base + streams.index->offset(), index_count * 2);
        for(auto& idx : indices)
            idx = from_le(idx);
        bool strip = static_cast<u32>(section.face_count()) * 3 != index_count;

        /* One triangle list per material of this section */
        i32 fallback = static_cast<i32>(m_materials.size()) - 1;
        std::unordered_map<i32, std::vector<u16>> tris;
        auto emit = [&](std::vector<u16>& out, Span<const u16> range) {
            if(!strip)
            {
                for(size_t i = 0; i + 2 < range.size(); i += 3)
                    out.insert(out.end(), {range[i], range[i + 1], range[i + 2]});
                return;
            }
            for(size_t i = 2; i < range.size(); i++)
            {
                u16 a = range[i - 2], b = range[i - 1], c = range[i];
                if(a == b || b == c || a == c)
                    continue;
                if(i % 2)
                    out.insert(out.end(), {b, a, c});
                else
                    out.insert(out.end(), {a, b, c});
            }
        };
        auto index_span = Span<const u16>(indices.data(), indices.size());
        bool split      = false;
        if(in_block(streams.submeshes))
        {
            auto const* subs = reinterpret_cast<geo::submesh_t const*>(
                bytes + base + streams.submeshes->offset());
            size_t count = streams.submeshes->size() / sizeof(geo::submesh_t);
            for(size_t s = 0; s < count; s++)
            {
                u32 start = subs[s].index_start();
                u32 len   = subs[s].index_length();
                i16 shader = subs[s].shader_index();
                if(!len || start + len > index_count)
                    continue;
                i32 mat = shader >= 0 &&
                                  static_cast<size_t>(shader) < materials.size()
                              ? materials[shader]
                              : fallback;
                emit(tris[mat], index_span.subspan(start, len));
                split = true;
            }
        }
        if(!split)
            emit(tris[fallback], index_span);

        /* Packed normals: pick the field order that agrees with the faces */
        if(!normals_a.empty())
        {
            f32 score_a = 0.f, score_b = 0.f;
            u32 counted = 0;
            for(auto const& [mat, list] : tris)
                for(size_t i = 0; i + 2 < list.size() && counted < 512; i += 3)
                {
                    auto const& p0 = m_vertices[vbase + list[i]].position;
                    auto const& p1 = m_vertices[vbase + list[i + 1]].position;
                    auto const& p2 = m_vertices[vbase + list[i + 2]].position;
                    Vecf3 face = glm::cross(p1 - p0, p2 - p0);
                    f32   len  = glm::length(face);
                    if(len < 1e-8f)
                        continue;
                    face /= len;
                    score_a += std::abs(glm::dot(face, normals_a[list[i]]));
                    score_b += std::abs(glm::dot(face, normals_b[list[i]]));
                    counted++;
                }
            bool use_a = score_a >= score_b;
            if(!m_normal_order_logged && counted)
            {
                cDebug(
                    "H2: packed normals: 11:11:10 agrees {:.2f}, 10:11:11 "
                    "agrees {:.2f} over {} faces",
                    score_a / counted,
                    score_b / counted,
                    counted);
                m_normal_order_logged = true;
            }
            auto const& chosen = use_a ? normals_a : normals_b;
            for(u32 v = 0; v < vertex_count; v++)
                m_vertices[vbase + v].normal = chosen[v];
        }

        u32 emitted = 0;
        for(auto& [mat, list] : tris)
        {
            if(list.empty() || mat < 0 ||
               static_cast<size_t>(mat) >= m_materials.size())
                continue;
            m_materials[mat].section_draws.push_back({
                .cmd =
                    {
                        .elements =
                            {
                                .count         = static_cast<u32>(list.size()),
                                .offset        = m_indices.size() * sizeof(u16),
                                .vertex_offset = vbase,
                                .type          = semantic::type_t::u16,
                            },
                        .instances = {.count = 1, .offset = 0},
                    },
                .section = bsp,
            });
            m_indices.insert(m_indices.end(), list.begin(), list.end());
            emitted++;
        }
        (void)sky;
        return emitted ? 1 : 0;
    }

    /* A render model under `transform`: permutation 0 of every region at
     * its most detailed LOD. Rigid sections are stored in their node's
     * space; everything else is already in model space. */
    template<typename V>
    u32 append_model(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::tag_t const&            mode_tag,
        Matf4 const&                 transform,
        bool                         sky,
        i32                          bsp = -1)
    {
        auto const* model =
            mode_tag.template data<geo::model_header_t>(map.magic());
        if(!model)
            return 0;
        auto bounds   = model->bounds.data(map.magic());
        auto sections = model->sections.data(map.magic());
        auto regions  = model->regions.data(map.magic());
        if(bounds.has_error() || bounds.value().empty() ||
           sections.has_error() || regions.has_error())
            return 0;

        std::vector<i32> materials;
        if(auto shader_refs = model->shaders.data(map.magic());
           !shader_refs.has_error())
            for(auto const& sref : shader_refs.value())
                materials.push_back(
                    material_for(map, pool, sref.shader.tag_id, sky));

        /* Parents precede their children in the node list */
        std::vector<Matf4> node_world;
        if(auto nodes = model->nodes.data(map.magic()); !nodes.has_error())
            for(auto const& node : nodes.value())
            {
                Vecf3 position{
                    from_le(node.position[0]),
                    from_le(node.position[1]),
                    from_le(node.position[2])};
                Quatf rotation{
                    from_le(node.rotation[3]),
                    from_le(node.rotation[0]),
                    from_le(node.rotation[1]),
                    from_le(node.rotation[2])};
                Matf4 local = glm::translate(Matf4(1.f), position) *
                              glm::mat4_cast(rotation);
                i16 parent = node.parent();
                node_world.push_back(
                    parent >= 0 && static_cast<size_t>(parent) < node_world.size()
                        ? node_world[parent] * local
                        : local);
            }

        u32 emitted = 0;
        for(auto const& region : regions.value())
        {
            auto perms = region.permutations.data(map.magic());
            if(perms.has_error() || perms.value().empty())
                continue;
            i16 sidx = perms.value()[0].best_section();
            if(sidx < 0 || static_cast<size_t>(sidx) >= sections.value().size())
                continue;
            auto const& section = sections.value()[sidx];
            Matf4       local   = transform;
            i16         node    = section.rigid_node_index();
            if(node >= 0 && static_cast<size_t>(node) < node_world.size())
                local = transform * node_world[node];
            emitted += append_section(
                pool,
                map.magic(),
                section,
                local,
                &bounds.value()[0],
                materials,
                sky,
                nullptr,
                bsp);
        }
        return emitted;
    }

    /* Decodes every cluster's lightmap (P8 image through its palette) into
     * the atlas; returns one entry per cluster, null where there is none */
    template<typename V>
    std::vector<LightmapImage const*> load_lightmaps(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::ltmp::header_t const&   ltmp,
        dim::bsp_magic const&        magic)
    {
        std::vector<LightmapImage const*> out;
        auto groups = ltmp.groups.data(magic);
        if(groups.has_error() || groups.value().empty())
            return out;
        auto const& group    = groups.value()[0];
        auto        infos    = group.cluster_render_info.data(magic);
        auto        palettes = group.section_palette.data(magic);
        if(infos.has_error() || palettes.has_error())
            return out;
        auto const* bitm_tag = map.tag_at(group.bitmap_group.tag_id);
        auto const* bitm =
            bitm_tag ? bitm_tag->template data<dim::bitm::header_t>(map.magic())
                     : nullptr;
        if(!bitm)
            return out;
        auto images = bitm->images.data(map.magic());
        if(images.has_error())
            return out;

        std::unordered_map<u32, size_t> decoded; /* (image, palette) -> slot */
        u32                             failed = 0;
        for(auto const& info : infos.value())
        {
            i16 image_idx = info.bitmap_index();
            i8  pal_idx   = info.palette_index;
            if(image_idx < 0 || static_cast<size_t>(image_idx) >= images.value().size() ||
               pal_idx < 0 || static_cast<size_t>(pal_idx) >= palettes.value().size())
            {
                out.push_back(nullptr);
                continue;
            }
            u32 key = static_cast<u32>(image_idx) << 8 | static_cast<u8>(pal_idx);
            if(auto it = decoded.find(key); it != decoded.end())
            {
                out.push_back(m_lightmaps[it->second].get());
                continue;
            }
            auto decoded_image = dim::ltmp::decode(
                pool, images.value()[image_idx], palettes.value()[pal_idx]);
            LightmapImage image;
            if(decoded_image)
            {
                image.width  = decoded_image->width;
                image.height = decoded_image->height;
                image.rgba   = std::move(decoded_image->rgba);
            }
            if(!decoded_image || !m_lightmap_atlas.place(image))
            {
                failed++;
                out.push_back(nullptr);
                continue;
            }
            m_lightmaps.push_back(
                std::make_unique<LightmapImage>(std::move(image)));
            decoded.emplace(key, m_lightmaps.size() - 1);
            out.push_back(m_lightmaps.back().get());
        }
        cDebug(
            "H2: lightmaps: {} clusters, {} images decoded, {} failed, {} "
            "atlas pages",
            out.size(),
            decoded.size(),
            failed,
            m_lightmap_atlas.pages.size());
        return out;
    }

    void upload_lightmaps()
    {
        if(m_lightmaps.empty())
            return;
        m_lightmap_tex = std::make_shared<gfx::compat::texture_2da_t>(
            m_api, typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8), 1);
        m_lightmap_tex->alloc(
            size_3d<u32>{
                LightmapAtlas::page,
                LightmapAtlas::page,
                static_cast<u32>(m_lightmap_atlas.pages.size())});
        for(auto& image : m_lightmaps)
            m_lightmap_tex->upload(
                std::move(image->rgba),
                Veci3{
                    static_cast<i32>(image->x),
                    static_cast<i32>(image->y),
                    static_cast<i32>(image->layer)},
                size_3d<i32>{
                    static_cast<i32>(image->width),
                    static_cast<i32>(image->height),
                    1});
        m_lightmaps.clear();
        m_lightmap_sampler = m_lightmap_tex->sampler();
        m_lightmap_sampler->alloc();
        m_lightmap_sampler->set_filtering(
            typing::Filtering::Linear, typing::Filtering::Linear);
    }

    /* obje -> hlmt -> mode */
    template<typename V>
    dim::tag_t const* render_model_of(
        dim::map_container<V> const& map, dim::tag_reference const& object)
    {
        auto const* obje_tag = map.tag_at(object.tag_id);
        auto const* obje =
            obje_tag
                ? obje_tag->template data<dim::obj::object_t>(map.magic())
                : nullptr;
        if(!obje)
            return nullptr;
        auto const* hlmt_tag = map.tag_at(obje->model.tag_id);
        auto const* hlmt =
            hlmt_tag ? hlmt_tag->template data<dim::obj::hlmt_t>(map.magic())
                     : nullptr;
        if(!hlmt)
            return nullptr;
        auto const* mode_tag = map.tag_at(hlmt->render_model.tag_id);
        return mode_tag && mode_tag->matches("mode") ? mode_tag : nullptr;
    }

    /* Every placement block of the scenario, then the multiplayer item and
     * vehicle spawns */
    template<typename V>
    void load_objects(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::scn::scenario_t const&  scnr)
    {
        auto place = [&](dim::tag_t const* mode_tag,
                         f32 const*        position,
                         f32 const*        rotation,
                         f32               scale,
                         i32               bsp) {
            Vecf3 pos{from_le(position[0]), from_le(position[1]), from_le(position[2])};
            Vecf3 rot = glm::radians(
                Vecf3{from_le(rotation[0]), from_le(rotation[1]), from_le(rotation[2])});
            /* Same yaw/pitch/roll order as the Halo 1 spawns */
            Quatf q = Quatf(Vecf3(0, 0, 1) * rot.x) * Quatf(Vecf3(0, 1, 0) * rot.y) *
                      Quatf(Vecf3(1, 0, 0) * rot.z);
            if(scale <= 0.f)
                scale = 1.f;
            Matf4 transform = glm::translate(Matf4(1.f), pos) * glm::mat4_cast(q) *
                              glm::scale(Matf4(1.f), Vecf3(scale));
            return append_model(map, pool, *mode_tag, transform, false, bsp) > 0;
        };

        auto block = [&](auto const& placements_block,
                         auto const& palette_block,
                         const char* what) {
            auto placements = placements_block.data(map.magic());
            auto palette    = palette_block.data(map.magic());
            if(placements.has_error() || palette.has_error())
                return;
            u32 placed = 0, skipped = 0;
            for(auto const& p : placements.value())
            {
                i16 pi = p.palette_index();
                dim::tag_t const* mode_tag =
                    pi >= 0 && static_cast<size_t>(pi) < palette.value().size()
                        ? render_model_of(map, palette.value()[pi].tag)
                        : nullptr;
                /* Manual BSP flags: one bit per structure BSP; none = all */
                u16 flags = static_cast<u16>(p.bsp_flags);
                i32 bsp   = flags ? static_cast<i32>(std::countr_zero(flags)) : -1;
                if(mode_tag &&
                   place(mode_tag, p.position, p.rotation, from_le(p.scale), bsp))
                    placed++;
                else
                    skipped++;
            }
            if(placed || skipped)
                cDebug("H2: {}: {} placed, {} skipped", what, placed, skipped);
        };
        block(scnr.scenery, scnr.scenery_palette, "scenery");
        block(scnr.bipeds, scnr.biped_palette, "bipeds");
        block(scnr.vehicles, scnr.vehicle_palette, "vehicles");
        block(scnr.equipment, scnr.equipment_palette, "equipment");
        block(scnr.weapons, scnr.weapon_palette, "weapons");
        block(scnr.machines, scnr.machine_palette, "machines");
        block(scnr.controls, scnr.control_palette, "controls");
        block(scnr.light_fixtures, scnr.light_fixture_palette, "light fixtures");

        /* Netgame spawns: the collection's first permutation stands in */
        if(auto spawns = scnr.netgame_equipment.data(map.magic());
           !spawns.has_error())
        {
            u32 placed = 0, skipped = 0;
            for(auto const& spawn : spawns.value())
            {
                auto const* coll_tag = map.tag_at(spawn.collection.tag_id);
                auto const* coll =
                    coll_tag ? coll_tag->template data<dim::scn::item_collection_t>(
                                   map.magic())
                             : nullptr;
                dim::tag_t const* mode_tag = nullptr;
                if(coll)
                    if(auto perms = coll->permutations.data(map.magic());
                       !perms.has_error() && !perms.value().empty())
                        mode_tag = render_model_of(map, perms.value()[0].item);
                if(mode_tag &&
                   place(mode_tag, spawn.position, spawn.orientation, 1.f, -1))
                    placed++;
                else
                    skipped++;
            }
            cDebug("H2: netgame equipment: {} placed, {} skipped", placed, skipped);
        }
    }

    /* The first sky's model, kept around the camera; scaled into the far
     * plane since its radius is in the thousands */
    template<typename V>
    void load_sky(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::scn::scenario_t const&  scnr)
    {
        m_sky_scale  = 1.f;
        auto skies   = scnr.skies.data(map.magic());
        if(skies.has_error() || skies.value().empty())
            return;
        auto const* sky_tag = map.tag_at(skies.value()[0].sky.tag_id);
        auto const* sky =
            sky_tag ? sky_tag->template data<dim::obj::sky_t>(map.magic())
                    : nullptr;
        if(!sky)
            return;
        auto const* mode_tag = map.tag_at(sky->render_model.tag_id);
        if(!mode_tag || !mode_tag->matches("mode"))
            return;
        size_t first    = m_vertices.size();
        u32    sections = append_model(map, pool, *mode_tag, Matf4(1.f), true);
        f32    radius   = 0.f;
        for(size_t i = first; i < m_vertices.size(); i++)
            radius = std::max(radius, glm::length(m_vertices[i].position));
        constexpr f32 sky_radius = 80.f; /* inside the 100 unit far plane */
        if(radius > sky_radius)
            m_sky_scale = sky_radius / radius;
        cDebug(
            "H2: sky {}: {} sections, radius {}",
            map.tag_name(*sky_tag),
            sections,
            radius);
    }

    /* Index into m_materials for a shad tag, created on first use. The last
     * material is always the untextured fallback. */
    template<typename V>
    i32 material_for(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::datum_index             shad_id,
        bool                         sky = false)
    {
        if(m_materials.empty())
            m_materials.push_back(Material{
                .name    = "<fallback>",
                .texture = m_white,
                .sampler = m_white_sampler,
            });
        auto const* shad_tag = map.tag_at(shad_id);
        if(!shad_tag)
            return static_cast<i32>(m_materials.size()) - 1;
        u32 key = shad_id.index() | (sky ? 0x10000u : 0u);
        if(auto it = m_material_index.find(key); it != m_material_index.end())
            return it->second;

        Material mat{
            .name    = std::string(map.tag_name(*shad_tag)),
            .texture = m_white,
            .sampler = m_white_sampler,
            .sky     = sky,
        };
        bool resolved = false;
        if(auto const* shad =
               shad_tag->template data<geo::shad_header_t>(map.magic()))
        {
            auto base = dim::material::resolve(map, *shad);
            resolved  = base.resolved;
            for(auto name : base.unknown_passes)
                if(m_unknown_passes.insert(std::string(name)).second)
                    cDebug("H2: no pass definition for {}", name);
            /* Runtime properties name a diffuse too, untiled; good enough
             * for templates the pass table does not know */
            dim::tag_t const* bitm_tag = base.bitmap;
            if(!bitm_tag)
                if(auto rt = shad->runtime_properties.data(map.magic());
                   !rt.has_error() && !rt.value().empty())
                    bitm_tag = map.tag_at(rt.value()[0].diffuse.tag_id);
            mat.uv_scale   = {base.scale_u, base.scale_v};
            mat.alpha_test = base.alpha_test;
            mat.water      = base.water;
            if(mat.water)
                if(auto bump = base.layer("bump_map"))
                    bitm_tag = bump->bitmap, mat.uv_scale = {bump->scale_u, bump->scale_v};
            if(auto tex = texture_for(map, pool, bitm_tag))
            {
                mat.texture = tex->texture;
                mat.sampler = tex->sampler;
            }
            /* Detail layers, by usage: a plain detail map, or two blended by
             * the base map's alpha */
            if(auto d1 = base.layer("blend_detail_map_1"),
               d2 = base.layer("blend_detail_map_2");
               d1 && d2)
            {
                auto t1 = texture_for(map, pool, d1->bitmap);
                auto t2 = texture_for(map, pool, d2->bitmap);
                if(t1 && t2)
                {
                    mat.detail_mode   = 2;
                    mat.detail        = *t1;
                    mat.detail_scale  = {d1->scale_u, d1->scale_v};
                    mat.detail2       = *t2;
                    mat.detail2_scale = {d2->scale_u, d2->scale_v};
                }
            } else if(auto d = base.layer("detail_map"))
                if(auto t = texture_for(map, pool, d->bitmap))
                {
                    mat.detail_mode  = 1;
                    mat.detail       = *t;
                    mat.detail_scale = {d->scale_u, d->scale_v};
                }
            if(sky)
                if(auto m = base.layer("cloud_mask"))
                    if(auto t = texture_for(map, pool, m->bitmap))
                    {
                        mat.mask       = *t;
                        mat.mask_scale = {m->scale_u, m->scale_v};
                    }
            /* Lights and panels; the illum colour constants are not
             * evaluated, so the map adds at full strength */
            if(base.usage != "self_illum_map")
                if(auto il = base.layer("self_illum_map"))
                    if(auto t = texture_for(map, pool, il->bitmap))
                    {
                        mat.illum       = *t;
                        mat.illum_scale = {il->scale_u, il->scale_v};
                        mat.illum_color = Vecf3{
                            base.illum_color[0],
                            base.illum_color[1],
                            base.illum_color[2]} / 255.f;
                    }
        }
        mat.variant = mat.sky     ? (mat.mask.sampler ? Variant::sky_mask : Variant::sky)
                      : mat.water ? Variant::water
                      : static_cast<Variant>(
                            mat.detail_mode + (mat.illum.sampler ? 3 : 0));
        cDebug(
            "H2: shad {}: uv scale {}x{}, {}{}{}",
            mat.name,
            mat.uv_scale.x,
            mat.uv_scale.y,
            mat.texture == m_white ? "no diffuse" : "textured",
            mat.detail_mode == 2   ? ", blended detail"
            : mat.detail_mode == 1 ? ", detail"
                                   : "",
            resolved ? "" : " (template unresolved)");
        m_materials.insert(m_materials.end() - 1, std::move(mat));
        i32 idx = static_cast<i32>(m_materials.size()) - 2;
        /* Everything after the inserted slot moved by one: only the
         * fallback, which holds no cached index */
        m_material_index.emplace(key, idx);
        return idx;
    }

    /* Uploaded once per bitm tag; detail maps are shared by many shaders */
    template<typename V>
    std::optional<Texture> texture_for(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::tag_t const*            bitm_tag)
    {
        if(!bitm_tag || !bitm_tag->matches("bitm"))
            return std::nullopt;
        auto key = static_cast<u16>(bitm_tag - map.tags().data());
        if(auto it = m_textures.find(key); it != m_textures.end())
            return it->second;
        auto tex = upload_bitmap(map, pool, *bitm_tag);
        if(!tex)
            return std::nullopt;
        Texture out{.texture = *tex, .sampler = (*tex)->sampler()};
        out.sampler->alloc();
        out.sampler->set_filtering(
            typing::Filtering::Linear,
            typing::Filtering::Linear,
            typing::Filtering::Linear);
        out.sampler->set_anisotropic(4);
        m_textures.emplace(key, out);
        return out;
    }

    /* First image of a bitm tag with its mip chain, from whichever LOD is
     * present in the files we have */
    template<typename V>
    std::optional<std::shared_ptr<gfx::texture_2d_t>> upload_bitmap(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::tag_t const&            bitm_tag)
    {
        auto const* bitm =
            bitm_tag.template data<dim::bitm::header_t>(map.magic());
        if(!bitm)
            return std::nullopt;
        auto images = bitm->images.data(map.magic());
        if(images.has_error() || images.value().empty())
            return std::nullopt;
        dim::bitm::image_t const& img = images.value()[0];
        if(from_le(img.type) != blam::bitm::type_t::tex_2d)
            return std::nullopt;
        auto format = from_le(img.format);
        auto fmt    = to_pixdesc(format);
        if(!fmt)
            return std::nullopt;

        u32 width  = static_cast<u32>(from_le(img.width));
        u32 height = static_cast<u32>(from_le(img.height));
        u32 base   = level_bytes(format, width, height);
        if(!width || !height || !base)
            return std::nullopt;
        bool compressed = is_compressed(format);
        u32  bpp        = compressed ? 0 : base / (width * height);
        u32  mip_count  = static_cast<u32>(std::max<i16>(from_le(img.mipmaps), 0)) + 1;

        for(int lod = 0; lod < 3; lod++)
        {
            u32 lod_size = from_le(img.lod_size[lod]);
            if(lod_size < base)
                continue;
            auto raw = pool.resolve(img.lod_offset[lod], lod_size);
            if(!raw)
                continue;
            auto const* src = reinterpret_cast<u8 const*>(raw->data());

            /* Mips follow the base level back to back */
            struct level_t
            {
                u32             w, h;
                std::vector<u8> pixels;
            };
            std::vector<level_t> levels;
            u32                  offset = 0, lw = width, lh = height;
            while(levels.size() < mip_count)
            {
                u32 bytes = level_bytes(format, lw, lh);
                if(!bytes || offset + bytes > lod_size)
                    break;
                if(compressed && (lw < 4 || lh < 4))
                    break;
                level_t level{.w = lw, .h = lh};
                level.pixels.resize(bytes);
                Span<const u8> in(src + offset, bytes);
                Span<u8>       out(level.pixels.data(), bytes);
                if(!compressed && img.swizzled())
                {
                    if(!blam::swizzle::deswizzle_bytes(in, out, lw, lh, bpp))
                        std::memcpy(out.data(), in.data(), bytes);
                } else
                    std::memcpy(out.data(), in.data(), bytes);
                if(format == blam::bitm::format_t::V8U8)
                    for(auto& b : level.pixels)
                        b = static_cast<u8>(b + 0x80);
                /* A8R8G8B8 is stored B,G,R,A */
                if(format == blam::bitm::format_t::ARGB8 ||
                   format == blam::bitm::format_t::XRGB8)
                    for(u32 i = 0; i + 3 < bytes; i += 4)
                    {
                        std::swap(level.pixels[i], level.pixels[i + 2]);
                        if(format == blam::bitm::format_t::XRGB8)
                            level.pixels[i + 3] = 0xFF;
                    }
                levels.push_back(std::move(level));
                offset += bytes;
                if(lw == 1 && lh == 1)
                    break;
                lw = std::max(lw / 2, 1u);
                lh = std::max(lh / 2, 1u);
            }
            if(levels.empty())
                break;

            cDebug(
                "H2: bitm {}: {}x{} format {} lod{} ({} bytes) -> {} of {} "
                "mip levels{}",
                map.tag_name(bitm_tag),
                width,
                height,
                static_cast<u16>(format),
                lod + 1,
                lod_size,
                levels.size(),
                mip_count,
                img.swizzled() && !compressed ? ", deswizzled" : "");
            auto tex = m_api->alloc_texture(
                gfx::textures::d2, *fmt, static_cast<u32>(levels.size()));
            tex->alloc(size_3d<u32>{width, height, 1});
            /* One- and two-channel formats land in R/RG; spread them */
            using gfx::textures::swizzle_t;
            switch(format)
            {
            case blam::bitm::format_t::Y8:
                tex->set_swizzle(
                    swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::one);
                break;
            case blam::bitm::format_t::AY8:
                tex->set_swizzle(
                    swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::red);
                break;
            case blam::bitm::format_t::A8:
                tex->set_swizzle(
                    swizzle_t::one, swizzle_t::one, swizzle_t::one, swizzle_t::red);
                break;
            case blam::bitm::format_t::A8Y8:
                tex->set_swizzle(
                    swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::green);
                break;
            default:
                break;
            }
            for(size_t i = 0; i < levels.size(); i++)
            {
                auto& level = levels[i];
                Veci2 size{static_cast<i32>(level.w), static_cast<i32>(level.h)};
                tex->upload(
                    std::move(level.pixels),
                    Veci2{0, 0},
                    size,
                    static_cast<i32>(i));
            }
            return tex;
        }
        return std::nullopt;
    }

    /* The Halo 1 loader creates the seats from the scenario; a Halo 2 map
     * has to do the same before anything can look at it */
    void place_camera(Vecf3 const& position, f32 yaw)
    {
        bool have_player = false;
        for([[maybe_unused]] auto const& p : m_container.select<PlayerInfo>())
        {
            have_player = true;
            break;
        }
        if(!have_player)
        {
            auto ref = m_container.create_entity(shared_recipes::player_recipe);
            for(size_t frame = 0; frame < Visibility::type::frame_count_v;
                frame++)
                m_container.get_at<Visibility>(ref.id(), frame)->skip_render_for =
                    0;
            auto& info      = ref.get<PlayerInfo>();
            info.player_idx = 0;
            info.seat_idx   = 0;
            auto& camera    = ref.get<PlayerCamera>();
            camera.keyboard.enabled = true;
            camera.controller.index = 0;
        }
        static const glm::mat3 bsp_basis_inv{{0, 1, 0}, {0, 0, 1}, {1, 0, 0}};
        for(auto entity : m_container.select<PlayerCamera, PlayerInfo, PlayerInput>())
        {
            auto [cam, info, input] = entity.components();
            if(info.is_remote())
                continue;
            cam.controller.opts.sens.move = {.1f, .1f};
            cam.controller.opts.curve     = 200.f;
            cam.camera_opts.accel.alt     = 50.f;
            cam.camera_opts.world_basis   = bsp_basis_inv;
            cam.camera.position           = position;
            cam.camera.rotation =
                glm::angleAxis(glm::pi<f32>() - yaw, Vecf3{0.f, 1.f, 0.f});
            /* No collision mesh to stand on: fly */
            info.mode.physics = false;
            input.input_mode  = PlayerInput::input_mode_t::game;
        }
        update_camera_aspect(m_container);
    }

    /* Entity id of the map's static collision body; out of the range the
     * object spawns use */
    static constexpr u64 collision_body_id = 0x4832'0000'0000'0000ull;

    compo::EntityContainer& m_container;
    gfx::api*               m_api{nullptr};

    std::shared_ptr<gfx::vertex_array_t> m_vao;
    std::shared_ptr<gfx::buffer_t>       m_vertex_buf;
    std::shared_ptr<gfx::buffer_t>       m_element_buf;
    std::array<std::shared_ptr<gfx::program_t>, static_cast<size_t>(Variant::count)>
        m_programs;
    std::shared_ptr<gfx::texture_2d_t>   m_white;
    std::shared_ptr<gfx::sampler_t>      m_white_sampler;

    std::vector<Section>           m_sections;
    std::vector<SwitchTrigger>     m_switches;
    i32                            m_active{-1};
    std::vector<Vertex>            m_vertices;
    std::vector<u16>               m_indices;
    std::vector<Material>          m_materials;
    std::unordered_map<u32, i32>   m_material_index; /* shad tag -> slot */
    std::unordered_map<u16, Texture> m_textures;     /* bitm tag -> gpu */
    std::vector<std::unique_ptr<LightmapImage>>  m_lightmaps;
    LightmapAtlas                                m_lightmap_atlas;
    std::shared_ptr<gfx::compat::texture_2da_t>  m_lightmap_tex;
    std::shared_ptr<gfx::sampler_t>              m_lightmap_sampler;
    std::shared_ptr<gfx::compat::texture_2da_t>  m_no_lightmaps;
    std::shared_ptr<gfx::sampler_t>              m_no_lightmaps_sampler;
    f32                            m_sky_scale{1.f};
    std::set<std::string>          m_unknown_passes;
    bool                           m_loaded{false};
    bool                           m_normal_order_logged{false};
    bool                           m_draw_error_logged{false};
};

void alloc_h2_renderer(compo::EntityContainer& e)
{
    auto& renderer = e.register_subsystem_inplace<H2MapRenderer>(std::ref(e));
    e.subsystem_cast<GameEventBus>().addEventFunction<MapLoadEvent>(
        0, [&renderer](GameEvent& ev, MapLoadEvent* load) {
            /* Consumed here: the Halo 1 loader never sees a Halo 2 map */
            if(renderer.on_map_load(*load))
                ev.type = GameEvent::None;
        });
}

} // namespace h2
