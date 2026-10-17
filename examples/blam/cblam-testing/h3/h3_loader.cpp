#include "h3_scene.h"
#include "h3_sound.h"

#include "components.h"
#include "data.h"
#include "physics.h"
#include "resource_creation.h"
#include "sounds.h"

#include <blam/elegy/bitmap.h>
#include <blam/elegy/physics.h>
#include <blam/elegy/shader.h>
#include <blam/elegy/sound.h>
#include <blam/elegy/structure.h>
#include <blam/elegy/views.h>
#include <blam/volta/blam_bitm.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include <coffee/components/proxy.h>
#include <coffee/components/subsystem.h>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_buffer.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <coffee/graphics/apis/gleam/rhi_texture.h>

#include <glm/gtc/matrix_inverse.hpp>

#if defined(FEATURE_ENABLE_BULLET3)
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#endif

#include <bit>
#include <cmath>
#include <limits>
#include <unordered_map>

void update_camera_aspect(compo::EntityContainer& e);

namespace h3 {

namespace el = blam::elegy;
using semantic::BytesConst;
using semantic::Span;

/* The structure BSP's static body in the physics world */
inline constexpr u64 collision_body_id = 0x4833'0000'0000'0000ull;

/* A placed object that moves: drawn where its Model is, simulated by physics */
static const auto object_recipe = compo::EntityRecipe{
    .components = {compo::type_hash_v<Model>(), compo::type_hash_v<ObjectPhysics>()},
    .tags       = ObjectGC,
};

/* Collision polygons fanned into triangles, decoded out of the big-endian map */
struct CollisionMesh
{
    std::vector<Vecf3> vertices;
    std::vector<i32>   indices;
};

/* The pixel formats Halo 1/2 already upload, by Halo 3 format */
static std::optional<blam::bitm::format_t> h1_format(el::bitmap_format_t format)
{
    using blam::bitm::format_t;
    using enum el::bitmap_format_t;
    switch(format)
    {
    case dxt1:
        return format_t::BC1;
    case dxt3:
        return format_t::BC2;
    case dxt5:
        return format_t::BC3;
    case a8:
        return format_t::A8;
    case y8:
        return format_t::Y8;
    case ay8:
        return format_t::AY8;
    case a8y8:
        return format_t::A8Y8;
    case r5g6b5:
        return format_t::R5G6B5;
    case a4r4g4b4:
        return format_t::ARGB4;
    case a8r8g8b8:
        return format_t::ARGB8;
    case x8r8g8b8:
        return format_t::XRGB8;
    case v8u8:
        return format_t::V8U8;
    default:
        return std::nullopt;
    }
}

/* The 360's one- and two-channel block formats: BC4/BC5 plus where the
 * channels go */
struct rgtc_t
{
    typing::pixels::PixDesc                    desc;
    std::array<gfx::textures::swizzle_t, 4>    swizzle;
};

static std::optional<rgtc_t> rgtc_format(el::bitmap_format_t format)
{
    using enum el::bitmap_format_t;
    using gfx::textures::swizzle_t;
    using typing::pixels::CompFmt;
    using typing::pixels::comp_flags;
    using typing::pixels::pix_fmt;
    auto bc4 = CompFmt(pix_fmt::BCn, comp_flags::BC4);
    auto bc5 = CompFmt(pix_fmt::BCn, comp_flags::BC5);
    constexpr auto r = swizzle_t::red, g = swizzle_t::green, one = swizzle_t::one,
                   zero = swizzle_t::zero;
    switch(format)
    {
    case dxt5a:
    case dxt5a_mono:
        return rgtc_t{bc4, {r, r, r, one}};
    case dxt5a_alpha:
        return rgtc_t{bc4, {one, one, one, r}};
    case dxn:
        return rgtc_t{bc5, {r, g, zero, one}};
    case dxn_mono_alpha:
        return rgtc_t{bc5, {r, r, r, g}};
    default:
        return std::nullopt;
    }
}

/* Parses a Halo 3 cache into H3Scene: the starting structure BSP from the
 * scenario lightmap, its instances and the scenery placed in it. Event
 * driven, no frame work */
struct H3MapLoader : compo::SubsystemBase
{
    using type = H3MapLoader;

    H3MapLoader(compo::EntityContainer& e, H3Scene& scene, H3SoundBank& bank)
        : m_container(e)
        , m_scene(scene)
        , m_bank(bank)
    {
    }

    /* True if the file is a Halo 3 cache; it is then loaded (or at least
     * attempted) here and no other loader must see it */
    bool on_map_load(MapLoadEvent const& load)
    {
        if(!load.file)
            return false;
        /* Outside the virtual filesystem (web) the Halo 2 loader fetches the
         * file and hands it back with file_data */
        if(compile_info::platform::is_emscripten && !load.file_data &&
           !platform::file::exists(*load.file).has_value())
            return false;
        auto& mapper = m_container.subsystem_cast<comp_app::FileMapper>();
        auto  file   = load.file_data ? load.file_data : mapper.fetch(*load.file).get();
        auto  bytes  = file ? static_cast<BytesConst>(*file) : BytesConst{};
        if(!is_halo3_cache(bytes))
        {
            unload();
            return false;
        }
        return load_cache(load, bytes, std::move(file));
    }

    static bool is_halo3_cache(BytesConst const& bytes)
    {
        if(!bytes.data || bytes.size < sizeof(el::file_header_t))
            return false;
        auto const* header = reinterpret_cast<el::file_header_t const*>(bytes.data);
        return header->valid() && header->version.value() == el::halo3_version;
    }

    void unload()
    {
        if(m_bank.loaded)
        {
            m_bank.loaded = false;
            m_bank.music  = {};
            m_bank.generation++;
        }
        if(m_scene.loaded)
            m_container.subsystem_cast<PostProcessParameters>().tone_curve = 0;
        m_scene.loaded = false;
        m_scene.materials.clear();
        m_scene.lightprobe.reset();
        m_scene.lightprobe_sampler.reset();
        m_scene.cubemaps.clear();
        m_scene.cubemap_probes.clear();
        m_scene.lights.clear();
        m_cube_textures.clear();
        m_scene.baked.clear();
        m_vertices.clear();
        m_indices.clear();
        m_material_index.clear();
        m_textures.clear();
        m_model_views.clear();
        m_side_maps.clear();
        m_side_files.clear();
        m_zone_masks.clear();
        m_switches.clear();
        m_zone_set = -1;
        m_loader.reset();
        m_map.reset();
        m_file.reset();
    }

  private:
    bool load_cache(
        MapLoadEvent const&                             load,
        BytesConst const&                               bytes,
        std::shared_ptr<comp_app::FileMapper::Resource> file)
    {
        unload();
        /* The map stays mapped for as long as it is loaded */
        m_file = std::move(file);
        /* Same teardown as the other loaders: the previous world goes away */
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
        ensure_gpu();
        m_scene.generation++;

        auto map = el::map_container::from_bytes(bytes);
        if(!map.has_value() || !map->has_tags())
        {
            cWarning("H3: not a loadable Halo 3 cache");
            return false;
        }
        m_map    = std::make_unique<el::map_container>(map.value());
        auto res = el::resource_loader::create(*m_map);
        if(!res)
        {
            cWarning("H3: no resource tables");
            return false;
        }
        m_loader = std::make_unique<el::resource_loader>(*res);
        if(side_pread())
        {
            open_side_caches(load, m_scene.generation, 0);
            return true;
        }
        attach_side_caches(load);
        return finish_load(load);
    }

    /* The rest of a load, once the side caches are attached */
    bool finish_load(MapLoadEvent const& load)
    {
        bool ok = load_world();
        if(ok)
            load_music();
        m_loader->trim();
        if(!ok)
        {
            cWarning("H3: nothing loaded from {}", load.file->internUrl);
            return false;
        }
        if(auto window = m_container.service<comp_app::WindowInfo>())
            window->setName(fmt::format("Blam! : {}", m_map->name()));
        m_scene.loaded = true;
        /* Linear HDR out; the composite applies Halo 3's tone curve */
        m_container.subsystem_cast<PostProcessParameters>().tone_curve = 1;
        /* The Halo 1 systems key off LoadingStatus; only the app state says
         * "loaded" (which is what the dummy plug waits for) */
        if(auto* app_info = m_container.service<comp_app::AppInfo>())
            app_info->setState(comp_app::interfaces::AppInfo::loaded);
        return true;
    }

    /* The menu's looping music: its first track's intro and every loop
     * permutation, as XMA2 for the sound system to decode */
    void load_music()
    {
        auto const& map   = *m_map;
        auto        magic = map.magic();
        auto looping = map.find<el::looping_sound_t>("main_menu_music");
        if(!looping)
            for(auto const& tag : map.tags())
                if(tag.valid() && map.is(tag, "lsnd") &&
                   map.name_of(tag).starts_with("sound\\music\\"))
                {
                    looping = map.get<el::looping_sound_t>(map.id_of(tag));
                    break;
                }
        auto gestalt = map.find<el::sound_gestalt_t>();
        if(!looping || !gestalt)
            return;
        auto tracks = looping->tracks.span(magic);
        if(tracks.empty())
            return;
        auto codecs = gestalt->codecs.span(magic);
        auto names  = gestalt->names.span(magic);

        auto permutations = [&](el::tag_reference const& ref) {
            std::vector<H3Sound> out;
            auto        sound = map.get<el::sound_t>(ref);
            auto const* codec = sound ? sound->codec.resolve(codecs) : nullptr;
            if(!codec || codec->compression != el::sound_compression_t::xma2)
                return out;
            auto view = m_loader->load(sound->resource);
            if(!view.has_value())
                return out;
            for(auto const& range : sound->pitch_ranges(*gestalt, magic))
                for(auto const& perm : el::permutations_of(range, *gestalt, magic))
                {
                    auto bytes = el::permutation_data(perm, *gestalt, magic, view.value());
                    if(bytes.empty())
                        continue;
                    auto const* name = perm.name.resolve(names);
                    out.push_back(H3Sound{
                        .name     = fmt::format(
                            "{}:{}",
                            map.name_of(*sound.tag),
                            name ? map.string(*name) : std::string_view("?")),
                        .data     = std::vector<char>(bytes.begin(), bytes.end()),
                        .rate     = codec->rate(),
                        .channels = codec->channels(),
                        .samples  = perm.sample_count,
                    });
                }
            return out;
        };

        auto const& track = tracks[0];
        auto        in    = permutations(track.in);
        m_bank.music      = H3LoopingSound{
                 .name  = std::string(map.name_of(*looping.tag)),
                 .in    = in.empty() ? H3Sound{} : std::move(in.front()),
                 .loops = permutations(track.loop),
                 .gain  = std::pow(10.f, track.gain.value() / 20.f),
        };
        m_bank.loaded     = !m_bank.music.loops.empty();
        m_bank.generation = m_scene.generation;
        cDebug(
            "H3: music {}: intro {}, {} loop permutations, gain {}",
            m_bank.music.name,
            m_bank.music.in.data.empty() ? "none" : m_bank.music.in.name,
            m_bank.music.loops.size(),
            m_bank.music.gain);
    }

    /* H3_SIDE_PREAD=1 reads the side caches by range on desktop too, the
     * path the web always takes */
    static bool side_pread()
    {
        if(compile_info::platform::is_emscripten)
            return true;
        auto v = platform::env::var("H3_SIDE_PREAD");
        return v.has_value() && v.value() == "1";
    }

    platform::url::Url side_cache_url(MapLoadEvent const& load, std::string_view name) const
    {
        return (load.file->path().dirname() / std::string(name)).url(load.file->flags);
    }

