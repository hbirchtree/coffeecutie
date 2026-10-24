#include "h3_scene.h"
#include "../world/column_grid.h"
#include "../world/texels.h"
#include "../world/map_lifecycle.h"
#include "../world/player_spawn.h"
#include "../world/static_collision.h"
#include "h3_sound.h"

#include "components.h"
#include "data.h"
#include "physics.h"
#include "resource_creation.h"
#include "sound_cache.h"
#include "sounds.h"

#include <blam/elegy/animation.h>
#include <blam/elegy/bitmap.h>
#include <blam/elegy/decorator.h>
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
#include <glm/gtc/packing.hpp>

#if defined(OAF_XMA2_DECODER_ENABLED)
#include <oaf/xma2/xma2_decode.h>
#endif

#include <bit>
#include <cmath>
#include <limits>
#include <unordered_map>

namespace h3 {

namespace el = blam::elegy;
using semantic::BytesConst;
using semantic::Span;

/* 'H3' in the high bytes of ids this loader makes, apart from other games' */
inline constexpr u64 h3_ids = 0x4833'0000'0000'0000ull;

/* The structure BSP's static body in the physics world */
inline constexpr u64 collision_body_id = h3_ids;

/* A placed object that moves: drawn where its Model is, simulated by physics */
static const auto object_recipe = compo::EntityRecipe{
    .components = {compo::type_hash_v<Model>(), compo::type_hash_v<ObjectPhysics>()},
    .tags       = ObjectGC,
};

/* Collision polygons fanned into triangles, decoded out of the big-endian map */
using world::CollisionMesh;

/* The level's water surfaces: water is drawn, not collided with, so what
 * stands in it is found by height */
struct WaterSurfaces
{
    world::ColumnGrid<std::array<Vecf3, 3>> triangles;

    void add(std::array<Vecf3, 3> const& t)
    {
        triangles.add(t, t);
    }

    /*! The highest water surface above `point` that is below `ceiling` */
    std::optional<f32> surface_above(Vecf3 const& point, f32 ceiling) const
    {
        std::optional<f32> out;
        triangles.for_each_at(point, [&](std::array<Vecf3, 3> const& t) {
            Vecf2 const a = Vecf2(t[1] - t[0]), b = Vecf2(t[2] - t[0]), p = Vecf2(point - t[0]);
            f32 const   d = a.x * b.y - a.y * b.x;
            if(std::abs(d) < 1e-8f)
                return;
            f32 const u = (p.x * b.y - p.y * b.x) / d, v = (a.x * p.y - a.y * p.x) / d;
            if(u < 0.f || v < 0.f || u + v > 1.f)
                return;
            f32 const z = t[0].z + u * (t[1].z - t[0].z) + v * (t[2].z - t[0].z);
            if(z > point.z && z < ceiling && (!out || z > *out))
                out = z;
        });
        return out;
    }
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
        m_water = {};
        m_footsteps = nullptr;
        m_step_from.reset();
        m_impact_sounds.clear();
        m_impacts.clear();
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
        m_scene.change_colors.clear();
        m_cube_textures.clear();
        m_scene.baked.clear();
        m_vertices.clear();
        m_object_lights.clear();
        m_decorator_texels.clear();
        m_scene.decorators.clear();
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