    /* Web: the side caches opened one after another through FileMapper's
     * store, read by range; the load finishes on the main queue after. A
     * newer load abandons the chain */
    void open_side_caches(MapLoadEvent const& load, u32 generation, size_t i)
    {
        using RangedFile = comp_app::FileMapper::RangedFile;
        if(generation != m_scene.generation || !m_loader)
            return;
        auto caches = m_loader->external_caches();
        if(i >= caches.size())
        {
            finish_load(load);
            return;
        }
        auto  name   = std::string(caches[i].file_name());
        auto& mapper = m_container.subsystem_cast<comp_app::FileMapper>();
        rq::runtime_queue::Queue(
            rq::dependent_task<std::shared_ptr<RangedFile>, void>::CreateSink(
                mapper.open_ranged(side_cache_url(load, name)),
                [this, load, generation, i, name](std::shared_ptr<RangedFile>* file) {
                    if(generation != m_scene.generation || !m_loader)
                        return;
                    attach_ranged(name, file ? *file : nullptr);
                    open_side_caches(load, generation, i + 1);
                }))
            .assume_value();
    }

    void attach_ranged(std::string const& name, std::shared_ptr<comp_app::FileMapper::RangedFile> file)
    {
        auto read = [file](u64 offset, Span<byte_t> dst) {
            return file->read(
                offset, gsl::span<char>(reinterpret_cast<char*>(dst.data()), dst.size()));
        };
        std::vector<byte_t> head(sizeof(el::file_header_t));
        auto source = file && file->size() && read(0, Span<byte_t>(head.data(), head.size()))
                          ? el::cache_source::from_header(
                                Span<byte_t const>(head.data(), head.size()), read)
                          : std::nullopt;
        if(!source)
        {
            cWarning("H3: {} not found, its pages are skipped", name);
            return;
        }
        m_loader->attach(name, std::move(*source));
    }

    /* Pages may live in mainmenu/shared/campaign.map next to the map */
    void attach_side_caches(MapLoadEvent const& load)
    {
        auto& mapper = m_container.subsystem_cast<comp_app::FileMapper>();
        for(auto const& cache : m_loader->external_caches())
        {
            auto name = std::string(cache.file_name());
            auto url  = (load.file->path().dirname() / name).url(load.file->flags);
            auto file = mapper.fetch(url).get();
            auto bytes = file ? static_cast<BytesConst>(*file) : BytesConst{};
            if(!bytes.data || !bytes.size)
            {
                cWarning("H3: {} not found, its pages are skipped", name);
                continue;
            }
            auto side = el::map_container::from_bytes(bytes);
            if(!side.has_value())
                continue;
            m_side_maps.push_back(std::make_unique<el::map_container>(side.value()));
            m_side_files.push_back(file);
            m_loader->attach(name, *m_side_maps.back());
        }
    }

    void ensure_gpu()
    {
        if(m_api)
            return;
        m_api     = &m_container.subsystem_cast<gfx::system>();
        auto& api = *m_api;
        m_scene.vertex_buf =
            api.alloc_buffer(gfx::buffers::vertex, semantic::RSCA::ReadOnly);
        m_scene.element_buf =
            api.alloc_buffer(gfx::buffers::element, semantic::RSCA::ReadOnly);
        m_scene.vertex_buf->alloc();
        m_scene.element_buf->alloc();
    }

    bool load_world()
    {
        auto const& map   = *m_map;
        auto        magic = map.magic();
        auto scnr = map.find<el::scenario_t>();
        if(!scnr)
            return false;

        Vecf3 eye{0.f, 0.f, 1.f};
        f32   yaw = 0.f;
        i16   bsp = 0;
        auto  starts = scnr->starting_locations.span(magic);
        if(!starts.empty())
        {
            eye = starts[0].position.value() + Vecf3{0.f, 0.f, 0.6f};
            yaw = starts[0].facing.value().x;
            bsp = std::max<i16>(starts[0].bsp.value(), 0);
        }

        /* Zone sets, and the triggers that commit them */
        m_zone_masks.clear();
        m_switches.clear();
        for(auto const& set : map.span(scnr->zone_sets))
            m_zone_masks.push_back(set.bsps);
        auto volumes = map.span(scnr->trigger_volumes);
        for(auto const& sw : map.span(scnr->zone_set_switch_triggers))
        {
            i16 const volume = sw.trigger_volume, commit = sw.commit_zone_set;
            if(commit >= 0 && static_cast<size_t>(commit) < m_zone_masks.size() && volume >= 0 &&
               static_cast<size_t>(volume) < volumes.size())
                m_switches.push_back({&volumes[static_cast<size_t>(volume)], commit, volumes[static_cast<size_t>(volume)].contains(eye)});
        }
        /* The first zone set holding the start; H3_ZONE_SET=<index> another */
        u32 bsps   = 1u << bsp;
        m_zone_set = -1;
        for(size_t i = 0; i < m_zone_masks.size(); i++)
            if(m_zone_masks[i] & bsps)
            {
                m_zone_set = static_cast<i32>(i), bsps = m_zone_masks[i];
                break;
            }
        if(auto forced = platform::env::var("H3_ZONE_SET"); forced.has_value())
            if(auto i = std::stoi(forced.value()); i >= 0 && static_cast<size_t>(i) < m_zone_masks.size())
                m_zone_set = i, bsps = m_zone_masks[static_cast<size_t>(i)];
        /* H3_BSP=<index> loads that structure BSP alone and stays there, for
         * comparing places away from the start */
        if(auto forced = platform::env::var("H3_BSP"); forced.has_value())
        {
            bsps       = 1u << std::stoi(forced.value());
            m_zone_set = -1;
            m_switches.clear();
        }
        if(!build_zone(*scnr, bsps, eye))
            return false;
        cDebug("H3: camera at ({}, {}, {}), yaw {}", eye.x, eye.y, eye.z, yaw);
        place_camera(eye, yaw);
        return true;
    }

    /* Crossing a zone set switch trigger loads that zone set's structure
     * BSPs; the camera stays where it is */
    void start_frame(compo::ContainerProxy&, time_point const&) override
    {
        if(!m_scene.loaded || m_switches.empty())
            return;
        std::optional<Vecf3> eye;
        for(auto entity : m_container.select<PlayerCamera, PlayerInfo>())
        {
            auto const& [camera, info] = entity.components();
            if(info.seat_idx == 0 && !info.is_remote())
                eye = camera.camera.position;
        }
        if(!eye)
            return;
        /* On entering, as the game commits them: overlapping triggers of two
         * zone sets would otherwise trade places every frame */
        for(auto& sw : m_switches)
        {
            bool const entered = sw.volume->contains(*eye) && !sw.inside;
            sw.inside          = sw.volume->contains(*eye);
            if(entered && sw.zone_set != m_zone_set)
            {
                switch_zone(sw.zone_set, *eye);
                return;
            }
        }
    }

    bool main_thread_only() const override
    {
        return true;
    }

    void switch_zone(i32 zone_set, Vecf3 const& eye)
    {
        auto const scnr = m_map->find<el::scenario_t>();
        auto const sets = scnr ? m_map->span(scnr->zone_sets) : Span<el::zone_set_t const>();
        if(zone_set < 0 || static_cast<size_t>(zone_set) >= sets.size())
            return;
        cDebug("H3: zone set {} '{}': bsps {:#x}", zone_set,
               m_map->string(sets[static_cast<size_t>(zone_set)].name), m_zone_masks[static_cast<size_t>(zone_set)]);
        m_zone_set = zone_set;
        /* The world placed in the old BSPs goes; textures, cubes and model
         * geometry views are kept */
        m_container.remove_entity_if([](compo::Entity const& en) {
            return stl_types::any_flag_of(en.tags, ObjectGC);
        });
        {
            Physics::Event reset_ev{Physics::Event::Reset};
            Physics::Reset reset{};
            m_container.subsystem_cast<PhysicsBus>().process(reset_ev, &reset);
        }
        m_scene.generation++;
        m_scene.materials.clear();
        m_material_index.clear();
        m_scene.cubemaps.clear();
        m_scene.cubemap_probes.clear();
        m_scene.lights.clear();
        m_vertices.clear();
        m_indices.clear();
        build_zone(*scnr, m_zone_masks[static_cast<size_t>(zone_set)], eye);
        m_loader->trim();
    }

    /* Everything of the structure BSPs in `bsps`: their geometry, baked
     * light, collision and cubemaps, and the objects, lights and sky placed
     * in them */
    bool build_zone(el::scenario_t const& scnr, u32 bsps, Vecf3 const& eye)
    {
        auto const& map = *m_map;
        /* Fallback slot for parts whose render method is not usable */
        m_scene.materials.push_back(Material{.name = "(fallback)"});
        /* One lightprobe array for all of them: the largest lightmap's size,
         * smaller ones in its corner */
        m_probe_extent = Vecui2(0);
        m_probe_layers.clear();
        m_scene.baked.clear();
        m_scene.lightprobe.reset();
        m_scene.lightprobe_sampler.reset();
        for(i16 b = 0; b < 32; b++)
            if(bsps & (1u << b))
                if(auto const* lit = el::lightmap_of(map, b))
                    m_probe_extent = glm::max(m_probe_extent, lightprobe_extent(*lit));
        m_seam_users.clear();
        for(i16 b = 0; b < 32; b++)
        {
            auto bsp = bsps & (1u << b) ? el::structure_bsp(map, scnr, b) : el::tagged<el::structure_bsp_t>{};
            if(!bsp)
                continue;
            std::vector<u8> seams;
            for(auto const& material : bsp->collision_materials.span(map.magic()))
                if(material.seam_sealer())
                {
                    auto const seam = static_cast<size_t>(material.seam_mapping.value());
                    seams.resize(std::max(seams.size(), seam + 1));
                    seams[seam] = 1;
                }
            m_seam_users.resize(std::max(m_seam_users.size(), seams.size()));
            for(size_t i = 0; i < seams.size(); i++)
                m_seam_users[i] += seams[i];
        }
        u32 slot = 0;
        i16 first = -1, holding = -1;
        for(i16 b = 0; b < 32 && slot < max_bsp_slots; b++)
        {
            if(!(bsps & (1u << b)))
                continue;
            if(load_bsp(b, slot))
            {
                first = first < 0 ? b : first;
                if(holding < 0 && bsp_holds(scnr, b, eye))
                    holding = b;
            }
            slot++;
            /* One BSP's pages at a time, or a big zone set outgrows the web heap */
            m_loader->trim();
        }
        if(first < 0)
            return false;
        /* Exposure from the one the camera is in */
        i16 const here = holding >= 0 ? holding : first;
        upload_lightprobes();
        m_scene.exposure = exposure_at(scnr, here, eye);
        load_atmosphere();
        /* H3_EXPOSURE=<stops> overrides it, for comparing with captures */
        if(auto forced = platform::env::var("H3_EXPOSURE"); forced.has_value())
        {
            m_scene.exposure              = std::exp2(std::stof(forced.value()));
            m_scene.auto_exposure.enabled = false;
        }
        load_objects(scnr, bsps);
        load_sky(scnr, bsps);
        for(i16 b = 0; b < 32; b++)
            if(bsps & (1u << b))
                load_cubemaps(scnr, b);
        if(m_vertices.empty())
            return false;

        m_scene.vertex_buf->commit(
            Span<const Vertex>(m_vertices.data(), m_vertices.size()));
        m_scene.element_buf->commit(Span<const u16>(m_indices.data(), m_indices.size()));
        size_t draws = 0;
        for(auto const& mat : m_scene.materials)
            draws += mat.draws.size();
        cDebug(
            "H3: {}: bsps {:#x}, {} vertices, {} indices, {} materials, {} draws, {} "
            "textures",
            map.name(),
            bsps,
            m_vertices.size(),
            m_indices.size(),
            m_scene.materials.size(),
            draws,
            m_textures.size());
        m_vertices = {};
        m_indices  = {};
        /* Model geometry is decoded; its pages need not stay */
        m_model_views.clear();
        return true;
    }

    /* Whether `point` is in one of the structure BSP's clusters */
    bool bsp_holds(el::scenario_t const& scnr, i16 bsp_index, Vecf3 const& point)
    {
        auto const bsp = el::structure_bsp(*m_map, scnr, bsp_index);
        if(!bsp)
            return false;
        auto resources = m_loader->load(bsp->tag_resources);
        return resources.has_value() && el::cluster_at(*m_map, *bsp, resources.value(), point);
    }

    /* The structure BSP's render geometry lives in the scenario lightmap:
     * clusters in world space, instances placed from their definitions */
    bool load_bsp(i16 bsp_index, u32 slot)
    {
        auto const& map   = *m_map;
        auto        magic = map.magic();
        auto scnr = map.find<el::scenario_t>();
        auto bsp  = scnr ? el::structure_bsp(map, *scnr, bsp_index) : el::tagged<el::structure_bsp_t>{};
        if(!bsp)
            return false;
        auto const* lit = el::lightmap_of(map, bsp_index);
        if(!lit)
        {
            cWarning("H3: no lightmap geometry for bsp {}", bsp_index);
            return false;
        }

        auto view = m_loader->load(lit->geometry.resource);
        if(!view.has_value())
        {
            cWarning("H3: bsp geometry resource failed: {}", static_cast<int>(view.error()));
            return false;
        }
        auto const& geometry    = lit->geometry;
        auto        meshes      = geometry.meshes.span(magic);
        auto        compression = geometry.compression.span(magic);
        auto materials = bsp->materials.span(magic);

        m_slot = slot;
        bool const have_lightprobes = load_lightprobes(*lit, slot);
        auto lit_clusters = lit->clusters.span(magic);
        auto lit_instances = lit->instances.span(magic);
        auto instance_texcoords = geometry.per_instance_lightmap_texcoords.span(magic);
        auto per_vertex = lit->per_vertex.span(magic);

        u32  clusters = 0, instances = 0;
        auto cluster_list = bsp->clusters.span(magic);
        for(size_t i = 0; i < cluster_list.size(); i++)
        {
            auto const* mesh = cluster_list[i].mesh.resolve(meshes);
            if(!mesh)
                continue;
            std::vector<Vecf2> uvs;
            if(have_lightprobes && i < lit_clusters.size() &&
               lit_clusters[i].texture_array_index.value() >= 0)
                uvs = el::lightmap_texcoords(view.value(), mesh->vertex_buffers[1]);
            clusters +=
                append_mesh(view.value(), *mesh, nullptr, Matf4(1.f), materials, uvs) > 0;
        }

        auto resources = m_loader->load(bsp->tag_resources);
        Span<el::instanced_geometry_definition_t const> definitions;
        Span<el::coll::collision_bsp_t const>           collision_bsps;
        if(resources.has_value())
            if(auto const* root = resources->root_as<el::structure_bsp_resources_t>())
            {
                definitions    = resources->block(root->instanced_geometry);
                collision_bsps = resources->block(root->collision_bsps);
            }
        /* Seams to another loaded BSP are walked through */
        auto              collision_materials = bsp->collision_materials.span(magic);
        std::vector<bool> open_seams(collision_materials.size());
        for(size_t i = 0; i < collision_materials.size(); i++)
            if(auto const& material = collision_materials[i]; material.seam_sealer())
                open_seams[i] = static_cast<size_t>(material.seam_mapping.value()) < m_seam_users.size() &&
                                m_seam_users[static_cast<size_t>(material.seam_mapping.value())] > 1;
        auto collision = std::make_shared<CollisionMesh>();
        if(!collision_bsps.empty())
            append_collision(
                el::coll::bsp_view_t::from(resources.value(), collision_bsps[0]),
                Matf4(1.f),
                *collision,
                open_seams);
        size_t const world_triangles = collision->indices.size() / 3;
        auto instance_list = bsp->instances.span(magic);
        for(size_t i = 0; i < instance_list.size(); i++)
        {
            auto const& instance   = instance_list[i];
            auto const* definition = instance.definition.resolve(definitions);
            if(!definition)
                continue;
            auto const* mesh = definition->mesh.resolve(meshes);
            if(!mesh)
                continue;
            Matf4 const transform = instance.transform();
            append_collision(
                el::coll::bsp_view_t::from(resources.value(), definition->collision),
                transform,
                *collision);
            /* An instance's texcoords are its own buffer, through the
             * lightmap's per-instance list */
            std::vector<Vecf2>              uvs;
            std::vector<el::vertex_light_t> lights;
            i16 texcoords = instance.lightmap_texcoords;
            if(have_lightprobes && i < lit_instances.size() &&
               lit_instances[i].target.texture_array_index.value() >= 0 && texcoords >= 0 &&
               static_cast<size_t>(texcoords) < instance_texcoords.size())
                uvs = el::lightmap_texcoords(
                    view.value(), instance_texcoords[static_cast<size_t>(texcoords)].vertex_buffer);
            else if(i < lit_instances.size() && lit_instances[i].target.per_vertex.value() >= 0 &&
                    static_cast<size_t>(lit_instances[i].target.per_vertex.value()) < per_vertex.size())
                lights = el::vertex_lights(
                    view.value(),
                    per_vertex[static_cast<size_t>(lit_instances[i].target.per_vertex.value())]
                        .vertex_buffer);
            instances += append_mesh(
                             view.value(),
                             *mesh,
                             definition->compression.resolve(compression),
                             transform,
                             materials,
                             uvs,
                             lights) > 0;
        }
        cDebug(
            "H3: {}: {} clusters, {} instances; collision {} world + {} instance triangles, {} seams open",
            map.name_of(*bsp.tag),
            clusters,
            instances,
            world_triangles,
            collision->indices.size() / 3 - world_triangles,
            std::ranges::count(open_seams, true));
        submit_collision(std::move(collision), collision_body_id + static_cast<u64>(bsp_index));
        return clusters + instances > 0;
    }

    /* Appends a collision BSP's surfaces as fanned triangles */
    static void append_collision(
        el::coll::bsp_view_t const& bsp,
        Matf4 const&                transform,
        CollisionMesh&              out,
        std::vector<bool> const&    skipped_materials = {})
    {
        i32 const vbase = static_cast<i32>(out.vertices.size());
        /* Bounding surfaces sit on vertices at infinity, which a quantized
         * BVH cannot hold */
        std::vector<bool> finite;
        finite.reserve(bsp.vertices.size());
        for(auto const& v : bsp.vertices)
        {
            Vecf3 const point = v.point.value();
            finite.push_back(std::isfinite(point.x) && std::isfinite(point.y) && std::isfinite(point.z));
            out.vertices.push_back(finite.back() ? Vecf3(transform * Vecf4(point, 1.f)) : Vecf3{});
        }
        std::vector<i32> polygon;
        for(u32 s = 0; s < bsp.surfaces.size(); s++)
        {
            if(stl_types::any_flag_of(bsp.surfaces[s].flags, el::coll::surface_flags_t::invalid))
                continue;
            if(auto material = bsp.surfaces[s].material.value();
               material >= 0 && static_cast<size_t>(material) < skipped_materials.size() &&
               skipped_materials[static_cast<size_t>(material)])
                continue;
            polygon.clear();
            bool usable = true;
            bsp.walk_surface(s, [&](u32 vertex) {
                if(vertex < finite.size() && finite[vertex])
                    polygon.push_back(vbase + static_cast<i32>(vertex));
                else
                    usable = false;
            });
            if(!usable)
                continue;
            for(size_t i = 2; i < polygon.size(); i++)
                out.indices.insert(out.indices.end(), {polygon[0], polygon[i - 1], polygon[i]});
        }
    }

    /* One static body for the loaded structure BSP */
    void submit_collision(std::shared_ptr<CollisionMesh> mesh, u64 id)
    {
#if defined(FEATURE_ENABLE_BULLET3)
        if(!mesh || mesh->indices.empty())
            return;
        auto body        = std::make_shared<Physics::BodyCreationPrebuilt>();
        body->entity_id  = id;
        body->mesh_iface = std::make_unique<btTriangleIndexVertexArray>();
        btIndexedMesh part;
        part.m_numTriangles        = static_cast<int>(mesh->indices.size() / 3);
        part.m_triangleIndexBase   = reinterpret_cast<unsigned char const*>(mesh->indices.data());
        part.m_triangleIndexStride = 3 * sizeof(i32);
        part.m_numVertices         = static_cast<int>(mesh->vertices.size());
        part.m_vertexBase   = reinterpret_cast<unsigned char const*>(mesh->vertices.data());
        part.m_vertexStride = sizeof(Vecf3);
        part.m_vertexType   = PHY_FLOAT;
        body->mesh_iface->addIndexedMesh(part, PHY_INTEGER);
        body->shape      = std::make_unique<btBvhTriangleMeshShape>(body->mesh_iface.get(), true);
        body->keep_alive = std::move(mesh);
        Physics::Event event{.type = Physics::Event::BodyCreationPrebuilt};
        m_container.subsystem_cast<PhysicsBus>().process(event, body.get());
#else
        (void)mesh;
#endif
    }

    /* Scenery, bipeds, vehicles, crates, weapons and equipment placed in this BSP. Those whose physics
     * model moves become entities that physics carries; the rest are drawn
     * in place. */
    void load_objects(el::scenario_t const& scnr, u32 bsps)
    {
        u32  moving = 0, fixed = 0, hidden = 0;
        auto place  = [&](auto const& placements, auto const& palette, bool item) {
            for(auto const& [placement, entry] :
                el::placements_at_load(*m_map, placements, palette, bsps))
            {
                auto const& object = placement.object;
                auto const  model  = entry ? drawn_model(entry->object) : el::tagged<el::render_model_t>{};
                if(!model)
                {
                    hidden++;
                    continue;
                }
                f32 const   scale    = object.scale_or_one();
                Quatf const q        = object.orientation();
                Vecf3 const position = object.position;
                auto const  light    = m_scene.light_below(position);
                auto const* lit      = light ? &*light : nullptr;
                auto        physics  = physics_of(entry->object, scale);
                if(!physics)
                {
                    auto const skin = stored_pose(*model, object);
                    append_model(model, object.transform(), lit, 0, Quatf(1.f, 0.f, 0.f, 0.f), skin);
                    attach_lights(entry->object, *model, object.transform(), 0);
                    fixed++;
                    continue;
                }
                physics->item     = item;
                physics->sleeping = enum_helpers::feval(
                    object.placement_flags.value(), el::placement_flags_t::create_at_rest);
                auto  ref     = m_container.create_entity(object_recipe);
                auto& placed  = *m_container.get<Model>(ref.id());
                placed.position      = position;
                placed.rotation      = q;
                placed.tick_position = position;
                placed.tick_rotation = q;
                placed.update_matrix();
                *m_container.get<ObjectPhysics>(ref.id()) = std::move(*physics);
                append_model(model, glm::scale(Matf4(1.f), Vecf3(scale)), lit, ref.id(), q);
                attach_lights(entry->object, *model, glm::scale(Matf4(1.f), Vecf3(scale)), ref.id());
                moving++;
            }
        };
        place(scnr.scenery, scnr.scenery_palette, false);
        /* Bipeds stand in their bind pose; their ragdolls are not bodies */
        place(scnr.bipeds, scnr.biped_palette, false);
        /* Vehicles are drawn where they are parked; their bodies span nodes */
        place(scnr.vehicles, scnr.vehicle_palette, false);
        place(scnr.crates, scnr.crate_palette, false);
        place(scnr.weapons, scnr.weapon_palette, true);
        place(scnr.equipment, scnr.equipment_palette, true);
        cDebug(
            "H3: objects: {} moving, {} fixed in place, {} editor-only or modelless; {} lights",
            moving,
            fixed,
            hidden,
            m_scene.lights.size());
    }