    /* From the physics bus; heard in start_frame */
    void on_impact(Physics::Impact const& impact)
    {
        if(m_impact_sounds.contains(impact.entity_id))
            m_impacts.push_back(impact);
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
        world::clear_world(m_container);
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
        /* H3_SIDE_PREAD=1 reads the side caches by range on desktop too */
        if(world::side_pread("H3_SIDE_PREAD"))
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
        if(ok)
            load_world_sounds();
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

    /* Halo 3 sounds play through the shared SoundSystem: decoded here into
     * SoundItems, registered under their tag */
    static u64 sound_key(el::tag_reference<> const& ref)
    {
        return h3_ids | ref.tag_id.value();
    }

    static f32 db_gain(f32 db)
    {
        return std::pow(10.f, db / 20.f);
    }

    /* Each pitch range's permutations decoded on their own; how loud and how
     * far it carries from the gestalt's playback */
    bool decode_sound(
        el::tag_reference<> const& ref, std::vector<SoundItem::pitch_range_t>& ranges, SoundItem::props_t& props)
    {
#if defined(FEATURE_ENABLE_OAF) && defined(OAF_XMA2_DECODER_ENABLED)
        auto&       cache   = m_container.subsystem_cast<SoundCache<halo_version>>();
        auto const& map     = *m_map;
        auto        magic   = map.magic();
        auto        gestalt = map.find<el::sound_gestalt_t>();
        auto        sound   = map.get<el::sound_t>(ref);
        if(!cache.api || !gestalt || !sound)
            return false;
        auto const* codec = sound->codec.resolve(gestalt->codecs.span(magic));
        if(!codec || codec->compression != el::sound_compression_t::xma2)
            return false;
        auto view = m_loader->load(sound->resource);
        if(!view.has_value())
            return false;
        for(auto const& range : sound->pitch_ranges(*gestalt, magic))
        {
            SoundItem::pitch_range_t out;
            for(auto const& perm : el::permutations_of(range, *gestalt, magic))
            {
                auto bytes = el::permutation_data(perm, *gestalt, magic, view.value());
                if(bytes.empty())
                    continue;
                auto                       buffer = cache.api->alloc_buffer();
                oaf::decode::xma2::decoder xma2;
                if(!xma2.decode(
                       gsl::span<const char>(reinterpret_cast<char const*>(bytes.data()), bytes.size()),
                       {.channels = codec->channels(), .rate = codec->rate(), .samples = perm.sample_count},
                       *buffer))
                    continue;
                m_sound_bytes += size_t(perm.sample_count) * codec->channels() * 2;
                out.permutations.push_back({.buffer = std::move(buffer)});
            }
            out.count = static_cast<u32>(out.permutations.size());
            if(!out.permutations.empty())
                ranges.push_back(std::move(out));
        }
        if(auto const* playback = sound->playback.resolve(gestalt->playbacks.span(magic)))
        {
            props.gain_modifier = db_gain(playback->gain_base);
            props.min_distance  = playback->minimum_distance;
            props.max_distance  = playback->maximum_distance;
        }
        /* Unset distances: audible across a room, gone across the level */
        if(props.min_distance <= 0.f)
            props.min_distance = 1.f;
        if(props.max_distance <= props.min_distance)
            props.max_distance = std::max(props.min_distance * 10.f, 20.f);
        return !ranges.empty();
#else
        (void)ref, (void)ranges, (void)props;
        return false;
#endif
    }

    /* Hands a decoded sound to the SoundCache under `key`; the key, 0 if it
     * does not play */
    u64 register_sound(u64 key, SoundItem&& item)
    {
        bool const ok = item.valid() &&
                        m_container.subsystem_cast<SoundCache<halo_version>>().add_external(key, std::move(item)).valid();
        m_sound_keys[key] = ok;
        return ok ? key : 0;
    }

    /* A snd! as a one-shot; its key, 0 if it does not play */
    u64 ensure_sound(el::tag_reference<> const& ref)
    {
        if(!m_map->get<el::sound_t>(ref))
            return 0;
        u64 const key = sound_key(ref);
        if(auto it = m_sound_keys.find(key); it != m_sound_keys.end())
            return it->second ? key : 0;
        SoundItem          item{.name = std::string(m_map->name_of(ref))};
        SoundItem::track_t track;
        if(decode_sound(ref, track.buffers[SoundItem::main], track.props[SoundItem::main]))
            item.tracks.push_back(std::move(track));
        return register_sound(key, std::move(item));
    }

    /* A looping sound: its tracks' intro, loop and outro, and its details
     * as one-shots; its key, 0 if it does not play */
    u64 ensure_looping(el::tag_reference<> const& ref)
    {
        auto const looping = m_map->get<el::looping_sound_t>(ref);
        if(!looping)
            return 0;
        u64 const key = sound_key(ref);
        if(auto it = m_sound_keys.find(key); it != m_sound_keys.end())
            return it->second ? key : 0;
        auto const magic = m_map->magic();
        SoundItem  item{
             .name    = std::string(m_map->name_of(ref)),
             .looping = !enum_helpers::feval(looping->flags.value(), el::looping_sound_flags_t::not_a_loop),
        };
        for(auto const& source : looping->tracks.span(magic))
        {
            SoundItem::track_t track;
            for(auto [role, part] : {std::pair{SoundItem::start, &source.in},
                                     std::pair{SoundItem::loop, &source.loop},
                                     std::pair{SoundItem::end, &source.out}})
            {
                if(!m_map->get<el::sound_t>(*part))
                    continue;
                auto& props = track.props[role];
                if(!decode_sound(*part, track.buffers[role], props))
                {
                    track.buffers.erase(role);
                    track.props.erase(role);
                    continue;
                }
                props.gain_modifier *= db_gain(source.gain);
            }
            if(track.buffers.contains(SoundItem::loop) || track.buffers.contains(SoundItem::start))
                item.tracks.push_back(std::move(track));
        }
        for(auto const& detail : looping->details.span(magic))
            if(u64 const sound = ensure_sound(detail.sound))
                item.details.push_back({
                    .external = sound,
                    .period   = {detail.period.min, detail.period.max},
                    .gain     = db_gain(detail.gain),
                    .yaw      = {detail.yaw.min, detail.yaw.max},
                    .pitch    = {detail.pitch.min, detail.pitch.max},
                    .distance = {detail.distance.min, detail.distance.max},
                });
        return register_sound(key, std::move(item));
    }

    void inject_sound(SoundEvent ev, void const* payload)
    {
        if(auto* bus = m_container.service<comp_app::EventBus<SoundEvent>>())
            bus->inject(ev, const_cast<void*>(payload));
    }

    /* The small, medium and large impact sounds of the global material
     * making up most of an object's physics model */
    std::optional<std::array<el::tag_reference<> const*, 3>> impact_sweeteners(
        el::object_view const& view, el::tagged<el::phys::globals_t> const& globals)
    {
        auto const& map     = *m_map;
        auto const  magic   = map.magic();
        auto const& physics = view.physics_model;
        auto const  body    = physics ? el::phys::body_of(*physics, magic) : std::nullopt;
        if(!body || !globals || body->hulls.empty())
            return std::nullopt;
        auto const& heaviest = *std::ranges::max_element(body->hulls, {}, &el::phys::hull_t::mass);
        auto const* shape    = physics->material(magic, heaviest.material);
        auto const* material = shape ? globals->material(magic, shape->global_name()) : nullptr;
        if(!material)
            return std::nullopt;
        std::array<el::tag_reference<> const*, 3> out{};
        for(u32 size = 0; size < out.size(); size++)
            out[size] = material->sound_sweetener(*globals, magic, size);
        if(std::ranges::none_of(out, [](auto const* ref) { return ref != nullptr; }))
            return std::nullopt;
        return out;
    }

    /* Each object hit sounds by how hard: its momentum lost picks the size.
     * Only after a quiet spell, so a body grinding against another is heard
     * once */
    void play_impacts(time_point const& now)
    {
        for(auto const& impact : std::exchange(m_impacts, {}))
        {
            auto it = m_impact_sounds.find(impact.entity_id);
            if(it == m_impact_sounds.end())
                continue;
            bool const quiet = now - it->second.last_contact > std::chrono::milliseconds(300);
            it->second.last_contact = now;
            if(!quiet)
                continue;
            f32 const force = impact.speed * std::max(it->second.mass, 1.f);
            u32       size  = force < 40.f ? 0 : force < 400.f ? 1 : 2;
            while(size > 0 && !it->second.sweeteners[size])
                size--;
            auto const* sound = it->second.sweeteners[size];
            u64 const   key   = sound ? ensure_sound(*sound) : 0;
            if(!key)
                continue;
            PlaySoundEvent play{.external = key, .position = impact.point};
            inject_sound({.type = SoundEvent::play_sound, .entity_id = impact.entity_id}, &play);
        }
    }

    /* The player unit's footstep for the ground's global material: walking
     * or running, the material's own else its parents' */
    el::tag_reference<> const* footstep_sound(u16 material_index, bool submerged, bool running)
    {
        auto const& map     = *m_map;
        auto const  magic   = map.magic();
        auto const  globals = map.find<el::phys::globals_t>();
        auto const  effects = m_footsteps ? map.span(m_footsteps->effects) : Span<el::material_effect_t const>();
        if(!globals || effects.empty())
            return nullptr;
        auto const  materials = map.span(globals->materials);
        auto const& effect    = effects[running && effects.size() > 1 ? 1 : 0];
        auto const* material  = material_index < materials.size() ? &materials[material_index] : nullptr;
        /* Under water a material counts as its proxy, as the game reports it */
        if(submerged && material)
            material = material->underwater(*globals, magic);
        for(u32 depth = 0; material && depth < el::phys::global_material_t::max_lineage; depth++)
        {
            for(auto const& sound : map.span(effect.sounds))
                if(sound.material.raw.value() == material->name.raw.value() && sound.tag.valid())
                    return &sound.tag;
            material = material->parent_in(*globals, magic);
        }
        return nullptr;
    }

    /* The player in a body steps every stride it walks: a ray down finds the
     * ground's material, its footstep plays where it lands */
    void play_footsteps(time_point const& now)
    {
        if(!m_footsteps)
            return;
        std::optional<std::tuple<Vecf3, f32, u64>> body;
        for(auto entity : m_container.select<PlayerCamera, PlayerInfo>())
        {
            auto const& [camera, info] = entity.components();
            if(info.seat_idx == 0 && !info.is_remote() && info.mode.physics)
                body = std::make_tuple(camera.camera.position, info.biped.eye_height, entity.id());
        }
        if(!body)
        {
            m_step_from.reset();
            return;
        }
        auto const [eye, eye_height, id] = *body;
        if(!m_step_from)
        {
            m_step_from = eye;
            m_step_time = now;
            return;
        }
        f32 const moved = glm::length(Vecf2(eye) - Vecf2(*m_step_from));
        if(moved < (m_running ? run_stride : walk_stride))
            return;
        f32 const seconds = std::max(std::chrono::duration<f32>(now - m_step_time).count(), 1e-3f);
        m_step_from       = eye;
        m_step_time       = now;
        m_running         = moved / seconds > run_speed;
        Physics::Raycast ray{.from = eye, .to = eye - Vecf3(0.f, 0.f, eye_height + 0.3f), .ignore = id};
        Physics::Event   event{Physics::Event::Raycast};
        m_container.subsystem_cast<PhysicsBus>().process(event, &ray);
        if(!ray.hit)
            return;
        bool const  submerged = m_water.surface_above(ray.point, eye.z).has_value();
        auto const* sound     = footstep_sound(ray.material_type, submerged, m_running);
        u64 const   key   = sound ? ensure_sound(*sound) : 0;
        if(!key)
            return;
        PlaySoundEvent play{.external = key, .position = ray.point};
        inject_sound({.type = SoundEvent::play_sound, .entity_id = id}, &play);
    }

    /* After the world: the shared SoundSystem drops the last map's sounds,
     * the sound scenery starts, and events stop queueing */
    void load_world_sounds()
    {
        m_sound_keys.clear();
        m_background = 0;
        m_sound_bytes = 0;
        m_container.subsystem_cast<SoundCache<halo_version>>().clear_external();
        auto& loading         = m_container.subsystem_cast<LoadingStatus>();
        loading.loaded_sounds = LoadingStatus::loaded;
        if(!m_scenario)
            return;
        auto const& map = *m_map;
        m_scenery_loops.clear();
        for(auto const& placed : el::placements(map, m_scenario->sound_scenery, m_scenario->sound_scenery_palette))
        {
            auto const& object = placed.placement.object;
            if(!placed.object || !object.placed_at_load() || !object.in_bsps(m_sound_bsps))
                continue;
            auto const tag = map.get<el::object_t>(*placed.object);
            if(!tag)
                continue;
            for(auto const& [j, attachment] : stl_types::enumerate(map.span(tag->attachments)))
                if(auto const looping = map.get<el::looping_sound_t>(attachment.type))
                {
                    f32 const reach = looping->distance.max;
                    m_scenery_loops.push_back({
                        .sound    = &attachment.type,
                        .position = object.position,
                        .entity   = SoundEvent::sound_scenery_base | (u64(placed.index) << 8) | u64(j),
                        .reach    = reach > 0.f ? reach : 30.f,
                    });
                }
        }
        cDebug("H3: sound: {} sound scenery loops, {} objects with impact sounds", m_scenery_loops.size(),
               m_impact_sounds.size());
    }

    /* Sound scenery plays while the listener is in reach, so only what is
     * near is decoded */
    void update_scenery_loops(Vecf3 const& eye)
    {
        for(auto& loop : m_scenery_loops)
        {
            f32 const distance = glm::distance(eye, loop.position);
            if(!loop.playing && distance < loop.reach)
            {
                u64 const key = ensure_looping(*loop.sound);
                if(!key)
                    continue;
                PlaySoundEvent play{.external = key, .position = loop.position, .looping = true};
                inject_sound({.type = SoundEvent::play_sound, .entity_id = loop.entity}, &play);
                loop.playing = true;
            } else if(loop.playing && distance > loop.reach * 1.25f)
            {
                inject_sound({.type = SoundEvent::stop_sound, .entity_id = loop.entity}, nullptr);
                loop.playing = false;
            }
        }
    }

    /* The background of the cluster the listener is in, crossfaded as it
     * changes; a few times a second */
    void update_background(time_point const& now)
    {
        if(now - m_background_checked < std::chrono::milliseconds(250) || !m_scenario)
            return;
        m_background_checked = now;
        std::optional<Vecf3> eye;
        for(auto entity : m_container.select<PlayerCamera, PlayerInfo>())
        {
            auto const& [camera, info] = entity.components();
            if(info.seat_idx == 0 && !info.is_remote())
                eye = camera.camera.position;
        }
        if(!eye)
            return;
        update_scenery_loops(*eye);
        auto const& map = *m_map;
        u64         key = 0;
        for(i16 b = 0; b < 32 && !key; b++)
        {
            if(!(m_sound_bsps & (1u << b)))
                continue;
            auto const bsp = el::structure_bsp(map, *m_scenario, b);
            if(!bsp)
                continue;
            auto resources = m_loader->load(bsp->tag_resources);
            auto const* cluster = resources.has_value() ? el::cluster_at(map, *bsp, resources.value(), *eye) : nullptr;
            if(!cluster)
                continue;
            if(auto const* acoustics = cluster->acoustics.resolve(map.span(bsp->acoustics)))
                key = ensure_looping(acoustics->background_sound);
            break;
        }
        if(key == m_background)
            return;
        m_background = key;
        cDebug("H3: sound: {} sounds, {} MB decoded", m_sound_keys.size(), m_sound_bytes >> 20);
        BackgroundSoundTransitionEvent transition{.external = key};
        inject_sound({.type = SoundEvent::background_sound_transition}, &transition);
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
                if(tag.valid() && map.is(tag, blam::tag_class_t::lsnd) &&
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

        auto permutations = [&](el::tag_reference<> const& ref) {
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
                 .gain  = db_gain(track.gain.value()),
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
                mapper.open_ranged(world::sibling_url(load, name)),
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
        auto read = world::pread_reader(file);
        std::vector<u8> head(sizeof(el::file_header_t));
        auto source = file && file->size() && read(0, Span<u8>(head.data(), head.size()))
                          ? el::cache_source::from_header(
                                Span<u8 const>(head.data(), head.size()), read)
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
            auto url  = world::sibling_url(load, name);
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
        m_scene.first_person_buf =
            api.alloc_buffer(gfx::buffers::vertex, semantic::RSCA::Persistent | semantic::RSCA::WriteOnly);
        m_scene.vertex_buf->alloc();
        m_scene.element_buf->alloc();
        m_scene.first_person_buf->alloc();
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
    void start_frame(compo::ContainerProxy&, time_point const& now) override
    {
        if(m_scene.loaded)
            animate_first_person(now);
        if(m_scene.loaded)
            update_background(now);
        if(m_scene.loaded)
            play_impacts(now);
        if(m_scene.loaded)
            play_footsteps(now);
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
        world::clear_objects(m_container);
        m_scene.generation++;
        m_scene.materials.clear();
        m_material_index.clear();
        m_scene.cubemaps.clear();
        m_scene.cubemap_probes.clear();
        m_scene.lights.clear();
        m_scene.change_colors.clear();
        m_vertices.clear();
        m_object_lights.clear();
        m_decorator_texels.clear();
        m_scene.decorators.clear();
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
        m_water         = {};
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
        /* Where the sun is, for cast shadows: from the brightest baked light */
        {
            Vecf3 sum{0.f};
            for(auto const& bsp : m_scene.baked)
                if(auto sun = bsp.sun())
                    sum += *sun;
            m_scene.sun_direction = glm::length(sum) > 1e-6f ? glm::normalize(sum) : Vecf3(0.f, 0.f, 1.f);
        }
        cDebug("H3: sun towards ({}, {}, {})", m_scene.sun_direction.x, m_scene.sun_direction.y,
               m_scene.sun_direction.z);
        m_scene.exposure = exposure_at(scnr, here, eye);
        load_atmosphere();
        /* H3_EXPOSURE=<stops> overrides it, for comparing with captures */
        if(auto forced = platform::env::var("H3_EXPOSURE"); forced.has_value())
        {
            m_scene.exposure              = std::exp2(std::stof(forced.value()));
            m_scene.auto_exposure.enabled = false;
        }
        /* H3_BLOOM=<intensity> likewise */
        if(auto forced = platform::env::var("H3_BLOOM"); forced.has_value())
            m_scene.bloom.intensity = std::stof(forced.value());
        load_objects(scnr, bsps);
        m_scenario   = &scnr;
        m_sound_bsps = bsps;
        load_first_person(scnr, eye);
        load_sky(scnr, bsps);
        for(i16 b = 0; b < 32; b++)
            if(bsps & (1u << b))
                load_cubemaps(scnr, b);
        if(m_vertices.empty())
            return false;

        m_scene.vertex_buf->commit(
            Span<const Vertex>(m_vertices.data(), m_vertices.size()));
        commit_first_person();
        upload_object_lights();
        upload_decorator_instances();
        load_area_specular_tables();
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
                collision_materials,
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
                *collision,
                collision_materials);
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
        load_decorators(*bsp);
        return clusters + instances > 0;
    }

    /* The BSP's decorators: each set's mesh, to be drawn at every instance
     * its clusters' groups place */
    void load_decorators(el::structure_bsp_t const& bsp)
    {
        auto const& map  = *m_map;
        auto const  sets = map.span(bsp.decorator_sets);
        if(sets.empty())
            return;
        auto view = m_loader->load(bsp.decorator_geometry.resource);
        if(!view.has_value())
            return;
        std::vector<std::vector<el::decorator_instance_t>> placed(sets.size());
        for(auto const& cluster : map.span(bsp.clusters))
            for(auto const& group : map.span(cluster.decorator_groups))
                if(group.set >= 0 && static_cast<size_t>(group.set) < sets.size())
                {
                    auto instances = el::decorator_instances(view.value(), group);
                    auto& into     = placed[static_cast<size_t>(group.set)];
                    into.insert(into.end(), instances.begin(), instances.end());
                }
        u32 drawn = 0, total = 0;
        for(size_t s = 0; s < sets.size(); s++)
        {
            auto const set   = map.get<el::decorator_set_t>(sets[s]);
            auto const model = set ? map.get<el::render_model_t>(set->render_model) : el::tagged<el::render_model_t>{};
            auto const texture_tag = set ? map.tag_at(set->texture.tag_id) : nullptr;
            auto const meshes = model ? map.span(model->geometry.meshes) : Span<el::mesh_t const>();
            if(placed[s].empty() || meshes.empty() || !texture_tag)
                continue;
            auto mesh_view = m_loader->load(model->geometry.resource);
            auto texture   = texture_for(*texture_tag);
            if(!mesh_view.has_value() || !texture)
                continue;
            auto buffers = el::mesh_buffers::from(mesh_view.value(), meshes[0]);
            auto compression = map.span(model->geometry.compression);
            m_decoded.clear();
            if(!buffers.decode(compression.empty() ? nullptr : &compression[0], m_decoded) || m_decoded.empty())
                continue;
            u32 const vbase = static_cast<u32>(m_vertices.size());
            for(auto const& v : m_decoded)
                m_vertices.push_back({.position = v.position, .texcoord = v.texcoord, .normal = v.normal,
                                      .lightmap = Vecf3(0.f, 0.f, -1.f)});
            m_triangles.clear();
            buffers.triangles(meshes[0].index_type, 0, buffers.index_count(), m_triangles);
            DecoratorSet out{
                .diffuse        = *texture,
                .curve          = curve_of(*texture_tag),
                .fade           = Vecf2(set->start_fade, set->end_fade),
                .two_sided      = enum_helpers::feval(set->render_flags, el::decorator_render_flags_t::two_sided),
                .first_instance = static_cast<u32>(m_decorator_texels.size() / 3),
                .instance_count = static_cast<u32>(placed[s].size()),
            };
            out.draws.push_back({
                .elements =
                    {
                        .count         = static_cast<u32>(m_triangles.size() * 3),
                        .offset        = m_indices.size() * sizeof(u16),
                        .vertex_offset = vbase,
                        .type          = semantic::type_t::u16,
                    },
                .instances = {.count = out.instance_count, .offset = 0},
            });
            for(auto const& t : m_triangles)
                m_indices.insert(m_indices.end(), t.begin(), t.end());
            for(auto const& d : placed[s])
            {
                m_decorator_texels.push_back(Vecf4(d.position, d.motion));
                m_decorator_texels.push_back(Vecf4(d.rotation.x, d.rotation.y, d.rotation.z, d.rotation.w));
                m_decorator_texels.push_back(Vecf4(d.light, static_cast<f32>(d.type)));
            }
            m_scene.decorators.push_back(std::move(out));
            drawn++;
            total += static_cast<u32>(placed[s].size());
        }
        cDebug("H3: decorators: {} of {} sets, {} instances", drawn, sets.size(), total);
    }

    /* rasterizer\\cc0236 and dd0236: cook_torrance's pre-integrated area
     * specular, by view angle and roughness; half floats */
    void load_area_specular_tables()
    {
        auto const& map = *m_map;
        auto        out = m_scene.area_specular.begin();
        for(auto name : {"rasterizer\\cc0236", "rasterizer\\dd0236"})
        {
            auto& table = *out++;
            table       = {};
            auto const bitmap   = map.find<el::bitmap_t>(name);
            auto       images   = bitmap ? map.span(bitmap->images) : Span<el::bitmap_image_t const>();
            auto       hardware = bitmap ? map.span(bitmap->hardware_textures) : Span<el::hardware_texture_t const>();
            if(images.empty() || hardware.empty() || images[0].format.value() != el::bitmap_format_t::abgrfp16)
                continue;
            auto view = m_loader->load(hardware[0].resource);
            std::vector<u8> halves;
            if(!view.has_value() || !el::linearize_level(view.value(), images[0], 0, halves))
                continue;
            u32 const          width = static_cast<u32>(images[0].width.value());
            u32 const          height = static_cast<u32>(images[0].height.value());
            std::vector<f32>   texels(size_t(width) * height * 4);
            if(halves.size() < texels.size() * 2)
                continue;
            for(size_t i = 0; i < texels.size(); i++)
                texels[i] = glm::unpackHalf1x16(static_cast<u16>(halves[2 * i] | (halves[2 * i + 1] << 8)));
            auto tex = m_api->alloc_texture(
                gfx::textures::d2, typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA32F), 1);
            tex->alloc(size_3d<u32>{width, height, 1});
            auto const* bytes = reinterpret_cast<u8 const*>(texels.data());
            tex->upload(std::vector<u8>(bytes, bytes + texels.size() * sizeof(f32)), Veci2{0, 0},
                        Veci2{static_cast<i32>(width), static_cast<i32>(height)}, 0);
            table.texture = tex;
            table.sampler = tex->sampler();
            table.sampler->alloc();
            table.sampler->set_filtering(typing::Filtering::Linear, typing::Filtering::Linear);
        }
    }

    /* Instances as texels, three each, rows a multiple of three wide */
    void upload_decorator_instances()
    {
        constexpr u32 width = 3 * 1024;
        if(m_decorator_texels.empty())
            m_decorator_texels.resize(3);
        u32 const rows = static_cast<u32>((m_decorator_texels.size() + width - 1) / width);
        m_decorator_texels.resize(size_t(rows) * width);
        auto tex = m_api->alloc_texture(gfx::textures::d2, typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA32F), 1);
        tex->alloc(size_3d<u32>{width, rows, 1});
        auto const* bytes = reinterpret_cast<u8 const*>(m_decorator_texels.data());
        tex->upload(std::vector<u8>(bytes, bytes + m_decorator_texels.size() * sizeof(Vecf4)), Veci2{0, 0},
                    Veci2{static_cast<i32>(width), static_cast<i32>(rows)}, 0);
        m_scene.decorator_instances         = tex;
        m_scene.decorator_instances_sampler = tex->sampler();
        m_scene.decorator_instances_sampler->alloc();
        m_scene.decorator_instances_sampler->set_filtering(typing::Filtering::Nearest, typing::Filtering::Nearest);
        m_decorator_texels = {};
    }

    /* Appends a collision BSP's surfaces as fanned triangles */
    static void append_collision(
        el::coll::bsp_view_t const&             bsp,
        Matf4 const&                            transform,
        CollisionMesh&                          out,
        Span<el::collision_material_t const>    materials,
        std::vector<bool> const&                skipped_materials = {})
    {
        auto skip = [&](u32 s) {
            if(stl_types::any_flag_of(bsp.surfaces[s].flags, el::coll::surface_flags_t::invalid))
                return true;
            auto const material = bsp.surfaces[s].material.value();
            return material >= 0 && static_cast<size_t>(material) < skipped_materials.size() &&
                   skipped_materials[static_cast<size_t>(material)];
        };
        auto material_of = [&](u32 s) {
            i16 const material = bsp.surfaces[s].material.value();
            i16 const global   = material >= 0 && static_cast<size_t>(material) < materials.size()
                                     ? materials[static_cast<size_t>(material)].global_material.value()
                                     : i16(-1);
            return global >= 0 ? static_cast<u16>(global) : u16(0xFFFF);
        };
        world::append_fanned<el::coll::traits>(
            out, transform, bsp.vertices, [](auto const& v) { return v.point.value(); }, bsp.surfaces, bsp.edges, skip,
            material_of);
    }

    /* One static body for the loaded structure BSP */
    void submit_collision(std::shared_ptr<CollisionMesh> mesh, u64 id)
    {
        world::submit_static_collision(m_container.subsystem_cast<PhysicsBus>(), std::move(mesh), id);
    }

    /* Scenery, bipeds, vehicles, crates, weapons and equipment placed in this BSP. Those whose physics
     * model moves become entities that physics carries; the rest are drawn
     * in place. */
    void load_objects(el::scenario_t const& scnr, u32 bsps)
    {
        u32        moving = 0, fixed = 0, hidden = 0;
        auto const globals = m_map->find<el::phys::globals_t>();
        auto       place   = [&](auto const& placements, auto const& palette, bool item) {
            for(auto const& [placement, object_ref] :
                el::placements_at_load(*m_map, placements, palette, bsps))
            {
                auto const& object = placement.object;
                auto const  view   = object_ref ? el::object_view::of(*m_map, *object_ref) : el::object_view{};
                auto const  model  = view.object ? drawn_model(view) : el::tagged<el::render_model_t>{};
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
                auto        physics  = physics_of(view, scale);
                if(!physics)
                {
                    auto const skin = stored_pose(*model, object);
                    append_model(model, object.transform(), lit, 0, Quatf(1.f, 0.f, 0.f, 0.f), skin);
                    attach_lights(view, *model, object.transform(), 0);
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
                if(auto sweeteners = impact_sweeteners(view, globals))
                    m_impact_sounds[ref.id()] = {.sweeteners = *sweeteners, .mass = physics->mass};
                *m_container.get<ObjectPhysics>(ref.id()) = std::move(*physics);
                if(view.object && !m_map->span(view.object->change_colors).empty())
                    m_scene.change_colors[ref.id()] = change_colors_of(*view.object, object.variant);
                append_model(model, glm::scale(Matf4(1.f), Vecf3(scale)), lit, ref.id(), q);
                attach_lights(view, *model, glm::scale(Matf4(1.f), Vecf3(scale)), ref.id());
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

    /* The player's first-person hands and the starting weapon's first-person
     * model, playing the weapon's idle */
    void load_first_person(el::scenario_t const& scnr, Vecf3 const& eye)
    {
        auto const& map = *m_map;
        auto&       fp  = m_first_person;
        fp              = {};
        auto const globals = map.find<el::phys::globals_t>();
        if(!globals)
            return;
        /* The first with hands is the Chief, in campaign and multiplayer */
        el::phys::player_representation_t const* player = nullptr;
        for(auto const& representation : map.span(globals->player_representations))
            if(map.get<el::render_model_t>(representation.first_person_hands))
            {
                player = &representation;
                break;
            }
        if(!player)
            return;
        if(auto const unit = map.get<el::object_t>(player->third_person_unit))
        {
            m_scene.change_colors[first_person_entity] = change_colors_of(*unit, player->third_person_variant);
            m_footsteps = map.get<el::material_effects_t>(unit->material_effects).data;
        }
        auto const profiles = map.span(scnr.starting_profiles);
        auto const weapon   = profiles.empty() ? el::tagged<el::weapon_t>{}
                                               : map.get<el::weapon_t>(profiles[0].primary_weapon);
        auto const views = weapon ? map.span(weapon->first_person) : Span<el::weapon_first_person_t const>();
        auto const hands = map.get<el::render_model_t>(player->first_person_hands);
        auto const held  = views.empty() ? el::tagged<el::render_model_t>{} : map.get<el::render_model_t>(views[0].model);
        fp.models.push_back(hands.data);
        if(held)
            fp.models.push_back(held.data);
        /* The weapon's graph animates hands and weapon as one skeleton,
         * nodes it leaves alone where the models put them */
        auto const graph =
            views.empty() ? el::tagged<el::animation_graph_t>{} : map.get<el::animation_graph_t>(views[0].animations);
        if(auto const* idle = graph ? graph->find(map, "first_person:idle") : nullptr)
        {
            fp.graph = graph.data;
            fp.idle  = el::decode_animation(map, *m_loader, *graph, *idle, el::default_pose(map, *graph, fp.models));
            for(auto const& node : map.span(graph->nodes))
                fp.names.push_back(node.name);
        }
        auto const  light = m_scene.light_below(eye);
        auto const* lit   = light ? &*light : nullptr;
        u32         drawn = 0;
        for(size_t i = 0; i < fp.models.size(); i++)
        {
            fp.model = static_cast<i32>(i);
            drawn += append_model(
                i == 0 ? hands : held, Matf4(1.f), lit, first_person_entity, Quatf(1.f, 0.f, 0.f, 0.f),
                first_person_skin(0.f)[i]);
        }
        fp.model = -1;
        cDebug("H3: first person: '{}' holding '{}', {} draws, {}", map.name_of(player->first_person_hands),
               weapon ? map.name_of(*weapon.tag) : std::string_view("nothing"), drawn,
               fp.idle ? "idle" : "default pose");
    }

    /* Per first-person model: its skinning matrices `frame` frames into the
     * idle, between frames blended */
    std::vector<std::vector<Matf4>> first_person_skin(f32 frame) const
    {
        auto const&                     fp = m_first_person;
        std::vector<std::vector<Matf4>> out(fp.models.size());
        if(!fp.idle)
            return out;
        u32 const frames = fp.idle->frames;
        f32 const at     = std::fmod(frame, static_cast<f32>(frames));
        u32 const f0     = static_cast<u32>(at) % frames, f1 = (f0 + 1) % frames;
        f32 const t      = at - std::floor(at);
        auto      a = fp.idle->frame(f0), b = fp.idle->frame(f1);
        std::vector<el::node_pose_t> pose(a.size());
        for(size_t i = 0; i < pose.size(); i++)
            pose[i] = {
                .rotation    = glm::slerp(a[i].rotation, b[i].rotation, t),
                .translation = glm::mix(a[i].translation, b[i].translation, t),
                .scale       = a[i].scale + (b[i].scale - a[i].scale) * t,
            };
        auto const nodes = fp.graph->node_matrices(*m_map, pose);
        for(size_t i = 0; i < fp.models.size(); i++)
            out[i] = el::skinning_matrices(*m_map, *fp.models[i], fp.names, nodes);
        return out;
    }

    void upload_object_lights()
    {
        auto const rows = static_cast<u32>(std::max<size_t>(m_object_lights.size(), 1));
        m_object_lights.resize(rows);
        auto tex = m_api->alloc_texture(gfx::textures::d2, typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA32F), 1);
        tex->alloc(size_3d<u32>{5, rows, 1});
        auto const* bytes = reinterpret_cast<u8 const*>(m_object_lights.data());
        tex->upload(std::vector<u8>(bytes, bytes + m_object_lights.size() * sizeof(m_object_lights[0])), Veci2{0, 0},
                    Veci2{5, static_cast<i32>(rows)}, 0);
        m_scene.object_lights         = tex;
        m_scene.object_lights_sampler = tex->sampler();
        m_scene.object_lights_sampler->alloc();
        m_scene.object_lights_sampler->set_filtering(typing::Filtering::Nearest, typing::Filtering::Nearest);
        m_object_lights.clear();
    }

    /* The first person's vertices into their own buffer, its draws offset to
     * match, so posing them never touches the static one */
    void commit_first_person()
    {
        auto& fp = m_first_person;
        if(fp.meshes.empty())
            return;
        fp.first = fp.meshes.front().first;
        std::vector<Vertex> all;
        for(auto const& mesh : fp.meshes)
            all.insert(all.end(), mesh.drawn.begin(), mesh.drawn.end());
        m_scene.first_person_buf->commit(Span<const Vertex>(all.data(), all.size()));
        for(auto& mat : m_scene.materials)
            for(auto& object : mat.object_draws)
                if(object.entity == first_person_entity)
                    for(auto& draw : object.draws)
                        draw.elements.vertex_offset -= fp.first;
    }

    /* The idle at 30 frames a second: the meshes posed and uploaded again */
    void animate_first_person(time_point const& now)
    {
        auto& fp = m_first_person;
        if(!fp.idle || fp.meshes.empty())
            return;
        if(fp.start == time_point{})
            fp.start = now;
        auto const skins = first_person_skin(std::chrono::duration<f32>(now - fp.start).count() * 30.f);
        for(auto& mesh : fp.meshes)
        {
            m_decoded = mesh.bind;
            pose_vertices(*mesh.mesh, skins[mesh.model], m_decoded);
            for(size_t i = 0; i < mesh.drawn.size() && i < m_decoded.size(); i++)
            {
                mesh.drawn[i].position = m_decoded[i].position;
                mesh.drawn[i].normal   = m_decoded[i].normal;
            }
            m_scene.first_person_buf->update(
                (mesh.first - fp.first) * sizeof(Vertex), Span<const Vertex>(mesh.drawn.data(), mesh.drawn.size()));
        }
    }

    /* The lights an object carries at its markers (ligh attachments), held
     * still: colour and intensity at their steady value. Falloff constants
     * put the light's edge at its maximum distance. */
    void attach_lights(el::object_view const& view, el::render_model_t const& model, Matf4 const& frame, u64 entity)
    {
        auto const& map = *m_map;
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
                .color     = el::function_color(light->color.function.span(map.magic())) *
                         el::function_scalar(light->intensity.function.span(map.magic())),
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
    std::optional<ObjectPhysics> physics_of(el::object_view const& view, f32 scale) const
    {
        auto const& physics = view.physics_model;
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
        auto key = map.id_of(*model.tag);
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
    el::tagged<el::render_model_t> drawn_model(el::object_view const& view) const
    {
        return view.editor_only(*m_map) ? el::tagged<el::render_model_t>{} : view.render_model;
    }

    el::tagged<el::render_model_t> drawn_model(el::tag_reference<> const& object) const
    {
        return drawn_model(el::object_view::of(*m_map, object));
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
        /* The first person's meshes are posed again every frame */
        bool const first_person = entity == first_person_entity && m_first_person.model >= 0;
        if(first_person)
            m_first_person.meshes.push_back({
                .mesh  = &mesh,
                .model = static_cast<u32>(m_first_person.model),
                .first = vbase,
                .bind  = m_decoded,
            });
        if(!skin.empty())
            pose_vertices(mesh, skin, m_decoded);
        /* An object's light is shaded per pixel from its row of the table;
         * the first person's follows the camera instead */
        Vecf3 unlit(0.f, 0.f, -1.f);
        if(object_light && !first_person)
        {
            auto const row = h3::packed(*object_light);
            if(m_object_lights.empty() || m_object_lights.back() != row)
                m_object_lights.push_back(row);
            unlit = Vecf3(static_cast<f32>(m_object_lights.size() - 1), 0.f, -2.f);
        }
        for(size_t i = 0; i < m_decoded.size(); i++)
        {
            auto const& v = m_decoded[i];
            Vertex      out{
                     .position = Vecf3(transform * Vecf4(v.position, 1.f)),
                     .texcoord = v.texcoord,
                     .normal   = glm::normalize(normal * v.normal),
                     .lightmap = lit ? Vecf3(lightmap_uvs[i] * m_slot_uv, static_cast<f32>(m_slot)) : unlit,
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
        if(first_person)
            m_first_person.meshes.back().drawn.assign(m_vertices.begin() + vbase, m_vertices.end());

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
                m_scene.baked[m_slot].triangles.add(out, out.position);
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
            if(mat.variant == Variant::water && !entity)
                for(auto const& t : m_triangles)
                    m_water.add({m_vertices[vbase + t[0]].position, m_vertices[vbase + t[1]].position,
                                 m_vertices[vbase + t[2]].position});
            if(entity)
            {
                if(mat.object_draws.empty() || mat.object_draws.back().entity != entity)
                    mat.object_draws.push_back({.entity = entity});
                auto& object = mat.object_draws.back();
                for(size_t v = vbase; v < m_vertices.size(); v++)
                    object.lo = glm::min(object.lo, m_vertices[v].position),
                    object.hi = glm::max(object.hi, m_vertices[v].position);
                draws = &object.draws;
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
    static void pose_vertices(el::mesh_t const& mesh, Span<Matf4 const> skin, std::vector<el::vertex_t>& vertices)
    {
        using el::vertex_type_t;
        auto const type  = mesh.vertex_type;
        bool const rigid = type == vertex_type_t::rigid || type == vertex_type_t::flat_rigid;
        bool const skinned = type == vertex_type_t::skinned || type == vertex_type_t::flat_skinned;
        if(rigid && (mesh.rigid_node < 0 || static_cast<size_t>(mesh.rigid_node) >= skin.size()))
            return;
        for(auto& v : vertices)
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
    size_t material_for(el::tag_reference<> const* method_ref)
    {
        auto const& map  = *m_map;
        auto const  view = method_ref ? el::render_method_view::of(map, *method_ref)
                                      : el::render_method_view{};
        if(!view)
            return 0;
        el::datum_index const key = method_ref->tag_id;
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

        if(map.is(*tag, blam::tag_class_t::rmw_))
        {
            load_water(mat, constants, texture_tags, option("foam"));
        } else if(map.is(*tag, blam::tag_class_t::rmtr))
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
            if(auto const* mask = slot("change_color_map");
               mask && detail && albedo == "two_change_color")
            {
                mat.change_color  = true;
                mat.detail2       = mask->texture;
                mat.detail2_scale = mask->scale;
            }
            if(albedo == "constant_color")
                mat.variant = Variant::base;
            load_specular(mat, option("material_model"), option("specular_mask"), constants);
            /* Halograms hardcode material_model none */
            if(map.is(*tag, blam::tag_class_t::rmhg))
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
        auto key = m_map->id_of(bitmap_tag);
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
            if(auto const own = map.get<el::camera_fx_settings_t>(entry.settings))
                settings = own;
            m_scene.bloom = bloom_of(settings ? settings.data : nullptr, &entry);
            if(enum_helpers::feval(entry.flags, el::camera_fx_flags_t::force_exposure))
                return std::exp2(entry.forced_exposure.value());
            if(enum_helpers::feval(entry.flags, el::camera_fx_flags_t::force_auto_exposure))
                /* Linear here, unlike the camera FX setting's stops */
                target = std::log2(std::max<f32>(entry.forced_auto_exposure_brightness, 1e-6f)),
                forced_target = true;
            if(enum_helpers::feval(entry.flags, el::camera_fx_flags_t::override_exposure_bounds))
                range = entry.exposure_range, found = true;
        } else
            m_scene.bloom = bloom_of(settings ? settings.data : nullptr, nullptr);
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

    /* An object's starting primary and secondary change colours for a model
     * variant, else its default; white where it has none */
    std::array<Vecf3, 2> change_colors_of(el::object_t const& object, el::string_id variant) const
    {
        auto const&          map    = *m_map;
        auto const           wanted = map.string(variant);
        std::array<Vecf3, 2> out{Vecf3(1.f), Vecf3(1.f)};
        auto                 colors = map.span(object.change_colors);
        for(size_t c = 0; c < std::min<size_t>(colors.size(), out.size()); c++)
        {
            el::change_color_permutation_t const* pick = nullptr;
            int                                   rank = -1;
            for(auto const& p : map.span(colors[c].initial_permutations))
            {
                auto const name = map.string(p.variant);
                int const  r    = name == wanted ? 2 : name == "default" || name.empty() ? 1 : 0;
                if(r > rank)
                    pick = &p, rank = r;
            }
            if(pick)
                out[c] = (Vecf3(pick->lower_bound) + Vecf3(pick->upper_bound)) * .5f;
        }
        return out;
    }

    /* Bloom of camera FX settings, values marked default from globals\default's */
    h3::Bloom bloom_of(el::camera_fx_settings_t const* settings, el::camera_fx_palette_t const* entry) const
    {
        auto const fallback = m_map->find<el::camera_fx_settings_t>("globals\\default");
        auto       pick     = [&](auto member) {
            if(settings && !(settings->*member).use_default())
                return (settings->*member).value.value();
            if(fallback)
                return (fallback.data->*member).value.value();
            return decltype((settings->*member).value.value()){};
        };
        h3::Bloom out{
            .point     = pick(&el::camera_fx_settings_t::bloom_point),
            .inherent  = pick(&el::camera_fx_settings_t::inherent_bloom),
            .intensity = pick(&el::camera_fx_settings_t::bloom_intensity),
            .colors    = {
                pick(&el::camera_fx_settings_t::bloom_large_color),
                pick(&el::camera_fx_settings_t::bloom_medium_color),
                pick(&el::camera_fx_settings_t::bloom_small_color)},
        };
        if(entry && enum_helpers::feval(entry->flags, el::camera_fx_flags_t::override_inherent_bloom))
            out.inherent = entry->inherent_bloom;
        if(entry && enum_helpers::feval(entry->flags, el::camera_fx_flags_t::override_bloom_intensity))
            out.intensity = entry->bloom_intensity;
        cDebug("H3: bloom point {}, inherent {}, intensity {}", out.point, out.inherent, out.intensity);
        return out;
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
        auto        key = map.id_of(bitmap_tag);
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
        u32 const width  = static_cast<u32>(image.width.value());
        u32 const height = static_cast<u32>(image.height.value());
        using blam::bitm::format_t;
        using gfx::textures::swizzle_t;

        /* One- and two-channel formats and where their channels go */
        auto const swizzle = world::channel_swizzle(h1.value_or(format_t::ARGB8));
        /* Without texture swizzle (WebGL) they go up expanded to RGBA8 */
        bool const expand = !rgtc && !ctx1 && swizzle && !m_api->feature_info().texture.swizzle;

        /* A level's pixels as the texture takes them */
        auto prepare = [&](std::vector<u8>& pixels, u32 w, u32 h) {
            if(dxt3a)
            {
                std::vector<u8> texels;
                el::decode_dxt3a(pixels, w, h, texels);
                pixels = std::move(texels);
            }
            if(ctx1)
            {
                std::vector<u8> texels;
                el::decode_ctx1(pixels, w, h, texels);
                pixels = std::move(texels);
                return;
            }
            if(rgtc)
            {
                if(format == el::bitmap_format_t::dxn)
                    el::dxn_to_bc5(pixels);
                return;
            }
            world::fix_channels(*h1, pixels);
            if(expand)
            {
                u32 const       channels = *h1 == format_t::A8Y8 ? 2 : 1;
                std::vector<u8> rgba(size_t(w) * h * 4);
                for(size_t i = 0; i < size_t(w) * h; i++)
                    for(size_t c = 0; c < 4; c++)
                    {
                        auto sc         = (*swizzle)[c];
                        rgba[i * 4 + c] = sc == swizzle_t::red     ? pixels[i * channels]
                                          : sc == swizzle_t::green ? pixels[i * channels + 1]
                                          : sc == swizzle_t::one   ? u8(255)
                                                                   : u8(0);
                    }
                pixels = std::move(rgba);
            }
        };

        /* Every level the texture keeps, so the far distance filters down */
        std::vector<std::vector<u8>> levels;
        for(u32 l = 0; l < el::level_count(view.value(), image); l++)
        {
            std::vector<u8> pixels;
            if(!el::linearize_level(view.value(), image, l, pixels))
                break;
            prepare(pixels, std::max(width >> l, 1u), std::max(height >> l, 1u));
            levels.push_back(std::move(pixels));
        }
        if(levels.empty())
            return std::nullopt;
        auto const desc = ctx1     ? blam::bitm::to_pixdesc(format_t::V8U8)
                          : rgtc   ? rgtc->desc
                          : expand ? typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8)
                                   : blam::bitm::to_pixdesc(*h1);
        auto tex = m_api->alloc_texture(gfx::textures::d2, desc, static_cast<u32>(levels.size()));
        /* Before alloc: without texture swizzle (WebGL) RGTC is decoded in */
        if(rgtc)
            tex->set_swizzle(rgtc->swizzle[0], rgtc->swizzle[1], rgtc->swizzle[2], rgtc->swizzle[3]);
        tex->alloc(size_3d<u32>{width, height, 1});
        if(swizzle && !expand && !rgtc && !ctx1)
            tex->set_swizzle((*swizzle)[0], (*swizzle)[1], (*swizzle)[2], (*swizzle)[3]);
        for(u32 l = 0; l < levels.size(); l++)
            tex->upload(
                std::move(levels[l]),
                Veci2{0, 0},
                Veci2{static_cast<i32>(std::max(width >> l, 1u)), static_cast<i32>(std::max(height >> l, 1u))},
                static_cast<i32>(l));
        return world::sampled(tex);
    }

    void place_camera(Vecf3 const& position, f32 yaw)
    {
        /* H3_CAMO=1 starts the player camouflaged */
        if(auto* info = world::ensure_local_player(m_container))
            info->camouflaged = platform::env::var("H3_CAMO").value_or("0") != "0";
        world::place_local_cameras(m_container, position, yaw);
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

    /* Sounds handed to the SoundSystem, by key: true once registered */
    std::unordered_map<u64, bool> m_sound_keys;
    el::scenario_t const*         m_scenario{nullptr};
    u32                           m_sound_bsps{0};
    u64                           m_background{0};
    size_t                        m_sound_bytes{0};

    struct SceneryLoop
    {
        el::tag_reference<> const* sound{nullptr};
        Vecf3                      position{};
        u64                        entity{0};
        f32                        reach{0.f}; /*!< lsnd distance bounds */
        bool                       playing{false};
    };
    std::vector<SceneryLoop> m_scenery_loops;

    struct ImpactSounds
    {
        std::array<el::tag_reference<> const*, 3> sweeteners{}; /*!< Small, medium, large */
        f32                                       mass{0.f};
        time_point                                last_contact{};
    };
    std::unordered_map<u64, ImpactSounds> m_impact_sounds;
    std::vector<Physics::Impact>          m_impacts;

    el::material_effects_t const* m_footsteps{nullptr}; /*!< The player unit's */
    WaterSurfaces                 m_water;
    std::optional<Vecf3>          m_step_from;
    time_point                    m_step_time{};
    bool                          m_running{false};
    /* World units per footstep, about three a second at a run */
    static constexpr f32 walk_stride = 0.45f;
    static constexpr f32 run_stride  = 0.72f;
    static constexpr f32 run_speed   = 1.2f; /*!< World units per second */
    time_point                    m_background_checked{};
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
    std::vector<std::array<Vecf4, 5>>                  m_object_lights; /*!< Rows of H3Scene::object_lights */
    std::vector<Vecf4>                                 m_decorator_texels; /*!< H3Scene::decorator_instances */
    /* The first person's idle, and its meshes as uploaded */
    struct FirstPersonMesh
    {
        el::mesh_t const*         mesh{nullptr};
        u32                       model{0}; /*!< Into FirstPerson::models */
        u32                       first{0}; /*!< Its first vertex in the buffer */
        std::vector<el::vertex_t> bind;
        std::vector<Vertex>       drawn;
    };
    struct FirstPerson
    {
        el::animation_graph_t const*           graph{nullptr};
        std::optional<el::decoded_animation_t> idle;
        std::vector<el::string_id>             names; /*!< The graph's nodes */
        std::vector<el::render_model_t const*> models;
        std::vector<FirstPersonMesh>           meshes;
        i32                                    model{-1}; /*!< Being drawn, while loading */
        u32                                    first{0};  /*!< Its first vertex in the static buffer */
        time_point                             start{};
    } m_first_person;
    std::vector<std::array<u16, 3>>                    m_triangles;


    std::unordered_map<el::datum_index, size_t>                 m_material_index;
    std::unordered_map<el::datum_index, std::optional<Texture>> m_textures;
    std::unordered_map<el::datum_index, std::optional<el::resource_view>>
        m_model_views;
    std::unordered_map<el::datum_index, std::optional<CubeTexture>>
        m_cube_textures;
    el::tag_t const*                                     m_pending_environment_map{nullptr};
};

void alloc_h3(compo::EntityContainer& e)
{
    auto& scene  = e.register_subsystem_inplace<H3Scene>();
    auto& bank   = e.register_subsystem_inplace<H3SoundBank>();
    auto& loader = e.register_subsystem_inplace<H3MapLoader>(
        std::ref(e), std::ref(scene), std::ref(bank));
    world::consume_map_loads(e, loader);
    e.subsystem_cast<PhysicsBus>().addEventFunction<Physics::Impact>(
        0, [&loader](Physics::Event&, Physics::Impact* impact) { loader.on_impact(*impact); });
    alloc_h3_renderer(e);
}

} // namespace h3