    /* The lights an object carries at its markers (ligh attachments), held
     * still: colour and intensity at their steady value. Falloff constants
     * put the light's edge at its maximum distance. */
    void attach_lights(
        el::tag_reference const& object, el::render_model_t const& model, Matf4 const& frame, u64 entity)
    {
        auto const& map  = *m_map;
        auto const  view = el::object_view::of(map, object);
        if(!view.object)
            return;
        std::vector<Matf4> nodes;
        for(auto const& attachment : map.span(view.object->attachments))
        {
            auto const light = map.get<el::light_t>(attachment.type);
            if(!light)
                continue;
            if(nodes.empty())
                nodes = el::node_matrices(map, model, el::default_pose(map, model));
            Matf4 const at = frame * el::marker_frame(map, model, attachment.marker, nodes).value_or(Matf4(1.f));
            f32 const   radius = light->maximum_distance;
            if(radius <= 0.f)
                continue;
            f32 const diffusion = std::max(light->distance_diffusion.value(), .01f);
            f32 const size      = diffusion * diffusion;
            f32 const at_light  = 1.f / size, at_edge = 1.f / (size + radius * radius);
            f32 const scale     = 1.f / (at_light - at_edge);
            DynamicLight out{
                .entity    = entity,
                .position  = Vecf3(at[3]),
                .direction = glm::normalize(Vecf3(at[0])),
                .color     = el::function_color(light->color.function.data(map.magic())) *
                         el::function_scalar(light->intensity.function.data(map.magic())),
                .size      = size,
                .radius    = radius,
            };
            out.falloff = Vecf4(scale, 0.f, -at_edge * scale, 1.f);
            if(light->type.value() == el::light_type_t::frustum)
            {
                f32 const edge = std::cos(light->frustum_field_of_view * .5f);
                out.falloff.y  = 1.f / std::max(1.f - edge, 1e-3f);
                out.falloff.w  = -edge * out.falloff.y;
                out.sphere     = light->percent_spherical;
                out.smooth     = std::max(light->angular_smoothness.value(), 1.f);
            }
            m_scene.lights.push_back(out);
        }
    }

    /* Skinning matrices for a placement posed by stored node orientations
     * (corpses), none when it stands in the default pose */
    std::vector<Matf4> stored_pose(el::render_model_t const& model, el::object_placement_t const& object) const
    {
        auto const stored = m_map->span(object.node_orientations);
        if(stored.empty() ||
           !enum_helpers::feval(object.placement_flags.value(), el::placement_flags_t::store_orientations))
            return {};
        auto pose = el::default_pose(*m_map, model);
        el::apply_orientations(*m_map, stored[0], pose);
        return el::skinning_matrices(*m_map, model, pose);
    }

    /* An object's physics model as one body (el::phys::body_of), scaled; flat
     * hulls get their shell's thickness so they have something to rest on */
    std::optional<ObjectPhysics> physics_of(el::tag_reference const& object, f32 scale) const
    {
        auto const physics = el::object_view::of(*m_map, object).physics_model;
        auto const body    = physics ? el::phys::body_of(*physics, m_map->magic()) : std::nullopt;
        if(!body)
            return std::nullopt;
        auto geometry = std::make_shared<CollisionGeometry>();
        for(auto const& hull : body->hulls)
        {
            auto& node = geometry->nodes.emplace_back();
            Vecf3 lo(std::numeric_limits<f32>::max()), hi(std::numeric_limits<f32>::lowest());
            for(auto const& point : hull.points)
            {
                node.points.push_back(point * scale);
                lo = glm::min(lo, node.points.back());
                hi = glm::max(hi, node.points.back());
            }
            /* Rifles are quads */
            f32 const shell = std::max(hull.radius * scale, .01f);
            for(glm::length_t axis = 0; axis < 3; axis++)
            {
                if(hi[axis] - lo[axis] >= shell)
                    continue;
                size_t const count = node.points.size();
                for(size_t i = 0; i < count; i++)
                {
                    Vecf3 above = node.points[i], below = node.points[i];
                    above[axis] += shell;
                    below[axis] -= shell;
                    node.points[i] = below;
                    node.points.push_back(above);
                }
            }
        }
        return ObjectPhysics{
            .collision      = std::move(geometry),
            .center_of_mass = body->center_of_mass * scale,
            .mass           = body->mass,
            .friction       = body->friction,
            .restitution    = body->restitution,
        };
    }

    /* A render model's first permutation of every region, moved by
     * `transform`; geometry resources are loaded once per model */
    u32 append_model(
        el::tagged<el::render_model_t> const& model,
        Matf4 const&                          transform,
        el::vertex_light_t const*             light,
        u64                                   entity         = 0,
        Quatf const&                          light_rotation = Quatf(1.f, 0.f, 0.f, 0.f),
        Span<Matf4 const>                     skin           = {})
    {
        auto const& map = *m_map;
        if(!model)
            return 0;
        u32  key = map.id_of(*model.tag).raw.value();
        auto it  = m_model_views.find(key);
        if(it == m_model_views.end())
        {
            auto view = m_loader->load(model->geometry.resource);
            it        = m_model_views
                     .emplace(key, view.has_value() ? std::optional(view.value()) : std::nullopt)
                     .first;
        }
        if(!it->second)
            return 0;
        auto compression = map.span(model->geometry.compression);
        auto materials   = map.span(model->materials);
        u32  emitted     = 0;
        for(auto const* mesh : el::default_meshes(map, *model))
            emitted += append_mesh(
                *it->second,
                *mesh,
                compression.empty() ? nullptr : &compression[0],
                transform,
                materials,
                {},
                {},
                light,
                entity,
                light_rotation,
                skin);
        return emitted;
    }

    /* The skies active in this BSP: scenery modelled in world space around
     * the level, so placed at the origin */
    void load_sky(el::scenario_t const& scnr, u32 bsps)
    {
        auto const& map   = *m_map;
        auto        magic = map.magic();
        u32         drawn = 0;
        for(auto const& sky : scnr.skies.span(magic))
        {
            u16 active = sky.active_bsps;
            if(active && !(active & bsps))
                continue;
            drawn += append_model(drawn_model(sky.object), Matf4(1.f), nullptr);
        }
        cDebug("H3: sky: {} meshes", drawn);
    }

    /* What draws an object in a game; nothing for editor-only ones */
    el::tagged<el::render_model_t> drawn_model(el::tag_reference const& object) const
    {
        auto const view = el::object_view::of(*m_map, object);
        return view.editor_only(*m_map) ? el::tagged<el::render_model_t>{} : view.render_model;
    }

    /* A mesh's triangles, one draw per part, vertices moved by `transform` */
    u32 append_mesh(
        el::resource_view const&              view,
        el::mesh_t const&                     mesh,
        el::compression_t const*              compression,
        Matf4 const&                          transform,
        Span<el::geometry_material_t const>   materials,
        std::vector<Vecf2> const&             lightmap_uvs = {},
        std::vector<el::vertex_light_t> const& vertex_lights = {},
        el::vertex_light_t const*              object_light  = nullptr,
        u64                                    entity        = 0,
        Quatf const&                           light_rotation = Quatf(1.f, 0.f, 0.f, 0.f),
        Span<Matf4 const>                      skin           = {})
    {
        auto magic   = m_map->magic();
        auto buffers = el::mesh_buffers::from(view, mesh);
        m_decoded.clear();
        if(!buffers.decode(compression, m_decoded) || m_decoded.empty() ||
           m_decoded.size() > std::numeric_limits<u16>::max())
            return 0;
        u32  vbase  = static_cast<u32>(m_vertices.size());
        auto normal = glm::mat3(glm::inverseTranspose(transform));
        bool const lit        = lightmap_uvs.size() == m_decoded.size();
        bool const vertex_lit = vertex_lights.size() == m_decoded.size() || object_light;
        if(!skin.empty())
            pose_vertices(mesh, skin);
        for(size_t i = 0; i < m_decoded.size(); i++)
        {
            auto const& v = m_decoded[i];
            Vertex      out{
                     .position = Vecf3(transform * Vecf4(v.position, 1.f)),
                     .texcoord = v.texcoord,
                     .normal   = glm::normalize(normal * v.normal),
                     .lightmap = lit ? Vecf3(lightmap_uvs[i] * m_slot_uv, static_cast<f32>(m_slot))
                                     : Vecf3(0.f, 0.f, -1.f),
            };
            if(vertex_lit)
            {
                /* Square root of a quarter: 8 bits reach 4x the white point
                 * and keep shade from rounding to black */
                Vecf3 light = glm::sqrt(glm::clamp(
                                  h3::diffuse_radiance(
                                      object_light ? *object_light : vertex_lights[i],
                                      light_rotation * out.normal) *
                                      0.25f,
                                  Vecf3(0.f),
                                  Vecf3(1.f))) *
                              255.f + .5f;
                out.light[0] = static_cast<u8>(light.x);
                out.light[1] = static_cast<u8>(light.y);
                out.light[2] = static_cast<u8>(light.z);
                out.light[3] = 255;
            }
            m_vertices.push_back(out);
        }

        auto parts = mesh.parts.span(magic);
        u32  emitted = 0;
        /* Lightmapped triangles, for lighting objects placed on them */
        auto keep_lit = [&](std::vector<std::array<u16, 3>> const& triangles) {
            if(!lit)
                return;
            for(auto const& t : triangles)
            {
                BakedLight::Triangle out;
                for(int c = 0; c < 3; c++)
                {
                    out.position[c] = m_vertices[vbase + t[c]].position;
                    out.uv[c]       = lightmap_uvs[t[c]];
                }
                m_scene.baked[m_slot].triangles.push_back(out);
            }
        };
        auto emit = [&](i16 material, u32 start, u32 count) {
            m_triangles.clear();
            buffers.triangles(mesh.index_type, start, count, m_triangles);
            if(m_triangles.empty())
                return;
            auto const* method = material >= 0 && static_cast<size_t>(material) < materials.size()
                                     ? &materials[material].render_method
                                     : nullptr;
            auto& mat   = m_scene.materials[material_for(method)];
            auto* draws = &mat.draws;
            if(entity)
            {
                if(mat.object_draws.empty() || mat.object_draws.back().entity != entity)
                    mat.object_draws.push_back({.entity = entity});
                draws = &mat.object_draws.back().draws;
            }
            draws->push_back({
                .elements =
                    {
                        .count         = static_cast<u32>(m_triangles.size() * 3),
                        .offset        = m_indices.size() * sizeof(u16),
                        .vertex_offset = vbase,
                        .type          = semantic::type_t::u16,
                    },
                .instances = {.count = 1, .offset = 0},
            });
            for(auto const& t : m_triangles)
                m_indices.insert(m_indices.end(), t.begin(), t.end());
            keep_lit(m_triangles);
            emitted++;
        };
        if(parts.empty())
            emit(-1, 0, buffers.index_count());
        for(auto const& part : parts)
            if(part.drawn())
                emit(part.material, part.index_start, part.index_count);
        return emitted;
    }

    /* Decoded vertices moved from the default pose by per-node matrices: a
     * rigid mesh by its node's, skinned vertices by their weighted sum */
    void pose_vertices(el::mesh_t const& mesh, Span<Matf4 const> skin)
    {
        using el::vertex_type_t;
        auto const type  = mesh.vertex_type;
        bool const rigid = type == vertex_type_t::rigid || type == vertex_type_t::flat_rigid;
        bool const skinned = type == vertex_type_t::skinned || type == vertex_type_t::flat_skinned;
        if(rigid && (mesh.rigid_node < 0 || static_cast<size_t>(mesh.rigid_node) >= skin.size()))
            return;
        for(auto& v : m_decoded)
        {
            Matf4 m(1.f);
            if(rigid)
                m = skin[static_cast<size_t>(mesh.rigid_node)];
            else if(skinned)
            {
                m = Matf4(0.f);
                for(int k = 0; k < 4; k++)
                    if(v.weights[k] > 0.f && v.nodes[k] < skin.size())
                        m += skin[v.nodes[k]] * v.weights[k];
            } else
                continue;
            v.position = Vecf3(m * Vecf4(v.position, 1.f));
            v.normal   = glm::normalize(glm::mat3(m) * v.normal);
        }
    }

    /* Material slot for a render method, built on first use */
    size_t material_for(el::tag_reference const* method_ref)
    {
        auto const& map  = *m_map;
        auto const  view = method_ref ? el::render_method_view::of(map, *method_ref)
                                      : el::render_method_view{};
        if(!view)
            return 0;
        u32 key = method_ref->id.raw.value();
        if(auto it = m_material_index.find(key); it != m_material_index.end())
            return it->second;

        auto const* tag = view.method.tag;
        Material    mat{.name = std::string(map.name_of(*tag))};
        m_pending_environment_map = nullptr;
        struct Slot
        {
            Texture texture;
            Vecf2   scale;
            f32     curve;
        };
        std::unordered_map<std::string, Slot>  slots;
        std::unordered_map<std::string, Vecf4> constants;
        std::unordered_map<std::string, TextureTag> texture_tags;
        for(auto const& [name, value] : view.named_constants())
        {
            if(name == "albedo_color")
                mat.tint = value;
            constants.emplace(std::string(name), value.value());
        }
        /* Every named texture slot, with its uv scale from the float
         * constant its transform names */
        for(auto const& [name, texture] : view.named_textures())
        {
            auto const bitmap = map.get<el::bitmap_t>(texture.bitmap);
            if(!bitmap)
                continue;
            if(name == "environment_map")
                m_pending_environment_map = bitmap.tag;
            Vecf4 const transform = view.transform_of(texture);
            texture_tags.emplace(std::string(name), TextureTag{bitmap.tag, transform});
            if(auto tex = texture_for(*bitmap.tag))
                slots.emplace(std::string(name), Slot{*tex, Vecf2(transform), curve_of(*bitmap.tag)});
        }
        auto options = option_flags(view, mat);
        auto option  = [&](std::string const& category) -> std::string_view {
            auto it = options.find(category);
            return it == options.end() ? std::string_view() : std::string_view(it->second);
        };
        auto slot = [&](std::string const& name) -> Slot const* {
            auto it = slots.find(name);
            return it == slots.end() ? nullptr : &it->second;
        };

        if(map.is(*tag, "rmw "))
        {
            load_water(mat, constants, texture_tags, option("foam"));
        } else if(map.is(*tag, "rmtr"))
        {
            /* Terrain: up to four layers weighted by the blend map */
            mat.variant = Variant::terrain;
            if(auto const* blend = slot("blend_map"))
            {
                mat.blend       = blend->texture;
                mat.blend_scale = blend->scale;
            }
            bool any = false;
            for(u32 k = 0; k < mat.layers.size(); k++)
            {
                auto& layer    = mat.layers[k];
                auto  material = option(fmt::format("material_{}", k));
                auto const* base   = slot(fmt::format("base_map_m_{}", k));
                auto const* detail = slot(fmt::format("detail_map_m_{}", k));
                layer.enabled = base && material != "off" && !material.empty();
                if(!layer.enabled)
                    continue;
                any              = true;
                layer.base       = base->texture;
                layer.base_curve = base->curve;
                layer.scale      = Vecf4(base->scale, 1.f, 1.f);
                /* "four_material_shaders_disable_detail_bump" drops the detail
                 * bump only; the detail albedo always applies */
                layer.detailed = detail != nullptr;
                if(detail)
                {
                    layer.detail       = detail->texture;
                    layer.detail_curve = detail->curve;
                    layer.scale        = Vecf4(base->scale, detail->scale);
                }
            }
            /* Without a blend map only the first layer shows */
            mat.visible = any && (mat.blend.sampler || mat.layers[0].enabled);
        } else
        {
            if(auto const* base = slot("base_map"))
            {
                mat.diffuse  = base->texture;
                mat.uv_scale = base->scale;
                mat.curves.x = base->curve;
            }
            auto const* detail  = slot("detail_map");
            auto const* detail2 = slot("detail_map2");
            auto        albedo  = option("albedo");
            if(detail)
            {
                mat.detail       = detail->texture;
                mat.detail_scale = detail->scale;
                mat.curves.y     = detail->curve;
                mat.variant      = Variant::detail;
            }
            mat.black_point = albedo == "two_detail_black_point";
            auto const* detail3 = slot(albedo == "two_detail_overlay" ? "detail_map_overlay" : "detail_map3");
            auto const* base    = slot("base_map");
            if(detail && detail2 && detail3 && base && base->scale.x != 0.f && base->scale.y != 0.f &&
               (albedo == "two_detail_overlay" || albedo == "three_detail_blend"))
            {
                mat.detail        = detail->texture;
                mat.detail_scale  = detail->scale;
                mat.curves.y      = detail->curve;
                mat.detail2       = detail2->texture;
                mat.detail2_scale = detail2->scale;
                mat.curves.z      = detail2->curve;
                mat.detail3       = detail3->texture;
                mat.detail3_ratio = detail3->scale / base->scale;
                mat.detail3_curve = detail3->curve;
                mat.variant = albedo == "two_detail_overlay" ? Variant::detail_overlay
                                                             : Variant::three_detail;
            } else if(detail && detail2 &&
               (albedo == "two_detail" || albedo == "detail_blend" || mat.black_point))
            {
                mat.detail2       = detail2->texture;
                mat.detail2_scale = detail2->scale;
                mat.curves.z      = detail2->curve;
                mat.variant = albedo == "detail_blend" ? Variant::detail_blend : Variant::two_detail;
            }
            if(albedo == "constant_color")
                mat.variant = Variant::base;
            load_specular(mat, option("material_model"), option("specular_mask"), constants);
            /* Halograms hardcode material_model none */
            if(map.is(*tag, "rmhg"))
                mat.model = MaterialModel::none;
            load_environment(mat, option("environment_mapping"), constants);
            load_self_illum(mat, option("self_illumination"), slot("self_illum_map"),
                            slot("self_illum_detail_map"), constants);
            if(auto const* test = slot("alpha_test_map"); test && mat.alpha_test)
            {
                mat.alpha_test_map   = test->texture;
                mat.alpha_test_ratio = mat.uv_scale.x != 0.f && mat.uv_scale.y != 0.f
                                           ? test->scale / mat.uv_scale
                                           : test->scale;
            }
            if(auto const* mask = slot("specular_mask_texture"))
                mat.specular_mask_map = mask->texture;
            if(auto const* material_map = slot("material_texture"))
                mat.material_map = material_map->texture;
            auto bump = option("bump_mapping");
            if(auto const* map = slot("bump_map"); map && (bump == "standard" || bump == "detail"))
            {
                mat.bump       = map->texture;
                mat.bump_scale = map->scale;
                mat.bump_mode  = 1;
                auto const* detail_bump = slot("bump_detail_map");
                if(detail_bump && bump == "detail" && map->scale.x != 0.f && map->scale.y != 0.f)
                {
                    mat.bump_detail       = detail_bump->texture;
                    mat.bump_detail_ratio = detail_bump->scale / map->scale;
                    mat.bump_mode         = 2;
                }
            }
            /* Helpers like z_invis_lightquad carry only an alpha test map */
            mat.visible = mat.diffuse.sampler || mat.constant_color;
        }
        /* H3_HIDE=<name part> leaves matching materials out, for finding
         * which one draws what */
        if(auto hide = platform::env::var("H3_HIDE");
           hide.has_value() && !hide->empty() && mat.name.find(hide.value()) != std::string::npos)
            mat.visible = false;
        m_scene.materials.push_back(std::move(mat));
        size_t idx = m_scene.materials.size() - 1;
        m_material_index.emplace(key, idx);
        return idx;
    }

    /* A texture parameter's bitmap and its uv scale (xy) and offset (zw) */
    struct TextureTag
    {
        el::tag_t const* bitmap;
        Vecf4            xform;
    };

    /* shader_water: colour, static reflection cube, wave slopes, foam */
    void load_water(
        Material&                                          mat,
        std::unordered_map<std::string, Vecf4> const&      constants,
        std::unordered_map<std::string, TextureTag> const& textures,
        std::string_view                                   foam)
    {
        auto scalar = [&](char const* name, f32 fallback) {
            auto it = constants.find(name);
            return it == constants.end() ? fallback : it->second.x;
        };
        auto tag = [&](char const* name) -> TextureTag const* {
            auto it = textures.find(name);
            return it == textures.end() ? nullptr : &it->second;
        };
        auto& w     = mat.water;
        mat.variant = Variant::water;
        mat.visible = true;
        if(auto const* t = tag("watercolor_texture"))
            if(auto tex = texture_for(*t->bitmap))
                w.color = *tex, w.curves.x = curve_of(*t->bitmap);
        if(auto const* t = tag("environment_map"))
            if(auto cube = cube_texture_for(*t->bitmap))
                w.reflection = *cube, w.misc.w = 1.f, w.curves.y = cube->curve;
        if(auto const* t = tag("wave_slope_array"))
        {
            w.slope_xform = t->xform;
            if(auto const* bitmap = m_map->tag_data<el::bitmap_t>(*t->bitmap))
                if(auto volume = upload_wave_volume(*bitmap))
                    w.slopes = volume->texture, w.slopes_sampler = volume->sampler,
                    w.misc.z = static_cast<f32>(volume->depth);
        }
        if(auto const* t = tag("wave_displacement_array"))
        {
            w.shape_xform = t->xform;
            if(auto const* bitmap = m_map->tag_data<el::bitmap_t>(*t->bitmap))
                if(auto volume = upload_wave_volume(*bitmap))
                    w.displacement = volume->texture, w.displacement_sampler = volume->sampler,
                    w.foam_params.w = static_cast<f32>(volume->depth);
        }
        if(auto const* t = tag("foam_texture"))
        {
            w.foam_xform = t->xform;
            if(auto tex = texture_for(*t->bitmap))
                w.foam = *tex;
        }
        if(auto const* t = tag("foam_texture_detail"))
        {
            w.foam_detail_xform = t->xform;
            if(auto tex = texture_for(*t->bitmap))
                w.foam_detail = *tex;
        }
        if(auto const* t = tag("global_shape_texture"))
            if(auto tex = texture_for(*t->bitmap))
                w.shape = *tex;
        w.foam_params.x = scalar("foam_height", 0.5f);
        w.foam_params.y = scalar("foam_pow", 2.f);
        w.foam_params.z = foam == "auto" ? 1.f : foam == "paint" ? 2.f : foam == "both" ? 3.f : 0.f;
        w.displacement_range = {scalar("displacement_range_x", 0.3f), scalar("displacement_range_y", 0.2f),
                                scalar("displacement_range_z", 0.3f), 0.f};
        w.slope   = {scalar("slope_range_x", 0.5f), scalar("slope_range_y", 0.4f),
                     scalar("detail_slope_scale_x", 10.f), scalar("detail_slope_scale_y", 3.f)};
        w.heights = {scalar("wave_height", 0.5f), scalar("wave_height_aux", 0.6f),
                     scalar("detail_slope_steepness", 0.5f), scalar("minimal_wave_disturbance", 1.f)};
        w.shading = {scalar("watercolor_coefficient", 1.f), scalar("reflection_coefficient", 1.f),
                     scalar("fresnel_coefficient", 0.1f), scalar("water_murkiness", 3.f)};
        w.refraction = {scalar("refraction_texcoord_shift", 0.12f),
                        scalar("refraction_extinct_distance", 1000.f),
                        scalar("refraction_depth_dominant_ratio", 0.95f),
                        scalar("bankalpha_infuence_depth", 0.22f)};
        w.misc.x = scalar("sunspot_cut", 0.01f);
        w.misc.y = scalar("shadow_intensity_mark", 0.5f);
        if(auto it = constants.find("water_diffuse"); it != constants.end())
            w.diffuse = it->second;
    }

    /* A wave volume (wave_slope_array V8U8, wave_displacement_array
     * A8R8G8B8): its slices, one per step of the wave loop, as an array */
    struct WaveVolume
    {
        std::shared_ptr<gfx::compat::texture_2da_t> texture;
        std::shared_ptr<gfx::sampler_t>             sampler;
        u32                                         depth{0};
    };

    std::optional<WaveVolume> upload_wave_volume(el::bitmap_t const& bitmap)
    {
        using el::bitmap_format_t;
        auto magic    = m_map->magic();
        auto images   = bitmap.images.span(magic);
        auto hardware = bitmap.hardware_textures.span(magic);
        if(images.empty() || hardware.empty())
            return std::nullopt;
        auto const& image  = images[0];
        auto const  format = image.format.value();
        if(format != bitmap_format_t::v8u8 && format != bitmap_format_t::a8r8g8b8)
            return std::nullopt;
        auto view = m_loader->load(hardware[0].resource);
        if(!view.has_value())
            return std::nullopt;
        u32  width  = static_cast<u32>(image.width.value());
        u32  height = static_cast<u32>(image.height.value());
        u32  depth  = std::max<u32>(image.depth, 1);
        u32  stride = el::slice_stride(format, width, height);
        auto stored = el::top_level(view.value(), image);
        bool const slopes = format == bitmap_format_t::v8u8;
        auto tex = std::make_shared<gfx::compat::texture_2da_t>(
            m_api,
            slopes ? blam::bitm::to_pixdesc(blam::bitm::format_t::V8U8)
                   : typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8),
            1);
        tex->alloc(size_3d<u32>{width, height, depth});
        for(u32 z = 0; z < depth; z++)
        {
            std::vector<u8> pixels;
            if(size_t(z) * stride >= stored.size() ||
               !el::linearize(stored.subspan(size_t(z) * stride), format, width, height, image.xbox_flags, pixels))
                return std::nullopt;
            if(slopes)
                for(auto& b : pixels)
                    b = static_cast<u8>(b + 0x80);
            else
                /* Little-endian A8R8G8B8 is B,G,R,A */
                for(size_t i = 0; i + 3 < pixels.size(); i += 4)
                    std::swap(pixels[i], pixels[i + 2]);
            tex->upload(std::move(pixels), Veci3{0, 0, static_cast<i32>(z)},
                        size_3d<i32>{static_cast<i32>(width), static_cast<i32>(height), 1});
        }
        auto sampler = tex->sampler();
        sampler->alloc();
        sampler->set_filtering(typing::Filtering::Linear, typing::Filtering::Linear);
        return WaveVolume{tex, sampler, depth};
    }

    /* Environment mapping: the BSP's cubemaps (dynamic) or the shader's own
     * cube (per_pixel), tinted */
    void load_environment(
        Material&                                     mat,
        std::string_view                              option,
        std::unordered_map<std::string, Vecf4> const& constants)
    {
        auto value = [&](char const* name, Vecf4 fallback) {
            auto it = constants.find(name);
            return it == constants.end() ? fallback : it->second;
        };
        mat.env_tint = Vecf4(
            Vecf3(value("env_tint_color", Vecf4(1.f))),
            value("environment_map_specular_contribution", Vecf4(0.f)).x);
        if(option == "dynamic")
            mat.env_mode = 1;
        else if(option == "per_pixel" && m_pending_environment_map)
        {
            if(auto cube = cube_texture_for(*m_pending_environment_map))
            {
                mat.environment_map = *cube;
                mat.env_mode        = 2;
            }
        }
    }

    /* A cube bitmap's first image, uploaded once per tag */
    std::optional<CubeTexture> cube_texture_for(el::tag_t const& bitmap_tag)
    {
        auto key = m_map->id_of(bitmap_tag).raw.value();
        if(auto it = m_cube_textures.find(key); it != m_cube_textures.end())
            return it->second;
        auto const* bitmap = m_map->tag_data<el::bitmap_t>(bitmap_tag);
        auto out = bitmap ? upload_cube(*bitmap, 0) : std::nullopt;
        m_cube_textures.emplace(key, out);
        return out;
    }

    /* One cube image of a bitmap: six faces stored like array slices, in
     * its own resource or interleaved with another image */
    std::optional<CubeTexture> upload_cube(el::bitmap_t const& bitmap, size_t index)
    {
        auto magic  = m_map->magic();
        auto images = bitmap.images.span(magic);
        if(index >= images.size())
            return std::nullopt;
        auto const& image = images[index];
        auto        h1    = h1_format(image.format.value());
        if(image.type.value() != el::bitmap_type_t::cube_map || !h1 ||
           (*h1 != blam::bitm::format_t::BC1 && *h1 != blam::bitm::format_t::BC2 &&
            *h1 != blam::bitm::format_t::BC3 && *h1 != blam::bitm::format_t::ARGB8))
            return std::nullopt;
        auto hardware = bitmap.hardware_textures.span(magic);
        auto interleaved = bitmap.interleaved_textures.span(magic);
        bool const in_pair = hardware.empty() && image.interleaved_interop >= 0 &&
                             static_cast<size_t>(image.interleaved_interop) < interleaved.size();
        if(!in_pair && index >= hardware.size())
            return std::nullopt;
        auto view = m_loader->load(
            in_pair ? interleaved[static_cast<size_t>(image.interleaved_interop)].resource
                    : hardware[index].resource);
        if(!view.has_value())
            return std::nullopt;
        auto stored = in_pair ? el::top_level_interleaved(view.value(), image)
                              : el::top_level(view.value(), image);
        u32 width  = static_cast<u32>(image.width.value());
        u32 height = static_cast<u32>(image.height.value());
        u32 stride = el::slice_stride(image.format.value(), width, height);
        std::array<std::vector<u8>, 6> faces;
        for(u32 f = 0; f < 6; f++)
            if(size_t(f) * stride >= stored.size() ||
               !el::linearize(
                   stored.subspan(size_t(f) * stride),
                   image.format.value(),
                   width,
                   height,
                   image.xbox_flags,
                   faces[f]))
                return std::nullopt;
        /* Little-endian A8R8G8B8 is B,G,R,A */
        if(*h1 == blam::bitm::format_t::ARGB8)
            for(auto& face : faces)
                for(size_t i = 0; i + 3 < face.size(); i += 4)
                    std::swap(face[i], face[i + 2]);
        auto tex = m_api->alloc_texture(gfx::textures::cube, blam::bitm::to_pixdesc(*h1), 1);
        tex->alloc(size_3d<u32>{width, height, 1});
        std::array<semantic::Span<const u8>, 6> spans;
        for(u32 f = 0; f < 6; f++)
            spans[f] = semantic::Span<const u8>(faces[f].data(), faces[f].size());
        tex->upload(spans, Veci2{0, 0}, Veci2{static_cast<i32>(width), static_cast<i32>(height)}, 0);
        CubeTexture out{.texture = tex, .sampler = tex->sampler(), .curve = curve_of(image.curve)};
        out.sampler->alloc();
        out.sampler->set_filtering(typing::Filtering::Linear, typing::Filtering::Linear);
        return out;
    }

    /* The BSP's cubemaps and the points they were captured at */
    void load_cubemaps(el::scenario_t const& scnr, i16 bsp_index)
    {
        auto const& map   = *m_map;
        auto        magic = map.magic();
        auto bsps = scnr.bsps.span(magic);
        if(static_cast<size_t>(bsp_index) >= bsps.size())
            return;
        auto const& entry  = bsps[static_cast<size_t>(bsp_index)];
        auto const  bitmap = map.get<el::bitmap_t>(entry.cubemaps);
        auto const  bsp    = map.get<el::structure_bsp_t>(entry.bsp);
        if(!bitmap || !bsp)
            return;
        auto images = bitmap->images.span(magic);
        std::vector<i32> uploaded(images.size(), -1);
        for(auto const& cluster : bsp->clusters.span(magic))
            for(auto const& point : cluster.cubemaps.span(magic))
            {
                i16 index = point.bitmap_index;
                if(index < 0 || static_cast<size_t>(index) >= images.size())
                    continue;
                if(uploaded[static_cast<size_t>(index)] < 0)
                {
                    auto cube = upload_cube(*bitmap, static_cast<size_t>(index));
                    if(!cube)
                        continue;
                    uploaded[static_cast<size_t>(index)] = static_cast<i32>(m_scene.cubemaps.size());
                    m_scene.cubemaps.push_back(*cube);
                }
                m_scene.cubemap_probes.push_back(
                    {point.position.value(), static_cast<u32>(uploaded[static_cast<size_t>(index)])});
            }
        cDebug(
            "H3: {} cubemaps at {} points", m_scene.cubemaps.size(), m_scene.cubemap_probes.size());
    }

    /* Self-illumination: simple, with a detail map, or three tinted
     * channels; other options with a map fall back to simple */
    template<typename Slot>
    static void load_self_illum(
        Material&                                     mat,
        std::string_view                              option,
        Slot const*                                   map,
        Slot const*                                   detail,
        std::unordered_map<std::string, Vecf4> const& constants)
    {
        if(!map || option.empty() || option == "off")
            return;
        auto value = [&](char const* name, Vecf4 fallback) {
            auto it = constants.find(name);
            return it == constants.end() ? fallback : it->second;
        };
        mat.illum       = map->texture;
        mat.illum_scale = map->scale;
        mat.illum_curve = map->curve;
        mat.self_illum  = 1;
        mat.illum_color = Vecf4(
            Vecf3(value("self_illum_color", Vecf4(1.f))), value("self_illum_intensity", Vecf4(1.f)).x);
        if(option == "illum_detail" && detail && map->scale.x != 0.f && map->scale.y != 0.f)
        {
            mat.self_illum         = 2;
            mat.illum_detail       = detail->texture;
            mat.illum_detail_ratio = detail->scale / map->scale;
        } else if(option.starts_with("multilayer"))
        {
            mat.self_illum   = 4;
            mat.illum_layers = Vecf4(
                value("layer_depth", Vecf4(0.f)).x,
                value("layer_contrast", Vecf4(1.f)).x,
                std::max(value("layers_of_4", Vecf4(1.f)).x, 1.f) * 4.f,
                value("depth_darken", Vecf4(1.f)).x);
            mat.illum_layer_xform =
                Vecf4(map->scale, value("texcoord_aspect_ratio", Vecf4(1.f)).x, 0.f);
        } else if(option == "3_channel_self_illum")
        {
            mat.self_illum     = 3;
            mat.illum_channels = {
                value("channel_a", Vecf4(1.f)),
                value("channel_b", Vecf4(1.f)),
                value("channel_c", Vecf4(1.f))};
        }
    }

    /* Material model parameters from the compiled float constants */
    static void load_specular(
        Material&                                     mat,
        std::string_view                              model,
        std::string_view                              mask,
        std::unordered_map<std::string, Vecf4> const& constants)
    {
        auto value = [&](char const* name, Vecf4 fallback) {
            auto it = constants.find(name);
            return it == constants.end() ? fallback : it->second;
        };
        auto scalar = [&](char const* name, f32 fallback) {
            return value(name, Vecf4(fallback)).x;
        };
        auto& s = mat.specular;
        s.coefficients = {
            scalar("specular_coefficient", 0.f),
            scalar("diffuse_coefficient", 1.f),
            scalar("analytical_specular_contribution", 0.5f),
            scalar("area_specular_contribution", 0.5f)};
        if(model == "two_lobe_phong")
        {
            mat.model = MaterialModel::two_lobe_phong;
            s.lobe    = {
                scalar("normal_specular_power", 10.f),
                scalar("glancing_specular_power", 10.f),
                scalar("fresnel_curve_steepness", 5.f),
                scalar("albedo_specular_tint_blend", 0.f)};
            s.tint0 = value("normal_specular_tint", Vecf4(1.f));
            s.tint1 = value("glancing_specular_tint", Vecf4(1.f));
        } else if(model == "cook_torrance")
        {
            mat.model = MaterialModel::cook_torrance;
            s.lobe    = {
                scalar("roughness", 0.1f),
                scalar("albedo_blend", 0.f),
                scalar("use_material_texture", 0.f),
                0.f};
            s.tint0 = value("fresnel_color", Vecf4(0.5f));
            s.tint1 = value("specular_tint", Vecf4(1.f));
        }
        if(mask == "specular_mask_from_diffuse")
            mat.specular_mask = 1;
        else if(mask == "specular_mask_from_texture" || mask == "specular_mask_from_color_texture")
            mat.specular_mask = 2;
    }

    /* The scenario's first atmosphere as compute_scattering's constants;
     * clusters naming none (the multiplayer maps) get it too */
    void load_atmosphere()
    {
        auto const& map = *m_map;
        m_scene.atmosphere = {Vecf4(0.f), Vecf4(0.f, 0.f, 0.f, -1.f)};
        auto const sky      = map.find<el::sky_atmosphere_t>();
        auto       settings = sky ? map.span(sky->settings) : Span<el::atmosphere_setting_t const>();
        if(settings.empty() ||
           !enum_helpers::feval(settings[0].flags.value(), el::atmosphere_flags_t::enable_atmosphere))
            return;
        auto const& s       = settings[0];
        f32 const   pitch   = glm::radians(s.sun_pitch.value());
        f32 const   heading = glm::radians(s.sun_heading.value());
        Vecf3 const sun_dir{
            std::cos(pitch) * std::cos(heading), std::cos(pitch) * std::sin(heading), std::sin(pitch)};
        /* The real sun lives elsewhere; its stand-in is the override */
        Vecf3 const sun = s.sun_color.value() * s.sun_intensity.value();
        Vecf3 const tr  = s.beta_m.value(), tm = s.beta_p.value();
        f32 const   g   = s.sun_phase_function, log2e = 1.4426950f;
        m_scene.atmosphere = {
            Vecf4(sun_dir, s.distance_bias),
            Vecf4(sun / glm::max(tr + tm, Vecf3(1e-9f)), s.max_fog_thickness),
            Vecf4(tr * log2e, 1.f + g * g),
            Vecf4(tm * log2e, s.sea_level),
            Vecf4(s.beta_m_theta_prefix.value(), s.mie_height_scale),
            Vecf4(s.beta_p_theta_prefix.value() * (1.f - g * g), s.rayleigh_height_scale),
            Vecf4(2.f * g, 0.f, 0.f, 0.f),
        };
        cDebug("H3: atmosphere {}", map.string(s.name));
    }

    /* Linear exposure for the camera FX of the cluster holding `eye`: the
     * middle of its auto exposure range, standing in for auto exposure */
    f32 exposure_at(el::scenario_t const& scnr, i16 bsp_index, Vecf3 const& eye)
    {
        auto const& map   = *m_map;
        auto        magic = map.magic();
        auto const bsp       = el::structure_bsp(map, scnr, bsp_index);
        int        camera_fx = -1;
        if(bsp)
            if(auto resources = m_loader->load(bsp->tag_resources); resources.has_value())
                if(auto const* cluster = el::cluster_at(map, *bsp, resources.value(), eye))
                    camera_fx = cluster->camera_fx;
        auto palette = scnr.camera_fx.span(magic);
        if(camera_fx < 0 || static_cast<size_t>(camera_fx) >= palette.size())
            camera_fx = palette.empty() ? -1 : 0;
        el::bounds_f range{};
        bool         found         = false;
        bool         forced_target = false;
        f32          target        = -4.5f;
        auto         settings      = map.get<el::camera_fx_settings_t>(scnr.camera_effects);
        m_scene.auto_exposure      = {};
        if(camera_fx >= 0)
        {
            auto const& entry = palette[static_cast<size_t>(camera_fx)];
            if(enum_helpers::feval(entry.flags, el::camera_fx_flags_t::force_exposure))
                return std::exp2(entry.forced_exposure.value());
            if(enum_helpers::feval(entry.flags, el::camera_fx_flags_t::force_auto_exposure))
                /* Linear here, unlike the camera FX setting's stops */
                target = std::log2(std::max<f32>(entry.forced_auto_exposure_brightness, 1e-6f)),
                forced_target = true;
            if(enum_helpers::feval(entry.flags, el::camera_fx_flags_t::override_exposure_bounds))
                range = entry.exposure_range, found = true;
            if(auto const own = map.get<el::camera_fx_settings_t>(entry.settings))
                settings = own;
        }
        if(settings && !found)
            range = settings->exposure_range, found = true;
        if(settings && !forced_target)
            target = settings->auto_exposure_brightness;
        if(!found)
            return 1.f;
        m_scene.auto_exposure = {
            .enabled = true, .target = target, .min = range.min, .max = range.max};
        auto light = m_scene.light_below(eye);
        f32  stops = light ? m_scene.auto_exposure.stops_for(glm::dot(
                                 h3::diffuse_radiance(*light, Vecf3(0.f, 0.f, 1.f)),
                                 Vecf3(0.2126f, 0.7152f, 0.0722f)))
                           : (range.min.value() + range.max.value()) * 0.5f;
        cDebug(
            "H3: camera fx {}, target {} stops, range [{}, {}], {} stops at the camera",
            camera_fx,
            target,
            range.min.value(),
            range.max.value(),
            stops);
        return std::exp2(stops);
    }

    /* A lightmap's size: its lightprobe texture's */
    Vecui2 lightprobe_extent(el::lightmap_bsp_t const& lit) const
    {
        auto const bitmap = m_map->get<el::bitmap_t>(lit.lightprobe_texture);
        auto       images = bitmap ? m_map->span(bitmap->images) : Span<el::bitmap_image_t const>();
        return images.empty() ? Vecui2(0)
                              : Vecui2(static_cast<u32>(images[0].width.value()), static_cast<u32>(images[0].height.value()));
    }

    /* A BSP's lightprobe texture and dominant light intensity, both arrays of
     * DXT5 slice pairs: ten layers for its slot of the lightprobe array, and
     * kept on the CPU for lighting objects from the lightmap under them */
    bool load_lightprobes(el::lightmap_bsp_t const& lit, u32 slot)
    {
        auto const& map = *m_map;
        if(m_scene.baked.size() <= slot)
            m_scene.baked.resize(slot + 1);
        if(m_probe_layers.size() <= slot)
            m_probe_layers.resize(slot + 1), m_probe_sizes.resize(slot + 1);
        std::vector<std::vector<u8>> layers;
        u32 width = 0, height = 0;
        for(auto const* ref : {&lit.lightprobe_texture, &lit.intensity_texture})
        {
            auto const bitmap   = map.get<el::bitmap_t>(*ref);
            auto       images   = bitmap ? map.span(bitmap->images) : Span<el::bitmap_image_t const>();
            auto       hardware = bitmap ? map.span(bitmap->hardware_textures)
                                         : Span<el::hardware_texture_t const>();
            if(images.empty() || hardware.empty() ||
               images[0].format.value() != el::bitmap_format_t::dxt5)
                return false;
            auto const& image = images[0];
            u32 w = static_cast<u32>(image.width.value());
            u32 h = static_cast<u32>(image.height.value());
            if((width && w != width) || (height && h != height))
                return false;
            width = w, height = h;
            auto view = m_loader->load(hardware[0].resource);
            if(!view.has_value())
                return false;
            auto stored = el::top_level(view.value(), image);
            u32  stride = el::slice_stride(image.format.value(), w, h);
            for(u32 z = 0; z < image.depth; z++)
            {
                if(size_t(z) * stride >= stored.size())
                    return false;
                layers.emplace_back();
                if(!el::linearize(
                       stored.subspan(size_t(z) * stride), image.format.value(), w, h, image.xbox_flags, layers.back()))
                    return false;
            }
        }
        if(layers.size() != 10 || !m_probe_extent.x || !m_probe_extent.y)
        {
            cWarning("H3: lightprobe textures have {} slices, expected 10", layers.size());
            return false;
        }
        auto&       baked = m_scene.baked[slot];
        auto const& c     = lit.compression;
        baked.layers          = layers;
        baked.size            = {width, height};
        baked.scale           = {c[0].value().x, c[2].value().x, c[4].value().x, c[6].value().x};
        baked.intensity_scale = c[8].value().x;
        m_scene.lightprobe_scales[slot]            = baked.scale;
        m_scene.intensity_scales[slot / 4][slot % 4] = baked.intensity_scale;
        m_probe_layers[slot] = std::move(layers);
        m_probe_sizes[slot]  = {width, height};
        /* Its texcoords reach only its corner of the array */
        m_slot_uv = Vecf2(Vecui2(width, height)) / Vecf2(m_probe_extent);
        return true;
    }

    /* The loaded BSPs' lightprobes as one array, ten layers a slot */
    void upload_lightprobes()
    {
        if(m_probe_layers.empty() || !m_probe_extent.x || !m_probe_extent.y)
            return;
        auto tex = std::make_shared<gfx::compat::texture_2da_t>(
            m_api, blam::bitm::to_pixdesc(blam::bitm::format_t::BC3), 1);
        tex->alloc(size_3d<u32>{m_probe_extent.x, m_probe_extent.y, static_cast<u32>(m_probe_layers.size() * 10)});
        for(size_t slot = 0; slot < m_probe_layers.size(); slot++)
        {
            auto const size = m_probe_sizes[slot];
            for(size_t z = 0; z < m_probe_layers[slot].size(); z++)
                tex->upload(
                    std::move(m_probe_layers[slot][z]),
                    Veci3{0, 0, static_cast<i32>(slot * 10 + z)},
                    size_3d<i32>{static_cast<i32>(size.x), static_cast<i32>(size.y), 1});
        }
        m_probe_layers.clear();
        m_scene.lightprobe         = tex;
        m_scene.lightprobe_sampler = tex->sampler();
        m_scene.lightprobe_sampler->alloc();
        m_scene.lightprobe_sampler->set_filtering(typing::Filtering::Linear, typing::Filtering::Linear);
    }

    /* Alpha test and blending from the definition's options; every
     * category's choice, by name */
    std::unordered_map<std::string, std::string> option_flags(
        el::render_method_view const& view, Material& mat)
    {
        std::unordered_map<std::string, std::string> out;
        for(auto const& [category, option] : view.options())
        {
            if(option.empty())
                continue;
            out.emplace(std::string(category), std::string(option));
            if(category == "albedo")
                mat.constant_color = option == "constant_color";
            else if(category == "alpha_test")
                mat.alpha_test = option != "none";
            else if(category == "blend_mode")
            {
                mat.additive = option == "additive";
                mat.blended  = option == "alpha_blend" || option == "pre_multiplied_alpha";
            }
        }
        return out;
    }

    static f32 curve_of(u8 curve)
    {
        switch(curve)
        {
        case 3: /* linear */
        case 4: /* offset log */
            return 1.f;
        case 5: /* sRGB */
            return 2.2f;
        default: /* unknown, xRGB and gamma 2.0 */
            return 2.f;
        }
    }

    /* Exponent decoding a bitmap's texels to linear, by its curve */
    f32 curve_of(el::tag_t const& bitmap_tag) const
    {
        auto const* bitmap = m_map->tag_data<el::bitmap_t>(bitmap_tag);
        auto images = bitmap ? m_map->span(bitmap->images) : Span<el::bitmap_image_t const>();
        if(images.empty())
            return 2.f;
        switch(images[0].curve)
        {
        case 3: /* linear */
        case 4: /* offset log */
            return 1.f;
        case 5: /* sRGB */
            return 2.2f;
        default: /* unknown, xRGB and gamma 2.0 */
            return 2.f;
        }
    }

    /* Top level of a bitmap's first image, uploaded once per tag */
    std::optional<Texture> texture_for(el::tag_t const& bitmap_tag)
    {
        auto const& map = *m_map;
        auto        key = map.id_of(bitmap_tag).raw.value();
        if(auto it = m_textures.find(key); it != m_textures.end())
            return it->second;
        auto out = upload_bitmap(bitmap_tag);
        m_textures.emplace(key, out);
        return out;
    }

    std::optional<Texture> upload_bitmap(el::tag_t const& bitmap_tag)
    {
        auto const& map    = *m_map;
        auto        magic  = map.magic();
        auto const* bitmap = map.tag_data<el::bitmap_t>(bitmap_tag);
        if(!bitmap)
            return std::nullopt;
        auto images = bitmap->images.span(magic);
        auto hardware = bitmap->hardware_textures.span(magic);
        if(images.empty() || hardware.empty())
            return std::nullopt;
        auto const& image  = images[0];
        auto        format = image.format.value();
        auto        h1     = h1_format(format);
        auto        rgtc   = rgtc_format(format);
        bool const  ctx1   = format == el::bitmap_format_t::ctx1;
        /* DXT3A decodes to one 8-bit channel, which goes up as Y8 or A8 */
        bool const dxt3a = format == el::bitmap_format_t::dxt3a ||
                           format == el::bitmap_format_t::dxt3a_mono ||
                           format == el::bitmap_format_t::dxt3a_alpha;
        if(dxt3a)
            h1 = format == el::bitmap_format_t::dxt3a_alpha ? blam::bitm::format_t::A8
                                                             : blam::bitm::format_t::Y8;
        if(image.type.value() != el::bitmap_type_t::texture_2d || (!h1 && !rgtc && !ctx1))
        {
            cDebug(
                "H3: bitmap {}: {} type {} not handled",
                map.name_of(bitmap_tag),
                el::to_string(format),
                static_cast<u16>(image.type.value()));
            return std::nullopt;
        }
        auto view = m_loader->load(hardware[0].resource);
        if(!view.has_value())
            return std::nullopt;
        u32 width  = static_cast<u32>(image.width.value());
        u32 height = static_cast<u32>(image.height.value());
        std::vector<u8> pixels;
        if(!el::linearize(
               el::top_level(view.value(), image),
               format,
               width,
               height,
               image.xbox_flags,
               pixels))
            return std::nullopt;
        using blam::bitm::format_t;
        using gfx::textures::swizzle_t;
        if(dxt3a)
        {
            std::vector<u8> texels;
            el::decode_dxt3a(pixels, width, height, texels);
            pixels = std::move(texels);
        }
        if(ctx1)
        {
            std::vector<u8> texels;
            el::decode_ctx1(pixels, width, height, texels);
            auto tex = m_api->alloc_texture(
                gfx::textures::d2, blam::bitm::to_pixdesc(format_t::V8U8), 1);
            tex->alloc(size_3d<u32>{width, height, 1});
            tex->upload(
                std::move(texels),
                Veci2{0, 0},
                Veci2{static_cast<i32>(width), static_cast<i32>(height)},
                0);
            return make_texture(tex);
        }
        if(rgtc)
        {
            if(format == el::bitmap_format_t::dxn)
                el::dxn_to_bc5(pixels);
            auto tex = m_api->alloc_texture(gfx::textures::d2, rgtc->desc, 1);
            /* Before alloc: without texture swizzle (WebGL) it is decoded in */
            auto const& sw = rgtc->swizzle;
            tex->set_swizzle(sw[0], sw[1], sw[2], sw[3]);
            tex->alloc(size_3d<u32>{width, height, 1});
            tex->upload(
                std::move(pixels),
                Veci2{0, 0},
                Veci2{static_cast<i32>(width), static_cast<i32>(height)},
                0);
            return make_texture(tex);
        }
        /* Little-endian A8R8G8B8 is B,G,R,A */
        if(*h1 == format_t::ARGB8 || *h1 == format_t::XRGB8)
            for(size_t i = 0; i + 3 < pixels.size(); i += 4)
            {
                std::swap(pixels[i], pixels[i + 2]);
                if(*h1 == format_t::XRGB8)
                    pixels[i + 3] = 0xFF;
            }
        if(*h1 == format_t::V8U8)
            for(auto& b : pixels)
                b = static_cast<u8>(b + 0x80);

        /* One- and two-channel formats and where their channels go */
        std::optional<std::array<swizzle_t, 4>> swizzle;
        switch(*h1)
        {
        case format_t::Y8:
            swizzle = {swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::one};
            break;
        case format_t::AY8:
            swizzle = {swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::red};
            break;
        case format_t::A8:
            swizzle = {swizzle_t::one, swizzle_t::one, swizzle_t::one, swizzle_t::red};
            break;
        case format_t::A8Y8:
            swizzle = {swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::green};
            break;
        default:
            break;
        }
        /* Without texture swizzle (WebGL) they go up expanded to RGBA8 */
        if(swizzle && !m_api->feature_info().texture.swizzle)
        {
            u32 const channels = *h1 == format_t::A8Y8 ? 2 : 1;
            std::vector<u8> rgba(size_t(width) * height * 4);
            for(size_t i = 0; i < size_t(width) * height; i++)
                for(size_t c = 0; c < 4; c++)
                {
                    auto sc = (*swizzle)[c];
                    rgba[i * 4 + c] = sc == swizzle_t::red     ? pixels[i * channels]
                                      : sc == swizzle_t::green ? pixels[i * channels + 1]
                                      : sc == swizzle_t::one   ? u8(255)
                                                               : u8(0);
                }
            auto tex = m_api->alloc_texture(
                gfx::textures::d2, typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8), 1);
            tex->alloc(size_3d<u32>{width, height, 1});
            tex->upload(
                std::move(rgba),
                Veci2{0, 0},
                Veci2{static_cast<i32>(width), static_cast<i32>(height)},
                0);
            return make_texture(tex);
        }
        auto tex = m_api->alloc_texture(gfx::textures::d2, blam::bitm::to_pixdesc(*h1), 1);
        tex->alloc(size_3d<u32>{width, height, 1});
        if(swizzle)
            tex->set_swizzle((*swizzle)[0], (*swizzle)[1], (*swizzle)[2], (*swizzle)[3]);
        tex->upload(
            std::move(pixels),
            Veci2{0, 0},
            Veci2{static_cast<i32>(width), static_cast<i32>(height)},
            0);
        return make_texture(tex);
    }

    static Texture make_texture(std::shared_ptr<gfx::texture_2d_t> const& tex)
    {
        Texture out{.texture = tex, .sampler = tex->sampler()};
        out.sampler->alloc();
        out.sampler->set_filtering(
            typing::Filtering::Linear, typing::Filtering::Linear, typing::Filtering::Linear);
        out.sampler->set_anisotropic(4);
        return out;
    }

    /* The Halo 1 loader creates the seats from the scenario; a Halo 3 map
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
            for(size_t frame = 0; frame < Visibility::type::frame_count_v; frame++)
                m_container.get_at<Visibility>(ref.id(), frame)->skip_render_for = 0;
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

    compo::EntityContainer& m_container;
    H3Scene&                m_scene;
    H3SoundBank&            m_bank;
    gfx::system*            m_api{nullptr};

    std::shared_ptr<comp_app::FileMapper::Resource>              m_file;
    std::vector<std::shared_ptr<comp_app::FileMapper::Resource>> m_side_files;
    std::vector<std::unique_ptr<el::map_container>>              m_side_maps;
    std::unique_ptr<el::map_container>                           m_map;
    std::unique_ptr<el::resource_loader>                         m_loader;

    /* Zone sets: each one's structure BSPs, and the triggers committing them */
    struct ZoneSwitch
    {
        el::trigger_volume_t const* volume;
        i32                         zone_set;
        bool                        inside{false}; /*!< The camera, last frame */
    };
    std::vector<u32>        m_zone_masks;
    /* Loaded BSPs sealed at each seam */
    std::vector<u8>         m_seam_users;
    std::vector<ZoneSwitch> m_switches;
    i32                     m_zone_set{-1};
    /* The BSP being loaded: its lightprobe slot and corner of the array */
    u32                     m_slot{0};
    Vecf2                   m_slot_uv{1.f, 1.f};
    Vecui2                  m_probe_extent{0};
    std::vector<std::vector<std::vector<u8>>> m_probe_layers;
    std::vector<Vecui2>                       m_probe_sizes;

    std::vector<Vertex>                                m_vertices;
    std::vector<u16>                                   m_indices;
    std::vector<el::vertex_t>                          m_decoded;
    std::vector<std::array<u16, 3>>                    m_triangles;


    std::unordered_map<u32, size_t>                    m_material_index;
    std::unordered_map<u32, std::optional<Texture>>    m_textures;
    std::unordered_map<u32, std::optional<el::resource_view>> m_model_views;
    std::unordered_map<u32, std::optional<CubeTexture>> m_cube_textures;
    el::tag_t const*                                     m_pending_environment_map{nullptr};
};

void alloc_h3(compo::EntityContainer& e)
{
    auto& scene  = e.register_subsystem_inplace<H3Scene>();
    auto& bank   = e.register_subsystem_inplace<H3SoundBank>();
    auto& loader = e.register_subsystem_inplace<H3MapLoader>(
        std::ref(e), std::ref(scene), std::ref(bank));
    e.subsystem_cast<GameEventBus>().addEventFunction<MapLoadEvent>(
        0, [&loader](GameEvent& ev, MapLoadEvent* load) {
            /* Consumed here: the Halo 1 loader never sees a Halo 3 map */
            if(loader.on_map_load(*load))
                ev.type = GameEvent::None;
        });
    alloc_h3_renderer(e);
}

} // namespace h3
