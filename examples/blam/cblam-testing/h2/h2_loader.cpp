#include "h2_bitmap.h"
#include "h2_scene.h"
#include "../world/column_grid.h"
#include "../world/texels.h"
#include "../world/map_lifecycle.h"
#include "../world/player_spawn.h"
#include "../world/static_collision.h"

#include <blam/common/vertex_pack.h>
#include "h2_shell_load.h"
#include "h2_sound.h"

#include "components.h"
#include "data.h"
#include "physics.h"
#include "resource_creation.h"
#include "sounds.h"

#include <blam/dimeter/animation.h>
#include <blam/dimeter/bitm.h>
#include <blam/dimeter/collision.h>
#include <blam/dimeter/fog.h>
#include <blam/dimeter/geometry.h>
#include <blam/dimeter/lightmap.h>
#include <blam/dimeter/vista.h>
#include <blam/dimeter/map.h>
#include <blam/dimeter/views.h>
#include <blam/dimeter/material.h>
#include <blam/dimeter/scenario.h>
#include <blam/dimeter/shader.h>
#include <blam/common/portal_walk.h>
#include <blam/volta/blam_swizzle.h>

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>
#include <coffee/components/subsystem.h>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/task_queue/task.h>
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

#if defined(COFFEE_EMSCRIPTEN)
#include <emscripten/heap.h>
#include <malloc.h>
#endif

#include <array>
#include <functional>
#include <bit>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <optional>
#include <cstring>
#include <set>
#include <unordered_map>

namespace h2 {

using blam::from_le;
using Coffee::Resource;
using libc_types::i64;
using libc_types::i8;
using semantic::BytesConst;

/* Lighting an external source supplies for a section's vertices: lightmap
 * texcoords (i16 pairs expanded through `bounds`) or per-vertex colours */
struct ExternalLighting
{
    Span<i16 const>            lightmap_uv; /*!< 2 per vertex */
    geo::model_bounds_t const* bounds{nullptr};
    Span<u8 const>             rgb; /*!< 3 per vertex */
};

/* A water definition as append_section sees a section: its counts are not
 * stored, the vertices come from the position stream (f32 x3) */
struct WaterSection
{
    geo::water_definition_t const*          water;
    dim::raw_ref                            data;
    dim::bsp_block<geo::resource_t>         resources;
    u16                                     vertices{0};

    void const* owner() const
    {
        return water;
    }

    u16 vertex_count() const
    {
        return vertices;
    }

    u16 face_count() const
    {
        return 0;
    }

    i32 data_size() const
    {
        return water->data_size();
    }

    i32 base_address() const
    {
        return water->base_address();
    }
};

/* An object with a physics body: physics moves its Model */
static const auto moving_recipe = compo::EntityRecipe{
    .components =
        {
            compo::type_hash_v<Model>(),
            compo::type_hash_v<ObjectPhysics>(),
        },
    .tags = ObjectGC,
};

/* Parses a Halo 2 cache into H2Scene: geometry, materials, textures,
 * lightmaps, placed objects, collision and visibility data. Event driven,
 * no frame work */
struct H2MapLoader : compo::SubsystemBase
{
    using type = H2MapLoader;

    H2MapLoader(
        compo::EntityContainer& e, H2Scene& scene, H2ShellData& shell, H2SoundBank& bank)
        : m_container(e)
        , m_scene(scene)
        , m_shell(shell)
        , m_bank(bank)
    {
    }

    /* Scripted placements are picked at load, so a change reloads the map */
    void start_frame(ContainerProxy&, time_point const&) override
    {
        bool const show =
            m_container.subsystem_cast<RenderingParameters>().show_scripted_placements;
        if(!m_source || show == m_scripted_shown)
            return;
        m_scripted_shown = show;
        auto      load   = *m_source;
        GameEvent ev{GameEvent::MapLoadStart};
        m_container.subsystem_cast<GameEventBus>().process(ev, &load);
    }

    /* True if the file is a Halo 2 cache; it is then loaded (or at least
     * attempted) here and the Halo 1 loader must not see it */
    bool on_map_load(MapLoadEvent const& load)
    {
        if(!load.file)
            return false;
        /* Fetched and sniffed below already, and found not to be ours */
        if(load.file_data)
        {
            unload();
            release_shared();
            return false;
        }
        /* Outside the virtual filesystem (web) a file only arrives
         * asynchronously: take the load and hand it back if it is Halo 1 */
        if(compile_info::platform::is_emscripten &&
           !platform::file::exists(*load.file).has_value())
        {
            fetch_and_load(load);
            return true;
        }
        auto& mapper = m_container.subsystem_cast<comp_app::FileMapper>();
        auto  file   = mapper.fetch(*load.file).get();
        auto  bytes  = file ? static_cast<BytesConst>(*file) : BytesConst{};
        if(!is_halo2_cache(bytes))
        {
            unload();
            release_shared();
            return false;
        }
        for(size_t i = 0; i < side_cache_names.size(); i++)
            m_side_sources[i] =
                /* H2_SIDE_PREAD=1 reads them by range on desktop too */
                world::side_pread("H2_SIDE_PREAD") ? ranged_source(mapper.open_ranged(
                                   side_cache_url(load, i)).get())
                             : mapped_source(
                                   mapper.fetch(side_cache_url(load, i)).get());
        m_font_table.clear();
        m_font_files.clear();
        if(is_mainmenu(bytes))
        {
            auto table = mapper.fetch(font_url(load, "font_table.txt")).get();
            if(table)
                m_font_table = font_table_of(static_cast<BytesConst>(*table));
            for(auto const& name : font_names())
                if(auto file = mapper.fetch(font_url(load, name)).get())
                    m_font_files.push_back({name, file});
        }
        dim::raw_pool pool;
        side_pool(pool);
        return load_cache(load, bytes, pool);
    }

    /* The shell's fonts live next to the map, in fonts/ */
    platform::url::Url font_url(MapLoadEvent const& load, std::string const& name) const
    {
        return (load.file->path().dirname() / "fonts" / name).url(load.file->flags);
    }

    static std::vector<std::string> font_table_of(BytesConst const& bytes)
    {
        return blam::dimeter::font::parse_font_table(
            Span<const u8>(reinterpret_cast<u8 const*>(bytes.data), bytes.size));
    }

    /* Each font file once, in table order */
    std::vector<std::string> font_names() const
    {
        std::vector<std::string> out;
        for(auto const& name : m_font_table)
            if(std::find(out.begin(), out.end(), name) == out.end())
                out.push_back(name);
        return out;
    }

    static bool is_mainmenu(BytesConst const& bytes)
    {
        if(!is_halo2_cache(bytes))
            return false;
        auto type = [](auto const& header) {
            return from_le(static_cast<i32>(header.map_type)) ==
                   static_cast<i32>(dim::cache_type_t::mainmenu);
        };
        if(auto map = dim::map_container<dim::xbox_version_t>::from_bytes(
               bytes, dim::xbox_version);
           map.has_value())
            return type(*map.value().header);
        if(auto map = dim::map_container<dim::vista_version_t>::from_bytes(
               bytes, dim::vista_version);
           map.has_value())
            return type(*map.value().header);
        return false;
    }

    static std::shared_ptr<dim::raw_source> mapped_source(
        std::shared_ptr<comp_app::FileMapper::Resource> res)
    {
        auto bytes = res ? static_cast<BytesConst>(*res) : BytesConst{};
        if(!bytes.data || !bytes.size)
            return {};
        return std::make_shared<dim::span_source>(
            Span<const u8>(bytes.data, bytes.size), std::move(res));
    }

    static std::shared_ptr<dim::raw_source> ranged_source(
        std::shared_ptr<comp_app::FileMapper::RangedFile> file)
    {
        if(!file || !file->size())
            return {};
        auto const size = file->size();
        return std::make_shared<dim::pread_source>(size, world::pread_reader(std::move(file)));
    }

    platform::url::Url side_cache_url(MapLoadEvent const& load, size_t i) const
    {
        return world::sibling_url(load, side_cache_names[i].second);
    }

    /* The side caches found */
    void side_pool(dim::raw_pool& pool)
    {
        for(size_t i = 0; i < side_cache_names.size(); i++)
            if(m_side_sources[i])
                pool.attach(side_cache_names[i].first, m_side_sources[i]);
    }

    /* Everything kept across map changes: the side caches and the
     * textures uploaded from them */
    void release_shared()
    {
        m_side_sources     = {};
        m_shared_textures.clear();
    }

    static bool is_halo2_cache(BytesConst const& bytes)
    {
        /* Xbox and Vista headers share the id/version/footer positions */
        return bytes.data && bytes.size >= sizeof(dim::file_header_xbox_t) &&
               reinterpret_cast<dim::file_header_xbox_t const*>(bytes.data)
                   ->valid();
    }

    /* Bitmap pixels may live in the global cache files next to the map */
    static constexpr std::array<std::pair<dim::raw_file_t, const char*>, 3>
        side_cache_names = {{
            {dim::raw_file_t::mainmenu, "mainmenu.map"},
            {dim::raw_file_t::shared, "shared.map"},
            {dim::raw_file_t::single_player_shared, "single_player_shared.map"},
        }};

    /* Web: the map, then the side caches, each brought into the local
     * store by FileMapper off the main thread; the load itself runs on the
     * main queue once everything is in. The side caches are read by range
     * so the heap never holds them whole */
    void fetch_and_load(MapLoadEvent const& load)
    {
        using AsyncResource = comp_app::FileMapper::Resource;
        using RangedFile    = comp_app::FileMapper::RangedFile;
        auto& mapper = m_container.subsystem_cast<comp_app::FileMapper>();
        auto  task   = rq::CreateMultiTask<void>(
            [this, load = MapLoadEvent(load)](
                std::shared_ptr<AsyncResource> file) mutable {
                auto bytes =
                    file ? static_cast<BytesConst>(*file) : BytesConst{};
                if(!is_halo2_cache(bytes))
                {
                    /* Not ours: the Halo 1 loader gets the fetched file */
                    load.file_data =
                        file ? file : std::make_shared<AsyncResource>();
                    GameEvent ev{GameEvent::MapLoadStart};
                    m_container.subsystem_cast<GameEventBus>().inject(
                        ev, &load);
                    return;
                }
                auto& mapper =
                    m_container.subsystem_cast<comp_app::FileMapper>();
                auto sides = rq::CreateMultiTask<void>(
                    [this, load, file](
                        std::shared_ptr<RangedFile> mainmenu,
                        std::shared_ptr<RangedFile> shared,
                        std::shared_ptr<RangedFile> sp_shared) {
                        m_side_sources = {
                            ranged_source(mainmenu),
                            ranged_source(shared),
                            ranged_source(sp_shared)};
                        auto finish = [this, load, file] {
                            dim::raw_pool pool;
                            side_pool(pool);
                            load_cache(load, static_cast<BytesConst>(*file), pool);
                        };
                        m_font_table.clear();
                        m_font_files.clear();
                        if(is_mainmenu(static_cast<BytesConst>(*file)))
                            fetch_fonts(load, std::move(finish));
                        else
                            finish();
                    },
                    mapper.open_ranged(side_cache_url(load, 0)),
                    mapper.open_ranged(side_cache_url(load, 1)),
                    mapper.open_ranged(side_cache_url(load, 2)));
                rq::runtime_queue::Queue(std::move(sides)).assume_value();
            },
            mapper.fetch_cached(*load.file));
        rq::runtime_queue::Queue(std::move(task)).assume_value();
    }

    /* Web: font_table.txt, then the fonts it names one after the other,
     * each through the store; `done` runs on the main queue after */
    void fetch_fonts(MapLoadEvent const& load, std::function<void()> done)
    {
        using AsyncResource = comp_app::FileMapper::Resource;
        auto& mapper = m_container.subsystem_cast<comp_app::FileMapper>();
        rq::runtime_queue::Queue(
            rq::dependent_task<std::shared_ptr<AsyncResource>, void>::CreateSink(
                mapper.fetch_cached(font_url(load, "font_table.txt")),
                [this, load, done = std::move(done)](
                    std::shared_ptr<AsyncResource>* table) mutable {
                    if(*table)
                        m_font_table = font_table_of(static_cast<BytesConst>(**table));
                    fetch_font(load, font_names(), 0, std::move(done));
                }))
            .assume_value();
    }

    void fetch_font(
        MapLoadEvent const&      load,
        std::vector<std::string> names,
        size_t                   i,
        std::function<void()>    done)
    {
        using AsyncResource = comp_app::FileMapper::Resource;
        if(i >= names.size())
        {
            done();
            return;
        }
        auto& mapper = m_container.subsystem_cast<comp_app::FileMapper>();
        auto  url    = font_url(load, names[i]);
        rq::runtime_queue::Queue(
            rq::dependent_task<std::shared_ptr<AsyncResource>, void>::CreateSink(
                mapper.fetch_cached(url),
                [this, load, names = std::move(names), i, done = std::move(done)](
                    std::shared_ptr<AsyncResource>* file) mutable {
                    if(*file)
                        m_font_files.push_back({names[i], *file});
                    fetch_font(load, std::move(names), i + 1, std::move(done));
                }))
            .assume_value();
    }

    bool load_cache(
        MapLoadEvent const& load, BytesConst const& bytes, dim::raw_pool& pool)
    {
        unload();
        world::clear_world(m_container);

        pool.attach(
            dim::raw_file_t::local, Span<const u8>(bytes.data, bytes.size));

        ensure_gpu();
        m_scene.generation++;

        bool loaded = false;
        if(auto map = dim::map_container<dim::xbox_version_t>::from_bytes(
               bytes, dim::xbox_version);
           map.has_value())
            loaded = load_map(map.value(), pool);
        else if(auto map = dim::map_container<dim::vista_version_t>::from_bytes(
                    bytes, dim::vista_version);
                map.has_value())
        {
            /* Vista borrows weapons, templates and the like from shared.map */
            auto vista  = map.value();
            auto shared = dim::load_shared(vista, pool);
            if(shared)
                vista.shared = &*shared;
            cDebug(
                "H2: shared map {}; side files {} / {} / {} bytes",
                shared ? "attached" : "none",
                pool.file_size(dim::raw_file_t::mainmenu),
                pool.file_size(dim::raw_file_t::shared),
                pool.file_size(dim::raw_file_t::single_player_shared));
            loaded = load_map(vista, pool);
        }
        else
            cWarning(
                "H2: not a supported Halo 2 cache: error {}",
                static_cast<int>(map.error()));
        if(!loaded)
        {
            cWarning("H2: nothing loaded from {}", load.file->internUrl);
            release_shared();
            return false;
        }
        m_source = MapLoadEvent{
            .origin = load.origin, .directory = load.directory, .file = load.file};
        m_scripted_shown =
            m_container.subsystem_cast<RenderingParameters>().show_scripted_placements;
        /* Shared textures this map did not ask for go away */
        size_t const before = m_shared_textures.size();
        std::erase_if(m_shared_textures, [this](auto const& entry) {
            return entry.second.generation != m_scene.generation;
        });
        cDebug(
            "H2: {} shared textures kept, {} evicted",
            m_shared_textures.size(),
            before - m_shared_textures.size());
        for(size_t i = 0; i < side_cache_names.size(); i++)
            if(auto* src = dynamic_cast<dim::pread_source*>(
                   m_side_sources[i].get()))
            {
                cDebug(
                    "H2: {} read by range: {} KiB of {} KiB",
                    side_cache_names[i].second,
                    src->bytes_read() >> 10,
                    src->size() >> 10);
                src->trim();
            }
#if defined(COFFEE_EMSCRIPTEN)
        cDebug(
            "H2: heap in use {} MiB of {} MiB",
            static_cast<u64>(mallinfo().uordblks) >> 20,
            static_cast<u64>(emscripten_get_heap_size()) >> 20);
#endif
        return true;
    }

    void unload()
    {
        m_source.reset();
        m_shell.loaded = false;
        m_bank.loaded  = false;
        m_scene.loaded = false;
        m_scene.sections.clear();
        m_scene.switches.clear();
        m_scene.materials.clear();
        m_scene.moving_objects.clear();
        m_scene.animations.clear();
        m_scene.animated_objects.clear();
        m_scene.first_person = {};
        m_animation_index.clear();
        m_scene.water = {};
        m_scene.lightmap_tex.reset();
        m_scene.lightmap_sampler.reset();
        m_scene.start_section = 0;
        m_vertices.clear();
        m_indices.clear();
        m_material_index.clear();
        m_placed.clear();
        m_textures.clear();
        m_cubes.clear();
        m_buckets.clear();
        m_scenery_lights.clear();
        m_objects_total = m_objects_clustered = 0;
        m_lightmaps.clear();
        m_lightmap_atlas = {};
        m_light_triangles.clear();
        m_below_lit_objects = m_unlit_objects = 0;
        m_normal_order_logged = false;
    }

  private:
    /* The scene's buffers are the loader's to fill */
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


    template<typename V>
    bool load_map(dim::map_container<V> const& map, dim::raw_pool const& pool)
    {
        auto const* scnr_tag = map.scenario();
        if(!scnr_tag)
            return false;
        auto const* scenario_body =
            scnr_tag->template data<dim::scn::scenario_t>(map.magic());
        if(!scenario_body)
            return false;
        std::optional<Vecf3> center;
        /* The scene section each located BSP becomes */
        u32 bsp_idx = 0;
        for(auto const& located : dim::structure_bsps(map))
        {
            auto collision = std::make_shared<CollisionMesh>();
            /* The BSP region has its own virtual base */
            auto const* bsp       = located.body;
            auto        bsp_magic = located.magic;
            if(scenario_body)
                load_decorators(map, pool, *scenario_body, *bsp, bsp_magic, static_cast<i32>(bsp_idx));
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
            auto const* bsp_tag = located.tag;
            m_scene.sections.push_back(Section{
                .name      = bsp_tag ? std::string(map.name_of(*bsp_tag)) : "",
                .lo        = {from_le(bsp->x_min), from_le(bsp->y_min), from_le(bsp->z_min)},
                .hi        = {from_le(bsp->x_max), from_le(bsp->y_max), from_le(bsp->z_max)},
                .collision = std::move(collision),
            });
            gather_visibility(*bsp, bsp_magic, m_scene.sections.back());
            if(scenario_body)
                gather_planar_fog(map, *scenario_body, *bsp, bsp_magic, m_scene.sections.back());

            /* Lightmaps: one image per cluster and per per-pixel instance,
             * through the region's ltmp; buckets light the rest per vertex */
            std::vector<LightmapImage const*> cluster_lightmaps;
            std::vector<LightmapImage const*> instance_lightmaps;
            std::vector<LightBucket>          buckets;
            std::vector<SceneryLight>         instance_lights;
            if(auto const* ltmp = located.lightmap)
                    if(auto groups = ltmp->groups.data(bsp_magic);
                       !groups.has_error() && !groups.value().empty())
                    {
                        auto const& group = groups.value()[0];
                        cluster_lightmaps = load_lightmaps(
                            map, pool, group, group.cluster_render_info, bsp_magic);
                        instance_lightmaps = load_lightmaps(
                            map, pool, group, group.instance_render_info, bsp_magic);
                        buckets         = load_buckets(pool, group, bsp_magic);
                        instance_lights = load_bucket_refs(
                            group.instance_bucket_refs, bsp_magic);
                        m_scenery_lights.push_back(
                            load_scenery_lights(group, bsp_magic));
                        m_buckets.push_back(buckets);
                    }
            if(m_scenery_lights.size() <= bsp_idx)
                m_scenery_lights.resize(bsp_idx + 1);
            if(m_buckets.size() <= bsp_idx)
                m_buckets.resize(bsp_idx + 1);

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
                    if(scenario_body && cluster_count < m_scene.sections.back().clusters.size())
                        resolve_fog(
                            map,
                            *scenario_body,
                            cluster,
                            m_scene.sections.back().clusters[cluster_count]);
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
                        DrawTag{
                            .section         = static_cast<i32>(bsp_idx),
                            .cluster_section = static_cast<i32>(bsp_idx),
                            .cluster = static_cast<i32>(cluster_count - 1),
                        });
                }
            if(auto waters = bsp->water_definitions.data(bsp_magic); !waters.has_error())
                for(auto const& water : waters.value())
                    sections += append_water(map, pool, bsp_magic, water, bsp_idx);
            std::vector<bool> instanced;
            if(!geom.has_error())
                instanced.assign(geom.value().size(), false);
            if(!geom.has_error() && !instances.has_error())
                for(auto const& inst : instances.value())
                {
                    auto sidx = inst.section.value();
                    if(sidx < 0 ||
                       static_cast<size_t>(sidx) >= geom.value().size())
                        continue;
                    instanced[sidx] = true;
                    /* Per-pixel instances have a lightmap and bucket UVs,
                     * per-vertex ones bucket colours */
                    ExternalLighting     lighting;
                    LightmapImage const* lightmap = nullptr;
                    auto                 def_bounds =
                        geom.value()[sidx].bounds.data(bsp_magic);
                    if(instance_count < instance_lights.size())
                    {
                        auto const& light = instance_lights[instance_count];
                        u32         vo    = light.section_offsets.empty()
                                                ? 0
                                                : static_cast<u32>(std::max<i16>(
                                                      0, light.section_offsets[0]));
                        u32 count = geom.value()[sidx].vertex_count();
                        if(light.bucket >= 0 &&
                           static_cast<size_t>(light.bucket) < buckets.size())
                        {
                            auto const& bucket = buckets[light.bucket];
                            bool per_pixel = inst.lightmapping_policy ==
                                             dim::geo::instance_lightmapping_policy_t::per_pixel;
                            if(per_pixel && instance_count < instance_lightmaps.size() &&
                               bucket.uv.size() >= (vo + count) * 2)
                            {
                                lightmap = instance_lightmaps[instance_count];
                                lighting.lightmap_uv =
                                    Span<i16 const>(bucket.uv.data() + vo * 2, count * 2);
                                if(!def_bounds.has_error() && !def_bounds.value().empty())
                                    lighting.bounds = &def_bounds.value()[0];
                            } else if(bucket.rgb.size() >= (vo + count) * 3)
                                lighting.rgb =
                                    Span<u8 const>(bucket.rgb.data() + vo * 3, count * 3);
                        }
                    }
                    size_t const first = m_vertices.size();
                    sections += append_section(
                        pool,
                        bsp_magic,
                        geom.value()[sidx],
                        instance_transform(inst),
                        nullptr,
                        materials,
                        false,
                        lightmap,
                        DrawTag{
                            .section         = static_cast<i32>(bsp_idx),
                            .cluster_section = static_cast<i32>(bsp_idx),
                            .instance        = static_cast<i32>(instance_count),
                        },
                        lighting);
                    if(!lightmap && lighting.rgb.empty())
                        light_from_below(first, Vecf3(instance_transform(inst)[3]));
                    instance_count++;
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
                            DrawTag{.section = static_cast<i32>(bsp_idx)});
                    }

            cDebug(
                "H2: sbsp {} ({}): {} of {} clusters + {} instances + {} "
                "orphan sections emitted, {} materials",
                bsp_idx++,
                bsp_tag ? map.name_of(*bsp_tag) : std::string_view{},
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
            load_first_person(map, pool, *scenario);
            load_switches(map, *scenario);
            if(auto locs = scenario->player_starting_locations.data(map.magic());
               !locs.has_error() && !locs.value().empty())
                start_section = locs.value()[0].bsp_index.value();
        }

        if(m_vertices.empty())
            return false;

        upload_lightmaps();
        m_scene.vertex_buf->commit(
            Span<const Vertex>(m_vertices.data(), m_vertices.size()));
        m_scene.element_buf->commit(
            Span<const u16>(m_indices.data(), m_indices.size()));
        u32 draws = 0;
        for(auto const& mat : m_scene.materials)
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
            m_scene.materials.size(),
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
        if(start_section < 0 || static_cast<size_t>(start_section) >= m_scene.sections.size())
            start_section = 0;
        m_scene.start_section = start_section;
        if(auto window = m_container.service<comp_app::WindowInfo>())
            window->setName(fmt::format("Blam! : {}", map.name()));
        m_scene.loaded = true;
        /* Halo 2's colours want no Halo 1 tonemap: shown as authored */
        auto& post       = m_container.subsystem_cast<PostProcessParameters>();
        post.exposure    = 1.f;
        post.gamma       = 1.f;
        post.auto_expose = false;
        post.tonemap     = false;

        /* mainmenu.map brings the shell with it */
        std::vector<ShellFontFile> fonts;
        for(auto const& [name, file] : m_font_files)
        {
            auto bytes = static_cast<BytesConst>(*file);
            fonts.push_back(
                {name,
                 Span<const u8>(
                     reinterpret_cast<u8 const*>(bytes.data), bytes.size)});
        }
        m_shell = {};
        m_bank  = {};
        if(from_le(static_cast<i32>(map.header->map_type)) ==
               static_cast<i32>(dim::cache_type_t::mainmenu) ||
           !m_font_files.empty())
            load_shell(*m_api, map, pool, m_font_table, fonts, m_shell, m_bank);
        m_shell.generation = m_scene.generation;
        m_bank.generation  = m_scene.generation;
        m_font_files.clear();

        /* The Halo 1 systems key off LoadingStatus and would read their
         * empty container, so only the app-level state says "loaded" (which
         * is what the dummy plug waits for) */
        if(auto* app_info = m_container.service<comp_app::AppInfo>())
            app_info->setState(comp_app::interfaces::AppInfo::loaded);
        return true;
    }

    /* Appends a collision BSP's surfaces as fanned triangles */
    template<blam::heap_t Heap>
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
        world::append_fanned<dim::coll::traits>(
            out,
            transform,
            vertices.value(),
            [](auto const& v) { return Vecf3{from_le(v.point[0]), from_le(v.point[1]), from_le(v.point[2])}; },
            surfaces.value(),
            edges.value(),
            [&](u32 s) { return surfaces.value()[s].has(dim::coll::surface_flags_t::invalid); });
    }

    /* The highest-priority fog plane with a fog tag: fills the space
     * under it */
    template<typename V>
    void gather_planar_fog(
        dim::map_container<V> const& map,
        dim::scn::scenario_t const&  scnr,
        geo::bsp_header_t const&     bsp,
        dim::bsp_magic const&        magic,
        Section&                     out)
    {
        auto planes  = bsp.fog_planes.data(magic);
        auto palette = scnr.planar_fog_palette.data(map.magic());
        if(planes.has_error() || palette.has_error())
            return;
        i16 best = std::numeric_limits<i16>::min();
        for(auto const& fp : planes.value())
        {
            auto const* entry = fp.scenario_planar_fog.resolve(palette.value());
            auto const* tag   = entry ? map.tag_at(entry->planar_fog.tag_id) : nullptr;
            auto const* fog =
                tag ? tag->template data<dim::fog::header_t>(map.magic()) : nullptr;
            if(!fog || from_le(fp.priority_raw) <= best)
                continue;
            best = from_le(fp.priority_raw);
            out.planar_fog = Section::PlanarFog{
                .plane =
                    {from_le(fp.plane[0]),
                     from_le(fp.plane[1]),
                     from_le(fp.plane[2]),
                     from_le(fp.plane[3])},
                .color =
                    {from_le(fog->color.x),
                     from_le(fog->color.y),
                     from_le(fog->color.z),
                     from_le(fog->maximum_density)},
                .opaque = {from_le(fog->opaque_distance), from_le(fog->opaque_depth)},
            };
            cDebug(
                "H2: planar fog {}: plane ({:.2f}, {:.2f}, {:.2f}; {:.1f}) colour "
                "({:.2f}, {:.2f}, {:.2f}) density {} opaque {} into, {} below",
                map.name_of(*tag),
                out.planar_fog.plane.x,
                out.planar_fog.plane.y,
                out.planar_fog.plane.z,
                out.planar_fog.plane.w,
                out.planar_fog.color.r,
                out.planar_fog.color.g,
                out.planar_fog.color.b,
                out.planar_fog.color.a,
                out.planar_fog.opaque.x,
                out.planar_fog.opaque.y);
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
                auto sidx = inst.section.value();
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
            auto const* vp = sw.trigger_volume.resolve(volumes.value());
            if(!vp)
                continue;
            auto const& v = *vp;
            Vecf3       a{from_le(v.position[0]), from_le(v.position[1]), from_le(v.position[2])};
            Vecf3       e{from_le(v.extents[0]), from_le(v.extents[1]), from_le(v.extents[2])};
            m_scene.switches.push_back(SwitchTrigger{
                .lo          = glm::min(a, a + e),
                .hi          = glm::max(a, a + e),
                .source      = sw.source_bsp.value(),
                .destination = sw.destination_bsp.value(),
            });
        }
        cDebug("H2: {} bsp switch triggers", m_scene.switches.size());
    }

    /* The fog a cluster sits in: the scenario's palette entry it names,
     * else its sky's atmospheric fog */
    template<typename V>
    void resolve_fog(
        dim::map_container<V> const& map,
        dim::scn::scenario_t const&  scnr,
        geo::bsp_cluster_t const&    cluster,
        Section::Cluster&            out)
    {
        if(cluster.atmospheric_fog.value() >= 0)
            if(auto palette = scnr.atmospheric_fog_palette.data(map.magic());
               !palette.has_error() &&
               static_cast<size_t>(cluster.atmospheric_fog.value()) < palette.value().size())
            {
                auto const& f = palette.value()[cluster.atmospheric_fog.value()];
                out.fog       = {
                    from_le(f.color[0]),
                    from_le(f.color[1]),
                    from_le(f.color[2]),
                    from_le(f.maximum_density)};
                out.fog_range = {from_le(f.start_distance), from_le(f.opaque_distance)};
                return;
            }
        /* Interior clusters name no sky (only a visible one): no fog */
        i16 sky_index = cluster.sky.value();
        if(sky_index < 0)
            return;
        auto skies = scnr.skies.data(map.magic());
        if(skies.has_error() || static_cast<size_t>(sky_index) >= skies.value().size())
            return;
        auto const sky = map.template get<dim::obj::sky_t>(skies.value()[sky_index].sky);
        if(!sky)
            return;
        if(auto fogs = sky->atmospheric_fog.data(map.magic());
           !fogs.has_error() && !fogs.value().empty())
        {
            auto const& f = fogs.value()[0];
            out.fog       = {
                from_le(f.color[0]),
                from_le(f.color[1]),
                from_le(f.color[2]),
                from_le(f.maximum_density)};
            out.fog_range = {from_le(f.start_distance), from_le(f.opaque_distance)};
        }
    }

    /* Leaf -> cluster, portals (copied into both clusters they join) and
     * the clusters each instance is listed under */
    void gather_visibility(
        geo::bsp_header_t const& bsp, dim::bsp_magic const& magic, Section& out)
    {
        if(auto coll = bsp.collision.data(magic);
           !coll.has_error() && !coll.value().empty())
        {
            auto nodes  = coll.value()[0].bsp3d_nodes.data(magic);
            auto planes = coll.value()[0].planes.data(magic);
            if(!nodes.has_error() && !planes.has_error())
            {
                out.nodes.assign(nodes.value().begin(), nodes.value().end());
                out.planes.assign(planes.value().begin(), planes.value().end());
            }
        }
        if(auto leaves = bsp.leaves.data(magic); !leaves.has_error())
            for(auto const& l : leaves.value())
                out.leaf_cluster.push_back(l.cluster.value());
        auto portals  = bsp.portals.data(magic);
        auto clusters = bsp.clusters.data(magic);
        if(clusters.has_error())
            return;
        auto instances = bsp.instances.data(magic);
        if(!instances.has_error())
            out.instance_clusters.resize(instances.value().size());
        for(auto const& [ci, c] : stl_types::enumerate(clusters.value()))
        {
            Section::Cluster cluster{
                .lo = {from_le(c.x_min), from_le(c.y_min), from_le(c.z_min)},
                .hi = {from_le(c.x_max), from_le(c.y_max), from_le(c.z_max)},
            };
            if(auto refs = c.portals.data(magic);
               !refs.has_error() && !portals.has_error())
                for(auto const& ref : refs.value())
                {
                    i16 pi = ref.value();
                    if(pi < 0 ||
                       static_cast<size_t>(pi) >= portals.value().size())
                        continue;
                    auto const& p = portals.value()[pi];
                    Section::Portal portal{
                        .front = p.front_cluster(), .back = p.back_cluster()};
                    if(auto verts = p.vertices.data(magic); !verts.has_error())
                        for(auto const& v : verts.value())
                            portal.vertices.push_back(Vecf3{
                                from_le(v.point[0]),
                                from_le(v.point[1]),
                                from_le(v.point[2])});
                    cluster.portals.push_back(std::move(portal));
                }
            if(auto refs = c.instance_indices.data(magic); !refs.has_error())
                for(auto const& ref : refs.value())
                {
                    i16 ii = ref.value();
                    if(ii >= 0 &&
                       static_cast<size_t>(ii) < out.instance_clusters.size())
                        out.instance_clusters[ii].push_back(
                            static_cast<i16>(ci));
                }
            out.clusters.push_back(std::move(cluster));
        }
        u32 listed = 0;
        for(auto const& ic : out.instance_clusters)
            listed += !ic.empty();
        cDebug(
            "H2: visibility: {} leaves, {} clusters, {} portals, {} of {} "
            "instances listed",
            out.leaf_cluster.size(),
            out.clusters.size(),
            portals.has_error() ? 0 : portals.value().size(),
            listed,
            out.instance_clusters.size());
    }

    /* The cluster an object stands in. Placed objects name their origin
     * BSP; others get the smallest section holding them. Objects planted
     * into the ground sit in solid space, so a point above is tried too. */
    DrawTag object_tag(Vecf3 const& pos, i32 bsp, i32 origin_bsp)
    {
        DrawTag tag{.section = bsp, .placed = true};
        i32     section = bsp >= 0 ? bsp : origin_bsp;
        if(section < 0 || static_cast<size_t>(section) >= m_scene.sections.size())
        {
            f32 best_volume = 0.f;
            for(size_t i = 0; i < m_scene.sections.size(); i++)
            {
                auto const& sec = m_scene.sections[i];
                if(glm::any(glm::lessThan(pos, sec.lo)) ||
                   glm::any(glm::greaterThan(pos, sec.hi)))
                    continue;
                Vecf3 size   = sec.hi - sec.lo;
                f32   volume = size.x * size.y * size.z;
                if(section < 0 || volume < best_volume)
                    section = static_cast<i32>(i), best_volume = volume;
            }
        }
        m_objects_total++;
        if(section < 0 || static_cast<size_t>(section) >= m_scene.sections.size())
            return tag;
        auto const& sec = m_scene.sections[section];
        for(f32 up : {0.f, 0.3f, 1.f})
            if(auto c = sec.cluster_of(pos + Vecf3{0.f, 0.f, up}))
            {
                tag.cluster_section = section;
                tag.cluster         = static_cast<i32>(*c);
                m_objects_clustered++;
                break;
            }
        return tag;
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
    template<typename Section, blam::heap_t Heap>
    u32 append_section(
        dim::raw_pool const&             pool,
        dim::magic_ptr_base<Heap> const& magic,
        Section const&                   section,
        Matf4 const&               transform,
        geo::model_bounds_t const* bounds,
        std::vector<i32> const&    materials,
        bool                             sky,
        LightmapImage const*             lightmap = nullptr,
        DrawTag const&                   tag      = {},
        ExternalLighting const&          external = {})
    {
        u16 vertex_count = section.vertex_count();
        i32 data_size    = section.data_size();
        if(!vertex_count || data_size <= 0)
            return 0;
        void const* owner = &section;
        if constexpr(requires { section.owner(); })
            owner = section.owner();
        auto block =
            pool.resolve(magic, owner, section.data, static_cast<u32>(data_size));
        if(!block)
        {
            if(m_model_fail_logged++ < 3)
                cDebug(
                    "H2: section raw {}+{:#x} ({} bytes) unresolved",
                    static_cast<u32>(section.data.file()),
                    section.data.offset(),
                    data_size);
            return 0;
        }
        auto resources = section.resources.data(magic);
        if(resources.has_error())
            return 0;
        auto streams = geo::find_streams(resources.value());
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
        constexpr auto expand = &geo::dequantize;

        u32  vbase   = static_cast<u32>(m_vertices.size());
        u32  pstride = streams.position->size() / vertex_count;
        bool has_uv  = in_block(streams.texcoord);
        u32  ustride = has_uv ? streams.texcoord->size() / vertex_count : 0;
        bool has_n   = in_block(streams.normal);
        u32  nstride = has_n ? streams.normal->size() / vertex_count : 0;
        /* Model streams are compressed against the bounds unless their
         * buffer header says floats */
        bool float_pos = !bounds, float_uv = !bounds;
        auto pos_type  = geo::vertex_type_t::point_compressed;
        if(in_block(streams.buffers, 2 * sizeof(geo::vertex_buffer_t)))
        {
            auto const* vb = reinterpret_cast<geo::vertex_buffer_t const*>(
                bytes + base + streams.buffers->offset());
            float_pos = vb[0].type == geo::vertex_type_t::point;
            float_uv  = vb[1].type == geo::vertex_type_t::texcoord;
            pos_type  = vb[0].type;
        }
        /* Boned positions name local nodes; the map makes them model nodes
         * (no map = they are model nodes already) */
        Span<const u8> node_map;
        if(in_block(streams.node_map))
            node_map = Span<const u8>(
                bytes + base + streams.node_map->offset(), streams.node_map->size());
        auto model_node = [&](u8 local) {
            return local < node_map.size() ? node_map[local] : local;
        };
        if(pstride < (float_pos ? 12u : 6u) || (!float_pos && !bounds))
            return 0;
        /* 4 = packed normal; 12 = packed normal, binormal, tangent; 36 =
         * the same three as floats (Vista) */
        if(!m_normal_order_logged && has_n && nstride != 4 && nstride != 12 &&
           nstride != 36)
        {
            cDebug("H2: unhandled normal stream stride {}", nstride);
            m_normal_order_logged = true;
        }
        bool const external_lm =
            lightmap && external.lightmap_uv.size() >= vertex_count * 2u;
        bool const external_rgb = external.rgb.size() >= vertex_count * 3u;
        bool has_lm = lightmap && !external_lm && in_block(streams.lightmap) &&
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
            if(!float_pos)
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
            /* Node and weight bytes follow the i16 position */
            {
                auto set = [&](u32 count, u32 nodes_at, u32 weights_at) {
                    if(std::max(nodes_at, weights_at) + count > pstride)
                        return;
                    for(u32 i = 0; i < count && i < 4; i++)
                    {
                        out.nodes[i]   = model_node(p[nodes_at + i]);
                        out.weights[i] = weights_at ? p[weights_at + i] : 255;
                    }
                };
                switch(pos_type)
                {
                case geo::vertex_type_t::rigid_boned:
                    set(1, 6, 0);
                    break;
                case geo::vertex_type_t::skinned_2:
                    set(2, 8, 10);
                    break;
                case geo::vertex_type_t::skinned_3:
                    set(3, 6, 9);
                    break;
                case geo::vertex_type_t::skinned_4:
                    set(4, 8, 12);
                    break;
                default:
                    break;
                }
            }
            /* Water has positions only; its maps tile across the world */
            if constexpr(std::is_same_v<Section, WaterSection>)
                out.texcoord = Vecf2(out.position);
            if(has_uv)
            {
                auto const* t = bytes + base + streams.texcoord->offset() + v * ustride;
                if(!float_uv && bounds && ustride >= 4)
                {
                    i16 q[2];
                    std::memcpy(q, t, 4);
                    out.texcoord = {
                        expand(from_le(q[0]), from_le(bounds->u_min), from_le(bounds->u_max)),
                        expand(from_le(q[1]), from_le(bounds->v_min), from_le(bounds->v_max))};
                } else if(float_uv && ustride >= 8)
                {
                    f32 uv[2];
                    std::memcpy(uv, t, 8);
                    out.texcoord = {from_le(uv[0]), from_le(uv[1])};
                }
            }
            /* Sky domes put their sun-lit side by the sky's light only
             * with v running the other way */
            if(sky)
                out.texcoord.y = 1.f - out.texcoord.y;
            if(has_n && nstride == 36)
            {
                f32 n[3];
                std::memcpy(n, bytes + base + streams.normal->offset() + v * nstride, 12);
                Vecf3 normal{from_le(n[0]), from_le(n[1]), from_le(n[2])};
                normals_a.push_back(glm::normalize(rotation * normal));
                normals_b.push_back(normals_a.back());
            } else if(has_n && (nstride == 4 || nstride == 12))
            {
                u32 packed;
                std::memcpy(
                    &packed,
                    bytes + base + streams.normal->offset() + v * nstride,
                    4);
                packed = from_le(packed);
                /* Both field orders the Xbox formats use; which one a map
                 * uses is decided against the face normals */
                normals_a.push_back(glm::normalize(rotation * blam::vertex_pack::unpack_11_11_10(packed)));
                normals_b.push_back(glm::normalize(rotation * blam::vertex_pack::unpack_10_11_11(packed)));
            }
            if(external_lm)
            {
                i16   q0 = from_le(external.lightmap_uv[v * 2]);
                i16   q1 = from_le(external.lightmap_uv[v * 2 + 1]);
                Vecf2 uv;
                if(external.bounds)
                    uv = {
                        expand(
                            q0,
                            from_le(external.bounds->u2_min),
                            from_le(external.bounds->u2_max)),
                        expand(
                            q1,
                            from_le(external.bounds->v2_min),
                            from_le(external.bounds->v2_max))};
                else
                    uv = {(q0 / 32767.f + 1.f) * 0.5f, (q1 / 32767.f + 1.f) * 0.5f};
                out.lightmap = Vecf3(
                    uv * lm_scale + lm_offset, static_cast<f32>(lightmap->layer));
            } else if(has_lm)
            {
                i16 q[2];
                std::memcpy(
                    q, bytes + base + streams.lightmap->offset() + v * 4, 4);
                /* SHORT2N: 0..32767 covers the chart */
                Vecf2 uv{from_le(q[0]) / 32767.f, from_le(q[1]) / 32767.f};
                out.lightmap = Vecf3(
                    uv * lm_scale + lm_offset, static_cast<f32>(lightmap->layer));
            }
            if(external_rgb)
            {
                for(u32 c = 0; c < 3; c++)
                    out.light[c] = external.rgb[v * 3 + c];
                out.light[3] = 255;
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
        i32 fallback = static_cast<i32>(m_scene.materials.size()) - 1;
        std::unordered_map<i32, std::vector<u16>> tris;
        /* A mirroring transform turns the winding around; keep fronts front */
        bool const mirrored = glm::determinant(glm::mat3(transform)) < 0.f;
        auto emit = [&](std::vector<u16>& out, Span<const u16> range) {
            auto tri = [&](u16 a, u16 b, u16 c) {
                if(mirrored)
                    out.insert(out.end(), {b, a, c});
                else
                    out.insert(out.end(), {a, b, c});
            };
            if(!strip)
            {
                for(size_t i = 0; i + 2 < range.size(); i += 3)
                    tri(range[i], range[i + 1], range[i + 2]);
                return;
            }
            for(size_t i = 2; i < range.size(); i++)
            {
                u16 a = range[i - 2], b = range[i - 1], c = range[i];
                if(a == b || b == c || a == c)
                    continue;
                if(i % 2)
                    tri(b, a, c);
                else
                    tri(a, b, c);
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
                i16 shader = subs[s].shader.value();
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
        {
            /* Water has one shader and no submesh table */
            i32 mat = fallback;
            if constexpr(std::is_same_v<Section, WaterSection>)
                if(!materials.empty())
                    mat = materials[0];
            emit(tris[mat], index_span);
        }

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

        if(lightmap && lm_scale.x > 0.f && lm_scale.y > 0.f)
            for(auto const& [mat, list] : tris)
                for(size_t i = 0; i + 2 < list.size(); i += 3)
                {
                    LightTriangle t{.image = lightmap};
                    for(size_t c = 0; c < 3; c++)
                    {
                        auto const& v = m_vertices[vbase + list[i + c]];
                        t.position[c] = v.position;
                        t.uv[c] = (Vecf2(v.lightmap) - lm_offset) / lm_scale;
                    }
                    m_light_triangles.add(t, t.position);
                }

        u32 emitted = 0;
        for(auto& [mat, list] : tris)
        {
            if(list.empty() || mat < 0 ||
               static_cast<size_t>(mat) >= m_scene.materials.size())
                continue;
            Vecf3 lo(std::numeric_limits<f32>::max()), hi(std::numeric_limits<f32>::lowest());
            for(u16 idx : list)
            {
                lo = glm::min(lo, m_vertices[vbase + idx].position);
                hi = glm::max(hi, m_vertices[vbase + idx].position);
            }
            m_scene.materials[mat].section_draws.push_back({
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
                .tag    = tag,
                .bounds = Vecf4((lo + hi) * .5f, glm::distance(lo, hi) * .5f),
            });
            m_indices.insert(m_indices.end(), list.begin(), list.end());
            emitted++;
        }
        return emitted ? 1 : 0;
    }

    /* A lightmapped BSP triangle: objects without baked light take the
     * lightmap under them, as the engine does */
    struct LightTriangle
    {
        std::array<Vecf3, 3> position;
        std::array<Vecf2, 3> uv; /*!< Within the image */
        LightmapImage const* image{nullptr};
    };

    /* The lightmap colour of the highest surface under a point */
    std::optional<Vecf3> light_below(Vecf3 const& at) const
    {
        f32 const  top  = at.z + 0.25f;
        f32        best = -std::numeric_limits<f32>::max();
        Vecf3      out{};
        bool       hit = false;
        m_light_triangles.for_each_at(at, [&](LightTriangle const& t) {
            Vecf2 const a  = Vecf2(t.position[0]);
            Vecf2 const e1 = Vecf2(t.position[1]) - a, e2 = Vecf2(t.position[2]) - a;
            f32 const   det = e1.x * e2.y - e2.x * e1.y;
            if(std::abs(det) < 1e-6f)
                return;
            Vecf2 const d = Vecf2(at) - a;
            f32 const   u = (d.x * e2.y - e2.x * d.y) / det;
            f32 const   v = (e1.x * d.y - d.x * e1.y) / det;
            if(u < 0.f || v < 0.f || u + v > 1.f)
                return;
            f32 const w = 1.f - u - v;
            f32 const z = w * t.position[0].z + u * t.position[1].z + v * t.position[2].z;
            if(z > top || z <= best)
                return;
            Vecf2 const uv  = w * t.uv[0] + u * t.uv[1] + v * t.uv[2];
            auto const& img = *t.image;
            if(img.rgba.empty())
                return;
            u32 const px = std::min(
                static_cast<u32>(std::clamp(uv.x, 0.f, 1.f) * img.width), img.width - 1);
            u32 const py = std::min(
                static_cast<u32>(std::clamp(uv.y, 0.f, 1.f) * img.height), img.height - 1);
            u8 const* c = img.rgba.data() + (py * img.width + px) * 4;
            out  = Vecf3{c[0], c[1], c[2]} / 255.f;
            best = z;
            hit  = true;
        });
        return hit ? std::optional<Vecf3>(out) : std::nullopt;
    }

    /* Vertices from `first` on take the lightmap under `at`, shaded by their
     * normals so the form survives */
    void light_from_below(size_t first, Vecf3 const& at)
    {
        auto light = light_below(at);
        if(!light)
        {
            m_unlit_objects++;
            return;
        }
        m_below_lit_objects++;
        Vecf3 const key = glm::normalize(Vecf3{0.3f, 0.2f, 1.f});
        for(size_t i = first; i < m_vertices.size(); i++)
        {
            auto&     v     = m_vertices[i];
            f32 const shade = 0.55f + 0.6f * std::max(glm::dot(v.normal, key), 0.f);
            for(u32 c = 0; c < 3; c++)
                v.light[c] = static_cast<u8>(
                    std::clamp((*light)[c] * shade * 255.f, 0.f, 255.f));
            v.light[3] = 255;
        }
    }

    /* The globals' player representation (hands, body) and the starting
     * weapon's view model, idling */
    template<typename V>
    void load_first_person(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::scn::scenario_t const&  scenario)
    {
        auto const* globals_tag = map.globals();
        auto const* globals =
            globals_tag ? globals_tag->template data<dim::obj::matg::globals_t>(map.magic())
                        : nullptr;
        if(!globals)
            return;
        auto reps = globals->player_representation.data(map.magic());
        if(reps.has_error())
            return;
        dim::obj::matg::player_representation_t const* rep = nullptr;
        for(auto const& r : reps.value())
            if(map.tag_at(r.first_person_hands.tag_id))
            {
                rep = &r;
                break;
            }
        if(!rep)
            return;

        /* The view model and its animations from the starting weapon */
        dim::tag_t const* weapon_tag = nullptr;
        if(auto profiles = scenario.player_starting_profiles.data(map.magic());
           !profiles.has_error() && !profiles.value().empty())
            weapon_tag = map.tag_at(profiles.value()[0].primary_weapon.tag_id);
        if(auto palette = scenario.weapon_palette.data(map.magic());
           !weapon_tag && !palette.has_error() && !palette.value().empty())
            weapon_tag = map.tag_at(palette.value()[0].tag.tag_id);
        /* Multiplayer hands out weapons by game type: the battle rifle */
        for(auto const& tag : map.tags())
            if(!weapon_tag && tag.matches(blam::tag_class_t::weap) &&
               map.name_of(tag).find("battle_rifle") != std::string_view::npos)
                weapon_tag = &tag;
        auto const* weapon =
            weapon_tag ? weapon_tag->template data<dim::obj::weapon_t>(map.magic()) : nullptr;
        dim::tag_t const* jmad_tag = nullptr;
        dim::tag_t const* view_tag = nullptr;
        if(weapon)
            if(auto views = weapon->first_person.data(map.magic());
               !views.has_error() && !views.value().empty())
            {
                jmad_tag = map.tag_at(views.value()[0].animations.tag_id);
                view_tag = map.tag_at(views.value()[0].model.tag_id);
            }

        auto add = [&](dim::tag_t const* mode_tag, std::optional<u32> animation) {
            if(!mode_tag || !mode_tag->matches(blam::tag_class_t::mode))
                return -1;
            if(!animation)
                animation = rest_pose(map, *mode_tag);
            if(!animation)
                return -1;
            i32 const index = static_cast<i32>(m_scene.animated_objects.size());
            DrawTag   tag{.animated = index, .first_person = true};
            if(append_model(map, pool, *mode_tag, Matf4(1.f), false, tag) == 0)
                return -1;
            m_scene.animated_objects.push_back({.animation = *animation});
            return index;
        };
        auto const* hands_tag = map.tag_at(rep->first_person_hands.tag_id);
        auto const* body_tag  = map.tag_at(rep->first_person_body.tag_id);
        auto&       fp        = m_scene.first_person;
        fp.hands  = add(
            hands_tag,
            jmad_tag ? view_animation(map, pool, *jmad_tag, *hands_tag, "first_person:idle")
                     : std::nullopt);
        if(view_tag && jmad_tag)
            fp.weapon =
                add(view_tag, view_animation(map, pool, *jmad_tag, *view_tag, "first_person:idle"));

        /* The body stands as the third-person unit does holding the weapon */
        std::optional<u32> stance;
        auto const         unit = map.template get<dim::obj::biped_t>(rep->third_person_unit);
        auto const         unit_view = dim::object_view::of(map, rep->third_person_unit);
        auto const*        graph_tag = unit_view.animation.tag;
        std::string weapon_class =
            weapon ? std::string(map.string(weapon->weapon_class)) : std::string{};
        if(graph_tag && body_tag)
            for(auto const& clip : {"combat:" + weapon_class + ":idle", std::string{"combat:rifle:idle"}})
                if(!stance)
                    stance = view_animation(map, pool, *graph_tag, *body_tag, clip);
        if(stance)
            drop_arms(map, *body_tag, m_scene.animations[*stance]);
        fp.body = add(body_tag, stance);
        if(unit && stance)
            fp.eye = camera_marker(
                map,
                *unit,
                unit_view.render_model.tag,
                *body_tag,
                m_scene.animations[*stance]);
        cDebug(
            "H2: first person: hands {} weapon {} ({}) body {} eye ({:.3f}, {:.3f}, {:.3f})",
            hands_tag ? map.name_of(*hands_tag) : "-",
            view_tag ? map.name_of(*view_tag) : "-",
            weapon_tag ? map.name_of(*weapon_tag) : "-",
            body_tag ? map.name_of(*body_tag) : "-",
            fp.eye.x,
            fp.eye.y,
            fp.eye.z);
    }

    /* One of a graph's clips on a model of another: the graph's skeleton
     * posed, each model node following the skeleton node of its name */
    template<typename V>
    std::optional<u32> view_animation(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::tag_t const&            jmad_tag,
        dim::tag_t const&            mode_tag,
        std::string_view             clip)
    {
        auto const* graph = jmad_tag.template data<dim::anim::header_t>(map.magic());
        auto const* model = mode_tag.template data<geo::model_header_t>(map.magic());
        if(!graph || !model)
            return std::nullopt;
        auto anims    = graph->animations.data(map.magic());
        auto groups   = graph->resource_groups.data(map.magic());
        auto skeleton = graph->skeleton.data(map.magic());
        auto data     = graph->node_data.data(map.magic());
        auto nodes    = model->nodes.data(map.magic());
        if(anims.has_error() || groups.has_error() || skeleton.has_error() ||
           data.has_error() || nodes.has_error() || nodes.value().size() > 64)
            return std::nullopt;
        auto const*  chosen = dim::find_animation(map, *graph, clip);
        size_t const joints = skeleton.value().size();
        if(!chosen || chosen->node_count != joints)
            return std::nullopt;
        /* The model's node of that name at rest, else the graph's own */
        auto       defaults = dim::graph_pose(map, *graph, joints);
        auto const rest     = dim::model_pose(map, *model);
        for(size_t i = 0; i < joints; i++)
            for(size_t n = 0; n < nodes.value().size(); n++)
                if(map.string(nodes.value()[n].name) == map.string(skeleton.value()[i].name))
                    defaults[i] = rest[n];
        auto decoded = dim::decode_animation(map, pool, *graph, *chosen, defaults);
        if(!decoded)
            return std::nullopt;

        /* Model node -> skeleton node by name */
        size_t const     count = nodes.value().size();
        std::vector<i32> joint(count, -1);
        for(size_t n = 0; n < count; n++)
            for(size_t j = 0; j < joints; j++)
                if(map.string(nodes.value()[n].name) == map.string(skeleton.value()[j].name))
                    joint[n] = static_cast<i32>(j);
        auto const           inverse = dim::inverse_bind(map, *model);
        H2Scene::animation_t out{.frames = decoded->frames, .nodes = static_cast<u32>(count)};
        out.palette.resize(static_cast<size_t>(out.frames) * count);
        std::vector<Matf4> world(joints);
        for(u32 f = 0; f < out.frames; f++)
        {
            for(size_t j = 0; j < joints; j++)
            {
                auto const& pose  = decoded->at(f, static_cast<u32>(j));
                Matf4 const local = glm::translate(Matf4(1.f), pose.translation) *
                                    glm::mat4_cast(pose.rotation) *
                                    glm::scale(Matf4(1.f), Vecf3(pose.scale));
                i16 parent = skeleton.value()[j].parent.value();
                world[j]   = parent >= 0 && static_cast<size_t>(parent) < j
                                 ? world[parent] * local
                                 : local;
            }
            for(size_t n = 0; n < count; n++)
                out.palette[f * count + n] =
                    joint[n] >= 0 ? world[joint[n]] * inverse[n] : Matf4(1.f);
        }
        cDebug(
            "H2: {} {} on {}: {} frames",
            map.name_of(jmad_tag),
            clip,
            map.name_of(mode_tag),
            out.frames);
        auto index = static_cast<u32>(m_scene.animations.size());
        m_scene.animations.push_back(std::move(out));
        return index;
    }

    /* The body's arms reach for the gun under the eye, where the view
     * model's are: each upper arm and what hangs from it shrinks into its
     * shoulder */
    template<typename V>
    static void drop_arms(
        dim::map_container<V> const& map, dim::tag_t const& mode_tag, H2Scene::animation_t& anim)
    {
        auto const* model = mode_tag.template data<geo::model_header_t>(map.magic());
        if(!model)
            return;
        auto nodes = model->nodes.data(map.magic());
        if(nodes.has_error() || nodes.value().size() != anim.nodes)
            return;
        std::vector<Matf4> const bind = dim::rest_frames(map, *model);
        /* Each node's upper arm, if it is one or hangs from one */
        std::vector<i32> arm(anim.nodes, -1);
        for(size_t n = 0; n < anim.nodes; n++)
        {
            auto const name = map.string(nodes.value()[n].name);
            i16 const  parent = nodes.value()[n].parent.value();
            if(name == "l_upperarm" || name == "r_upperarm")
                arm[n] = static_cast<i32>(n);
            else if(parent >= 0 && static_cast<size_t>(parent) < n)
                arm[n] = arm[parent];
        }
        for(u32 f = 0; f < anim.frames; f++)
        {
            Matf4* frame = &anim.palette[static_cast<size_t>(f) * anim.nodes];
            for(size_t n = 0; n < anim.nodes; n++)
                if(arm[n] >= 0)
                {
                    auto const  a        = static_cast<size_t>(arm[n]);
                    Vecf3 const shoulder = Vecf3(frame[a] * bind[a][3]);
                    frame[n] = glm::translate(Matf4(1.f), shoulder) * glm::scale(Matf4(1.f), Vecf3(0.f));
                }
        }
    }

    /* Where the unit's camera marker sits on the posed body, body space;
     * the marker lives on the unit's own model, matched to the body by node
     * name */
    template<typename V>
    static Vecf3 camera_marker(
        dim::map_container<V> const& map,
        dim::obj::biped_t const&     unit,
        dim::tag_t const*            unit_mode_tag,
        dim::tag_t const&            body_tag,
        H2Scene::animation_t const&  anim)
    {
        Vecf3 const fallback{0.f, 0.f, from_le(unit.standing_camera_height)};
        auto const* unit_model =
            unit_mode_tag ? unit_mode_tag->template data<geo::model_header_t>(map.magic())
                          : nullptr;
        auto const* body = body_tag.template data<geo::model_header_t>(map.magic());
        if(!unit_model || !body)
            return fallback;
        auto groups     = unit_model->marker_groups.data(map.magic());
        auto unit_nodes = unit_model->nodes.data(map.magic());
        auto body_nodes = body->nodes.data(map.magic());
        if(groups.has_error() || unit_nodes.has_error() || body_nodes.has_error())
            return fallback;
        std::string_view const wanted = map.string(unit.camera_marker_name);
        for(auto const& group : groups.value())
        {
            if(map.string(group.name) != (wanted.empty() ? "head" : wanted))
                continue;
            auto markers = group.markers.data(map.magic());
            if(markers.has_error() || markers.value().empty())
                return fallback;
            auto const& marker = markers.value()[0];
            auto const  node   = static_cast<size_t>(marker.node);
            if(marker.node < 0 || node >= unit_nodes.value().size())
                return fallback;
            std::vector<Matf4> const bind = dim::rest_frames(map, *body);
            for(size_t n = 0; n < body_nodes.value().size() && n < anim.nodes; n++)
                if(map.string(body_nodes.value()[n].name) == map.string(unit_nodes.value()[node].name))
                    return Vecf3(
                        anim.palette[n] * bind[n] *
                        Vecf4(
                            from_le(marker.translation[0]),
                            from_le(marker.translation[1]),
                            from_le(marker.translation[2]),
                            1.f));
        }
        return fallback;
    }

    /* A skinned model standing in its bind pose */
    template<typename V>
    std::optional<u32> rest_pose(dim::map_container<V> const& map, dim::tag_t const& mode_tag)
    {
        auto const* model = mode_tag.template data<geo::model_header_t>(map.magic());
        if(!model)
            return std::nullopt;
        auto nodes = model->nodes.data(map.magic());
        if(nodes.has_error() || nodes.value().size() > 64)
            return std::nullopt;
        auto const count = static_cast<u32>(nodes.value().size());
        auto       index = static_cast<u32>(m_scene.animations.size());
        m_scene.animations.push_back(
            {.frames = 1, .nodes = count, .palette = std::vector<Matf4>(count, Matf4(1.f))});
        return index;
    }

    /* An object's collision model (hlmt -> coll), permutation 0 of every
     * region, into the collision mesh of the section(s) it is placed in.
     * The coll nodes mirror the render model's, which carry the transforms. */
    template<typename V>
    u32 append_object_collision(
        dim::map_container<V> const& map,
        dim::tag_reference<> const&           object,
        Matf4 const&                        transform,
        i32                                 section,
        std::unordered_map<u32, i16> const* permutations = nullptr)
    {
        auto const view = dim::object_view::of(map, object);
        auto const& coll = view.collision_model;
        if(!coll)
            return 0;
        std::vector<Matf4> node_world;
        if(view.render_model)
            node_world = dim::rest_frames(map, *view.render_model);
        auto regions = coll->regions.data(map.magic());
        if(regions.has_error())
            return 0;
        u32 triangles = 0;
        for(auto const& region : regions.value())
        {
            auto const* perm = dim::picked_permutation(map, region, permutations);
            if(!perm)
                continue;
            auto bsps = perm->bsps.data(map.magic());
            if(bsps.has_error())
                continue;
            for(auto const& ref : bsps.value())
            {
                Matf4 local = transform;
                i16   node  = ref.node.value();
                if(node >= 0 && static_cast<size_t>(node) < node_world.size())
                    local = transform * node_world[node];
                for(size_t i = 0; i < m_scene.sections.size(); i++)
                {
                    if(section >= 0 && static_cast<i32>(i) != section)
                        continue;
                    auto& mesh  = *m_scene.sections[i].collision;
                    size_t before = mesh.indices.size();
                    append_collision(ref.bsp, map.magic(), local, mesh);
                    if(static_cast<i32>(i) == section || section < 0)
                        triangles += (mesh.indices.size() - before) / 3;
                }
            }
        }
        return triangles;
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
        DrawTag const&               tag          = {},
        SceneryLight const*          lighting     = nullptr,
        LightBucket const*           bucket       = nullptr,
        std::unordered_map<u32, i16> const* permutations = nullptr)
    {
        auto const* model =
            mode_tag.template data<geo::model_header_t>(map.magic());
        if(!model)
        {
            if(m_model_fail_logged++ < 3)
                cDebug("H2: {}: model body unresolved", map.name_of(mode_tag));
            return 0;
        }
        auto bounds   = model->bounds.data(map.magic());
        auto sections = model->sections.data(map.magic());
        auto regions  = model->regions.data(map.magic());
        if((bounds.has_error() || bounds.value().empty() ||
            sections.has_error() || regions.has_error()) &&
           m_model_fail_logged++ < 3)
            cDebug(
                "H2: {}: bounds {} sections {} regions {}",
                map.name_of(mode_tag),
                bounds.has_error() ? "error" : std::to_string(bounds.value().size()),
                sections.has_error() ? "error" : std::to_string(sections.value().size()),
                regions.has_error() ? "error" : std::to_string(regions.value().size()));
        if(bounds.has_error() || bounds.value().empty() ||
           sections.has_error() || regions.has_error())
            return 0;

        std::vector<i32> materials;
        if(auto shader_refs = model->shaders.data(map.magic());
           !shader_refs.has_error())
            for(auto const& sref : shader_refs.value())
                materials.push_back(
                    material_for(map, pool, sref.shader.tag_id, sky));

        u32 emitted = 0;
        for(auto const& region : regions.value())
        {
            auto const* perm = dim::picked_permutation(map, region, permutations);
            if(!perm)
                continue;
            i16 sidx = perm->best_section();
            if(sidx < 0 || static_cast<size_t>(sidx) >= sections.value().size())
                continue;
            auto const& section = sections.value()[sidx];
            /* Rigid sections are stored in model space like the rest; their
             * node only says what moves them */
            Matf4 const local = transform;
            i16         node  = section.rigid_node.value();
            /* Baked per-vertex light, one bucket range per model section */
            ExternalLighting external;
            if(lighting && bucket && !lighting->section_offsets.empty())
            {
                size_t slot = std::min<size_t>(
                    static_cast<size_t>(sidx), lighting->section_offsets.size() - 1);
                u32 vo    = static_cast<u32>(std::max<i16>(0, lighting->section_offsets[slot]));
                u32 count = section.vertex_count();
                if(bucket->rgb.size() >= (vo + count) * 3)
                    external.rgb = Span<u8 const>(bucket->rgb.data() + vo * 3, count * 3);
            }
            size_t const first = m_vertices.size();
            emitted += append_section(
                pool,
                map.magic(),
                section,
                local,
                &bounds.value()[0],
                materials,
                sky,
                nullptr,
                tag,
                external);
            /* Rigid sections ride their node whole */
            if(node >= 0 && node < 256)
                for(size_t v = first; v < m_vertices.size(); v++)
                {
                    m_vertices[v].nodes[0]   = static_cast<u8>(node);
                    m_vertices[v].weights[0] = 255;
                }
        }
        return emitted;
    }

    /* Decodes every cluster's lightmap (P8 image through its palette) into
     * the atlas; returns one entry per cluster, null where there is none */
    template<typename V>
    std::vector<LightmapImage const*> load_lightmaps(
        dim::map_container<V> const&                            map,
        dim::raw_pool const&                                    pool,
        dim::ltmp::group_t const&                               group,
        dim::tag_block<dim::ltmp::render_info_t, blam::heap_t::bsp> const& render_info,
        dim::bsp_magic const&                                   magic)
    {
        std::vector<LightmapImage const*> out;
        auto infos    = render_info.data(magic);
        auto palettes = group.section_palette.data(magic);
        if(infos.has_error() || palettes.has_error())
            return out;
        auto const bitm = map.template get<dim::bitm::header_t>(group.bitmap_group);
        if(!bitm)
            return out;
        auto images = bitm->images.data(map.magic());
        if(images.has_error())
            return out;

        std::unordered_map<u32, size_t> decoded; /* (image, palette) -> slot */
        u32                             failed = 0;
        for(auto const& info : infos.value())
        {
            i16 image_idx = info.bitmap.value();
            i8  pal_idx   = info.palette.value();
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
            "H2: lightmaps: {} sections, {} images decoded, {} failed, {} "
            "atlas pages",
            out.size(),
            decoded.size(),
            failed,
            m_lightmap_atlas.pages.size());
        return out;
    }

    /* The buckets' (0,0) lightmap texcoord and (0,1) colour streams */
    static std::vector<LightBucket> load_buckets(
        dim::raw_pool const&      pool,
        dim::ltmp::group_t const& group,
        dim::bsp_magic const&     magic)
    {
        std::vector<LightBucket> out;
        auto                     buckets = group.geometry_buckets.data(magic);
        if(buckets.has_error())
            return out;
        for(auto const& b : buckets.value())
        {
            LightBucket bucket;
            i32         size = b.data_size();
            auto        block =
                size > 0 ? pool.resolve(b.data, static_cast<u32>(size)) : std::nullopt;
            auto resources = b.resources.data(magic);
            if(block && !resources.has_error())
            {
                auto const* bytes = reinterpret_cast<u8 const*>(block->data());
                i32         base  = b.base_address();
                for(auto const& r : resources.value())
                {
                    if(r.type0() != 0 || r.offset() < 0 || r.size() <= 0 ||
                       static_cast<i64>(base) + r.offset() + r.size() >
                           static_cast<i64>(block->size()))
                        continue;
                    auto const* at = bytes + base + r.offset();
                    if(r.type1() == 0)
                    {
                        bucket.uv.resize(static_cast<size_t>(r.size()) / 2);
                        std::memcpy(bucket.uv.data(), at, bucket.uv.size() * 2);
                    } else if(r.type1() == 1)
                        bucket.rgb.assign(at, at + r.size());
                }
            }
            out.push_back(std::move(bucket));
        }
        return out;
    }

    static std::vector<SceneryLight> load_bucket_refs(
        dim::tag_block<dim::ltmp::bucket_ref_t, blam::heap_t::bsp> const& refs,
        dim::bsp_magic const&                                           magic)
    {
        std::vector<SceneryLight> out;
        auto                      list = refs.data(magic);
        if(list.has_error())
            return out;
        for(auto const& r : list.value())
        {
            SceneryLight light{.bucket = r.bucket.value()};
            if(auto offsets = r.section_offsets.data(magic); !offsets.has_error())
                for(auto const& o : offsets.value())
                    light.section_offsets.push_back(o.value());
            out.push_back(std::move(light));
        }
        return out;
    }

    /* Scenery lighting keyed by the placement's unique id */
    static std::unordered_map<u32, SceneryLight> load_scenery_lights(
        dim::ltmp::group_t const& group, dim::bsp_magic const& magic)
    {
        std::unordered_map<u32, SceneryLight> out;
        auto infos = group.scenery_object_info.data(magic);
        auto refs  = load_bucket_refs(group.scenery_bucket_refs, magic);
        if(infos.has_error())
            return out;
        for(auto const& [i, info] : stl_types::enumerate(infos.value()))
            if(i < refs.size())
                out.emplace(from_le(info.unique_id.raw), refs[i]);
        return out;
    }

    void upload_lightmaps()
    {
        if(m_lightmaps.empty())
            return;
        m_scene.lightmap_tex = std::make_shared<gfx::compat::texture_2da_t>(
            m_api, typing::pixels::PixDesc(typing::pixels::pix_fmt::RGBA8), 1);
        m_scene.lightmap_tex->alloc(
            size_3d<u32>{
                LightmapAtlas::page,
                LightmapAtlas::page,
                static_cast<u32>(m_lightmap_atlas.pages.size())});
        for(auto& image : m_lightmaps)
            m_scene.lightmap_tex->upload(
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
        m_scene.lightmap_sampler = m_scene.lightmap_tex->sampler();
        m_scene.lightmap_sampler->alloc();
        m_scene.lightmap_sampler->set_filtering(
            typing::Filtering::Linear, typing::Filtering::Linear);
    }

    /* An object's looping animation (idle, else device:position) baked to
     * skin matrices per frame; shared by objects of the same tag and model.
     * None when the graph does not fit the model. */
    template<typename V>
    std::optional<u32> object_animation(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::tag_reference<> const&  object,
        dim::tag_t const&            mode_tag)
    {
        auto const  view  = dim::object_view::of(map, object);
        auto const& graph = view.animation;
        auto const* model = mode_tag.template data<geo::model_header_t>(map.magic());
        if(!graph || !model)
            return std::nullopt;
        u64 const key = (static_cast<u64>(view.object.tag->tag_id.raw) << 32) |
                        mode_tag.tag_id.raw;
        if(auto it = m_animation_index.find(key); it != m_animation_index.end())
            return it->second;
        m_animation_index.emplace(key, std::nullopt);

        auto anims  = graph->animations.data(map.magic());
        auto groups = graph->resource_groups.data(map.magic());
        auto nodes  = model->nodes.data(map.magic());
        if(anims.has_error() || groups.has_error() || nodes.has_error())
            return std::nullopt;
        dim::anim::animation_t const* chosen = nullptr;
        for(std::string_view name : {"idle", "device:position"})
            if(!chosen)
                chosen = dim::find_animation(map, *graph, name);
        constexpr size_t max_nodes = 64;
        size_t const     node_count = nodes.value().size();
        if(!chosen || node_count == 0 || node_count > max_nodes ||
           chosen->node_count != node_count)
            return std::nullopt;

        /* The model's own pose for nodes the animation leaves alone */
        auto const defaults = dim::model_pose(map, *model);
        auto decoded = dim::decode_animation(map, pool, *graph, *chosen, defaults);
        if(!decoded)
            return std::nullopt;

        auto const           inverse = dim::inverse_bind(map, *model);
        H2Scene::animation_t out{.frames = decoded->frames, .nodes = static_cast<u32>(node_count)};
        /* A device's position runs 0 to 1 over its transition time */
        if(map.string(chosen->name) == "device:position" && view.is_device())
            if(auto const* device = view.object.tag->template data<dim::obj::device_t>(map.magic());
               device && from_le(device->position_transition_time) > 0.f)
                out.rate = static_cast<f32>(out.frames) * from_le(device->position_transition_time);
        out.palette.resize(static_cast<size_t>(out.frames) * node_count);
        std::vector<Matf4> world(node_count);
        for(u32 f = 0; f < out.frames; f++)
            for(size_t n = 0; n < node_count; n++)
            {
                auto const& pose  = decoded->at(f, static_cast<u32>(n));
                Matf4 const local = glm::translate(Matf4(1.f), pose.translation) *
                                    glm::mat4_cast(pose.rotation) *
                                    glm::scale(Matf4(1.f), Vecf3(pose.scale));
                i16 parent = nodes.value()[n].parent.value();
                world[n]   = parent >= 0 && static_cast<size_t>(parent) < n
                                 ? world[parent] * local
                                 : local;
                out.palette[f * node_count + n] = world[n] * inverse[n];
            }
        cDebug(
            "H2: {}: animation {} ({} frames, {} nodes)",
            map.name_of(*graph.tag),
            map.string(chosen->name),
            out.frames,
            node_count);
        auto index = static_cast<u32>(m_scene.animations.size());
        m_scene.animations.push_back(std::move(out));
        m_animation_index[key] = index;
        return index;
    }

    /* obje -> hlmt -> mode */
    template<typename V>
    dim::tag_t const* render_model_of(
        dim::map_container<V> const& map, dim::tag_reference<> const& object)
    {
        return dim::object_view::of(map, object).render_model.tag;
    }

    /* A BSP's water: its shader on its own geometry, and the tints the sea
     * shades with */
    template<typename V>
    u32 append_water(
        dim::map_container<V> const&   map,
        dim::raw_pool const&           pool,
        dim::bsp_magic const&          magic,
        geo::water_definition_t const& water,
        size_t                         bsp_idx)
    {
        auto resources = water.resources.data(magic);
        if(resources.has_error())
            return 0;
        u32 positions = 0;
        for(auto const& r : resources.value())
            if(r.type0() == 56 && r.type1() == 0)
                positions = static_cast<u32>(std::max(r.size(), 0));
        WaterSection section{
            .water     = &water,
            .data      = water.data,
            .resources = water.resources,
            .vertices  = static_cast<u16>(positions / 12),
        };
        auto rgb = [](Vecf3 const& c) {
            return Vecf3{from_le(c[0]), from_le(c[1]), from_le(c[2])};
        };
        m_scene.water = {
            .sun_spot   = rgb(water.sun_spot_color),
            .sun_power  = from_le(water.sun_specular_power),
            .reflection = rgb(water.reflection_tint),
            .refraction = rgb(water.refraction_tint),
            .horizon    = rgb(water.horizon_color),
        };
        cDebug(
            "H2: water {}: {} vertices, sun spot ({}, {}, {}) x{}, horizon ({}, {}, {})",
            map.name_of(*map.tag_at(water.shader.tag_id)),
            section.vertices,
            m_scene.water.sun_spot.x,
            m_scene.water.sun_spot.y,
            m_scene.water.sun_spot.z,
            m_scene.water.sun_power,
            m_scene.water.horizon.x,
            m_scene.water.horizon.y,
            m_scene.water.horizon.z);
        i32 const        mat = material_for(map, pool, water.shader.tag_id);
        m_scene.materials[mat].water_fan = true;
        std::vector<i32> materials(64, mat);
        return append_section(
            pool,
            magic,
            section,
            Matf4(1.f),
            nullptr,
            materials,
            false,
            nullptr,
            DrawTag{.section = static_cast<i32>(bsp_idx)});
    }

    /* One convex hull per collision BSP of a collision model, posed by the
     * render model's nodes */
    template<typename V>
    static void append_hulls(
        dim::map_container<V> const&        map,
        dim::obj::coll::model_t const&      coll,
        Matf4 const&                        transform,
        std::vector<Matf4> const&           node_world,
        std::unordered_map<u32, i16> const* permutations,
        CollisionGeometry&                  out)
    {
        auto regions = coll.regions.data(map.magic());
        if(regions.has_error())
            return;
        for(auto const& region : regions.value())
        {
            auto const* perm = dim::picked_permutation(map, region, permutations);
            if(!perm)
                continue;
            auto bsps = perm->bsps.data(map.magic());
            if(bsps.has_error())
                continue;
            for(auto const& ref : bsps.value())
            {
                Matf4 local = transform;
                i16   node  = ref.node.value();
                if(node >= 0 && static_cast<size_t>(node) < node_world.size())
                    local = transform * node_world[node];
                auto vertices = ref.bsp.vertices.data(map.magic());
                if(vertices.has_error())
                    continue;
                CollisionGeometry::node_t hull;
                for(auto const& v : vertices.value())
                {
                    Vecf3 point{
                        from_le(v.point[0]), from_le(v.point[1]), from_le(v.point[2])};
                    if(std::isfinite(point.x) && std::isfinite(point.y) &&
                       std::isfinite(point.z))
                        hull.points.push_back(Vecf3(local * Vecf4(point, 1.f)));
                }
                if(hull.points.size() >= 4)
                    out.nodes.push_back(std::move(hull));
            }
        }
    }

    /* Objects whose physics model has a body that is not fixed, and mass:
     * hulls from the collision model, the material from the shapes */
    template<typename V>
    std::optional<ObjectPhysics> moving_physics(
        dim::map_container<V> const&        map,
        dim::tag_reference<> const&         object,
        f32                                 scale,
        std::unordered_map<u32, i16> const* permutations)
    {
        auto const view = dim::object_view::of(map, object);
        /* Devices move as they are animated, never freely */
        if(!view.model || view.is_device())
            return std::nullopt;
        auto const& model = view.render_model;
        auto const& phmo  = view.physics_model;
        bool moves = false;
        f32  mass  = phmo ? from_le(phmo->mass) : 0.f;
        f32  sum   = 0.f;
        if(phmo)
            if(auto bodies = phmo->rigid_bodies.data(map.magic()); !bodies.has_error())
                for(auto const& body : bodies.value())
                {
                    /* Fixed and keyframed (swaying foliage) bodies stay put */
                    using motion = dim::obj::phmo::motion_type_t;
                    auto const m = body.motion_type();
                    moves |= m != motion::fixed && m != motion::keyframed;
                    sum += from_le(body.mass);
                }
        if(mass <= 0.f)
            mass = sum;
        /* Weapons, equipment and debris without a physics model are light
         * boxes, as in Halo 1 */
        bool const item = view.is(blam::tag_class_t::weap) || view.is(blam::tag_class_t::eqip) ||
                          view.is(blam::tag_class_t::garb);
        if(!(moves && mass > 0.f) && !item)
            return std::nullopt;
        if(!(moves && mass > 0.f))
        {
            if(!model)
                return std::nullopt;
            auto bounds = model->bounds.data(map.magic());
            if(bounds.has_error() || bounds.value().empty())
                return std::nullopt;
            auto const& b  = bounds.value()[0];
            Vecf3 const lo = Vecf3{from_le(b.x_min), from_le(b.y_min), from_le(b.z_min)} * scale;
            Vecf3 const hi = Vecf3{from_le(b.x_max), from_le(b.y_max), from_le(b.z_max)} * scale;
            return ObjectPhysics{
                .half_extents = glm::max((hi - lo) * .5f, Vecf3(.05f)),
                .center       = (hi + lo) * .5f,
                .mass         = 1.f,
                .item         = true,
            };
        }

        ObjectPhysics out{.mass = mass};
        f32 friction = 0.f, restitution = 0.f;
        u32 shapes   = 0;
        auto material = [&](auto const& block) {
            if(auto data = block.data(map.magic()); !data.has_error())
                for(auto const& s : data.value())
                {
                    friction += from_le(s.friction);
                    restitution += from_le(s.restitution);
                    shapes++;
                }
        };
        material(phmo->spheres);
        material(phmo->pills);
        material(phmo->boxes);
        if(shapes)
        {
            out.friction    = friction / static_cast<f32>(shapes);
            out.restitution = restitution / static_cast<f32>(shapes);
        }

        /* One hull per collision BSP, posed by the render model's nodes */
        auto const& coll = view.collision_model;
        std::vector<Matf4> node_world;
        if(model)
            node_world = dim::rest_frames(map, *model);
        Matf4 const scaled = glm::scale(Matf4(1.f), Vecf3(scale));
        auto        hulls  = std::make_shared<CollisionGeometry>();
        if(coll)
            append_hulls(map, *coll, scaled, node_world, permutations, *hulls);
        /* Bipeds stand, light enough to carry about, as in Halo 1 */
        if(view.is(blam::tag_class_t::bipd))
        {
            out.mass    = 1.f;
            out.upright = true;
        }
        if(!hulls->nodes.empty())
            out.collision = std::move(hulls);
        else if(model)
        {
            /* No collision model: the render model's bounds */
            auto bounds = model->bounds.data(map.magic());
            if(bounds.has_error() || bounds.value().empty())
                return std::nullopt;
            auto const& b  = bounds.value()[0];
            Vecf3 const lo = Vecf3{from_le(b.x_min), from_le(b.y_min), from_le(b.z_min)} * scale;
            Vecf3 const hi = Vecf3{from_le(b.x_max), from_le(b.y_max), from_le(b.z_max)} * scale;
            out.half_extents = glm::max((hi - lo) * .5f, Vecf3(.05f));
            out.center       = (hi + lo) * .5f;
        } else
            return std::nullopt;
        return out;
    }

    /* Every placement block of the scenario, then the multiplayer item and
     * vehicle spawns */
    template<typename V>
    void load_objects(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::scn::scenario_t const&  scnr)
    {
        u32  collision_triangles = 0, collision_objects = 0;
        u32  lit_objects = 0, varied_objects = 0;
        auto place = [&](dim::tag_t const*         mode_tag,
                         dim::tag_reference<> const* object,
                         Vecf3 const&              position,
                         Vecf3 const&              rotation,
                         f32                       scale,
                         i32                       bsp,
                         i32                       origin_bsp,
                         u32                       unique_id,
                         dim::string_id            variant,
                         bool                      collides = true,
                         bool                      animates = true) {
            Vecf3 pos{from_le(position[0]), from_le(position[1]), from_le(position[2])};
            /* Radians on disk (Assembly's "degree3" is a display unit);
             * same yaw/pitch/roll order as the Halo 1 spawns */
            Vecf3 rot{from_le(rotation[0]), from_le(rotation[1]), from_le(rotation[2])};
            Quatf q = Quatf(Vecf3(0, 0, 1) * rot.x) * Quatf(Vecf3(0, 1, 0) * rot.y) *
                      Quatf(Vecf3(1, 0, 0) * rot.z);
            if(scale <= 0.f)
                scale = 1.f;
            Matf4 transform = glm::translate(Matf4(1.f), pos) * glm::mat4_cast(q) *
                              glm::scale(Matf4(1.f), Vecf3(scale));
            DrawTag tag = object_tag(pos, bsp, origin_bsp);
            /* Scenarios place some objects more than once in one spot (per
             * BSP, or twice outright); copies seen together z-fight on
             * rounding, so only the first is kept */
            for(auto const& other : m_placed)
                if(other.model == mode_tag &&
                   (other.section < 0 || tag.section < 0 || other.section == tag.section) &&
                   glm::distance(other.position, pos) < .01f &&
                   std::abs(glm::dot(other.rotation, q)) > .9999f)
                    return false;
            m_placed.push_back({mode_tag, pos, q, tag.section});
            std::unordered_map<u32, i16> permutations;
            std::optional<ObjectPhysics> physics;
            if(object)
            {
                permutations = dim::variant_picks(map, dim::object_view::of(map, *object), variant);
                if(!permutations.empty())
                    varied_objects++;
                physics = moving_physics(map, *object, scale, &permutations);
            }
            /* Moving objects keep their geometry in object space and their
             * collision on their own body */
            if(physics)
            {
                transform    = glm::scale(Matf4(1.f), Vecf3(scale));
                tag.object   = static_cast<i32>(m_scene.moving_objects.size());
                tag.cluster  = -1;
                tag.instance = -1;
            } else if(object && collides)
                if(u32 n = append_object_collision(
                       map, *object, transform, bsp, &permutations))
                {
                    collision_triangles += n;
                    collision_objects++;
                }
            /* Animated objects too keep their geometry in object space,
             * skinned by their loop */
            std::optional<u32> animation;
            if(object && !physics && animates)
                animation = object_animation(map, pool, *object, *mode_tag);
            if(animation)
            {
                transform    = Matf4(1.f);
                tag.animated = static_cast<i32>(m_scene.animated_objects.size());
            }
            /* The ltmp of the BSP the object was lit in knows it by id */
            SceneryLight const* lighting = nullptr;
            LightBucket const*  bucket   = nullptr;
            for(size_t i = 0; i < m_scenery_lights.size() && unique_id; i++)
                if(auto it = m_scenery_lights[i].find(unique_id);
                   it != m_scenery_lights[i].end() && it->second.bucket >= 0 &&
                   i < m_buckets.size() &&
                   static_cast<size_t>(it->second.bucket) < m_buckets[i].size())
                {
                    lighting = &it->second;
                    bucket   = &m_buckets[i][it->second.bucket];
                    lit_objects++;
                    break;
                }
            size_t const first  = m_vertices.size();
            bool const   placed = append_model(
                                    map,
                                    pool,
                                    *mode_tag,
                                    transform,
                                    false,
                                    tag,
                                    lighting,
                                    bucket,
                                    &permutations) > 0;
            if(placed && !lighting)
                light_from_below(first, pos);
            if(placed && animation)
                m_scene.animated_objects.push_back({
                    .transform = glm::translate(Matf4(1.f), pos) * glm::mat4_cast(q) *
                                 glm::scale(Matf4(1.f), Vecf3(scale)),
                    .animation = *animation,
                });
            if(placed && physics)
            {
                auto  ref   = m_container.create_entity(moving_recipe);
                auto& model = ref.template get<Model>();
                model.position = pos;
                model.rotation = q;
                model.update_matrix();
                ref.template get<ObjectPhysics>() = std::move(*physics);
                m_scene.moving_objects.push_back(ref.id());
            }
            return placed;
        };

        auto block = [&](auto const& placements_block,
                         auto const& palette_block,
                         const char* what) {
            u32 placed = 0, skipped = 0, scripted = 0;
            bool const show_scripted =
                m_container.subsystem_cast<RenderingParameters>().show_scripted_placements;
            for(auto [p, object] : dim::placements(map, placements_block, palette_block))
            {
                /* Left for scripts to create (cutscene actors and props) */
                if(!show_scripted &&
                   static_cast<u32>(p.placement_flags) &
                   static_cast<u32>(
                       dim::scn::placement_flags_t::not_automatically |
                       dim::scn::placement_flags_t::never_placed))
                {
                    scripted++;
                    continue;
                }
                dim::tag_t const* mode_tag = object ? render_model_of(map, *object) : nullptr;
                /* Manual BSP flags: one bit per structure BSP; none = all.
                 * An object shared by BSPs is placed once per BSP, each copy
                 * living in its origin BSP, so they never overlap */
                u16 flags  = static_cast<u16>(p.bsp_flags);
                i16 origin = p.origin_bsp.value();
                i32 bsp    = !flags          ? -1
                             : origin >= 0 && origin < 16 && (flags >> origin) & 1
                                 ? origin
                                 : static_cast<i32>(std::countr_zero(flags));
                dim::string_id variant{};
                if constexpr(requires { p.variant; })
                    variant = p.variant.variant_name;
                /* Devices a group positions wait for it, closed */
                bool animates = true;
                if constexpr(requires { p.position_group; })
                    animates = p.position_group.value() < 0;
                if(mode_tag && place(
                                   mode_tag,
                                   &object->untyped(),
                                   p.position,
                                   p.rotation,
                                   from_le(p.scale),
                                   bsp,
                                   p.origin_bsp.value(),
                                   from_le(p.unique_id.raw),
                                   variant,
                                   true,
                                   animates))
                    placed++;
                else
                    skipped++;
            }
            if(placed || skipped || scripted)
                cDebug(
                    "H2: {}: {} placed, {} skipped, {} left to scripts",
                    what,
                    placed,
                    skipped,
                    scripted);
        };
        block(scnr.scenery, scnr.scenery_palette, "scenery");
        block(scnr.bipeds, scnr.biped_palette, "bipeds");
        block(scnr.vehicles, scnr.vehicle_palette, "vehicles");
        block(scnr.equipment, scnr.equipment_palette, "equipment");
        block(scnr.weapons, scnr.weapon_palette, "weapons");
        block(scnr.machines, scnr.machine_palette, "machines");
        block(scnr.controls, scnr.control_palette, "controls");
        block(scnr.light_fixtures, scnr.light_fixture_palette, "light fixtures");
        block(scnr.crates, scnr.crate_palette, "crates");

        /* Netgame spawns: the collection's first permutation stands in */
        if(auto spawns = scnr.netgame_equipment.data(map.magic());
           !spawns.has_error())
        {
            u32 placed = 0, skipped = 0;
            for(auto const& spawn : spawns.value())
            {
                auto const coll = map.template get<dim::scn::item_collection_t>(spawn.collection);
                dim::tag_t const*         mode_tag = nullptr;
                dim::tag_reference<> const* item     = nullptr;
                dim::string_id            variant{};
                if(coll)
                    if(auto perms = coll->permutations.data(map.magic());
                       !perms.has_error() && !perms.value().empty())
                    {
                        item     = &perms.value()[0].item.untyped();
                        variant  = perms.value()[0].variant_name;
                        mode_tag = render_model_of(map, *item);
                    }
                /* Pickups: drawn with their variant; only those with a
                 * physics body collide */
                if(mode_tag && place(
                                   mode_tag,
                                   item,
                                   spawn.position,
                                   spawn.orientation,
                                   1.f,
                                   -1,
                                   -1,
                                   0,
                                   variant,
                                   false))
                    placed++;
                else
                    skipped++;
            }
            cDebug("H2: netgame equipment: {} placed, {} skipped", placed, skipped);
        }
        cDebug(
            "H2: {} of {} objects resolved to a cluster; {} contribute {} "
            "collision triangles; {} lit per vertex, {} from the lightmap "
            "below, {} unlit; {} with a variant; {} move",
            m_objects_clustered,
            m_objects_total,
            collision_objects,
            collision_triangles,
            lit_objects,
            m_below_lit_objects,
            m_unlit_objects,
            varied_objects,
            m_scene.moving_objects.size());
    }

    /* A structure BSP's decorators: grass as the cache's prebuilt quads,
     * models (rocks, reeds) as the set's meshes, one per placement. Each
     * cache block is one 8 wu cell, a leaf of the cache's octree; placements
     * sit at offsets into it and carry their baked light */
    template<typename V>
    void load_decorators(
        dim::map_container<V> const&                  map,
        dim::raw_pool const&                          pool,
        dim::scn::scenario_t const&                   scnr,
        geo::bsp_header_t const&                      bsp,
        dim::bsp_magic const&                         bsp_magic,
        i32                                           bsp_idx)
    {
        auto palette = scnr.decorator_palette.data(map.magic());
        auto caches  = bsp.decorator_cache.data(bsp_magic);
        if(palette.has_error() || caches.has_error())
            return;
        u32 sprites = 0, models = 0;
        for(auto const& cache : caches.value())
        {
            auto blocks = cache.cache_blocks.data(bsp_magic);
            auto groups = cache.groups.data(bsp_magic);
            auto cells  = cache.cells.data(bsp_magic);
            if(blocks.has_error() || groups.has_error() || cells.has_error())
                continue;

            /* Each cache block's cell corner */
            std::vector<std::optional<Vecf3>> corner(blocks.value().size());
            f32 const root = static_cast<f32>(from_le(cache.cell_count)) * 8.f;
            auto      locate = [&](auto& self, size_t ci, Vecf3 at, f32 size, u32 depth) -> void {
                if(ci >= cells.value().size() || depth > 16)
                    return;
                auto const& cell = cells.value()[ci];
                i16 const   blk  = from_le(cell.cache_block);
                if(blk >= 0 && static_cast<size_t>(blk) < corner.size())
                    corner[blk] = at;
                f32 const half = size * .5f;
                for(u32 k = 0; k < 8; k++)
                    if(i16 const child = from_le(cell.children[k]); child >= 0)
                        self(self,
                             static_cast<size_t>(child),
                             at + Vecf3((k >> 2) & 1, (k >> 1) & 1, k & 1) * half,
                             half,
                             depth + 1);
            };
            Vecf3 const center(
                from_le(cache.grid_center[0]),
                from_le(cache.grid_center[1]),
                from_le(cache.grid_center[2]));
            locate(locate, 0, center - Vecf3(root * .5f), root, 0);

            for(auto const& group : groups.value())
            {
                if(group.set >= palette.value().size() || group.cache_block < 0 ||
                   static_cast<size_t>(group.cache_block) >= blocks.value().size())
                    continue;
                auto const set = map.template get<geo::decorator_set_t>(palette.value()[group.set].tag);
                if(!set)
                    continue;
                auto classes = set->classes.data(map.magic());
                auto shaders = set->shaders.data(map.magic());
                if(classes.has_error() || shaders.has_error() || shaders.value().empty() ||
                   group.type >= classes.value().size())
                    continue;
                auto perms = classes.value()[group.type].permutations.data(map.magic());
                if(perms.has_error() || perms.value().empty())
                    continue;
                auto const& perm =
                    perms.value()[std::min<size_t>(group.permutation, perms.value().size() - 1)];
                i32 const mat = material_for(
                    map,
                    pool,
                    shaders.value()[std::min<size_t>(perm.shader, shaders.value().size() - 1)]
                        .shader.tag_id);
                if(mat < 0)
                    continue;
                m_scene.materials[mat].alpha_test = true;

                auto const& block = blocks.value()[group.cache_block];
                auto raw = pool.resolve(bsp_magic, &block, block.data, from_le(block.size));
                auto resources = block.resources.data(bsp_magic);
                if(!raw || resources.has_error() ||
                   raw->size() < sizeof(geo::decorator_block_t))
                    continue;
                auto const* bytes = reinterpret_cast<u8 const*>(raw->data());
                auto const* body  = reinterpret_cast<geo::decorator_block_t const*>(bytes);
                auto data_of = [&](size_t field) -> u8 const* {
                    for(auto const& r : resources.value())
                        if(static_cast<size_t>(r.type0()) == field - 8)
                        {
                            size_t at = from_le(block.header_size) + 8 + from_le(r.offset_raw);
                            return at + from_le(r.size_raw) <= raw->size() ? bytes + at : nullptr;
                        }
                    return nullptr;
                };
                auto const* placements = reinterpret_cast<geo::decorator_placement_t const*>(
                    data_of(offsetof(geo::decorator_block_t, placements)));
                i32 const nplace = body->placements.count();
                i16 const pstart = from_le(group.placement_start);
                i16 const pcount = from_le(group.placement_count);
                if(!placements || pstart < 0 || pcount <= 0 || pstart + pcount > nplace)
                    continue;
                auto light_of = [&](i32 p, Vertex& out) {
                    /* Stored at full brightness; the lightmap path doubles */
                    auto const& l = placements[p].light;
                    out.light[0]  = l[0] / 2;
                    out.light[1]  = l[1] / 2;
                    out.light[2]  = l[2] / 2;
                    out.light[3]  = 255;
                };
                DrawTag const tag{
                    .section         = bsp_idx,
                    .cluster_section = bsp_idx,
                    .cluster         = from_le(group.cluster),
                };

                std::vector<u16> list;
                auto const       vbase = static_cast<u32>(m_vertices.size());
                Vecf3 lo(std::numeric_limits<f32>::max()), hi(std::numeric_limits<f32>::lowest());
                f32   reach = 0.f;
                i16 const vcount = from_le(group.vertex_count);
                i16 const icount = from_le(group.index_count);
                if(vcount > 0 && icount > 0)
                {
                    /* Prebuilt quads: pivots in world space, corners turned to
                     * face the eye in the vertex shader */
                    auto const* verts = reinterpret_cast<geo::decorator_sprite_vertex_t const*>(
                        data_of(offsetof(geo::decorator_block_t, sprite_vertices)));
                    auto const* indices = reinterpret_cast<u16 const*>(
                        data_of(offsetof(geo::decorator_block_t, sprite_indices)));
                    i16 const vstart = from_le(group.vertex_start);
                    i16 const istart = from_le(group.index_start);
                    if(!verts || !indices || vstart < 0 || istart < 0 ||
                       vstart + vcount > body->sprite_vertices.count() ||
                       istart + icount > body->sprite_indices.count())
                        continue;
                    m_scene.materials[mat].billboard = true;
                    for(i16 i = 0; i < vcount; i++)
                    {
                        auto const& v = verts[vstart + i];
                        Vertex      out{
                                 .position = Vecf3(from_le(v.position[0]), from_le(v.position[1]), from_le(v.position[2])),
                                 .texcoord = Vecf2(from_le(v.texcoord[0]), from_le(v.texcoord[1])),
                                 .normal   = Vecf3(from_le(v.offset[0]), from_le(v.offset[1]), from_le(v.offset[2])),
                        };
                        /* Four corners a placement, in order */
                        if(i32 const p = pstart + i / 4; p < nplace)
                            light_of(p, out);
                        lo    = glm::min(lo, out.position);
                        hi    = glm::max(hi, out.position);
                        reach = std::max(reach, glm::length(out.normal));
                        m_vertices.push_back(out);
                    }
                    for(i16 i = 0; i < icount; i++)
                    {
                        i32 const idx = static_cast<i32>(from_le(indices[istart + i])) - vstart;
                        if(idx < 0 || idx >= vcount)
                            break;
                        list.push_back(static_cast<u16>(idx));
                    }
                    if(list.size() != static_cast<size_t>(icount))
                    {
                        m_vertices.resize(vbase);
                        continue;
                    }
                    sprites += static_cast<u32>(pcount);
                } else
                {
                    if(!corner[group.cache_block])
                        continue;
                    auto set_models = set->models.data(map.magic());
                    auto set_verts  = set->vertices.data(map.magic());
                    auto set_index  = set->indices.data(map.magic());
                    if(set_models.has_error() || set_verts.has_error() ||
                       set_index.has_error() || set_models.value().empty())
                        continue;
                    /* Models follow the classes' permutations in order */
                    size_t index = std::min<size_t>(group.permutation, perms.value().size() - 1);
                    for(size_t c = 0; c < group.type; c++)
                        index += static_cast<size_t>(std::max(classes.value()[c].permutations.count(), 0));
                    if(index >= set_models.value().size())
                        continue;
                    auto const* model = &set_models.value()[index];
                    i32 const first = from_le(model->index_start);
                    i32 const count = from_le(model->index_count);
                    if(first < 0 || count <= 0 ||
                       static_cast<size_t>(first + count) > set_index.value().size())
                        continue;
                    /* Its own vertex range, so each copy re-bases its indices */
                    u16 vmin = 0xFFFF, vmax = 0;
                    for(i32 i = 0; i < count; i++)
                    {
                        u16 const v = from_le(set_index.value()[first + i]);
                        vmin        = std::min(vmin, v);
                        vmax        = std::max(vmax, v);
                    }
                    if(vmax >= set_verts.value().size())
                        continue;
                    u32 const span = vmax - vmin + 1u;
                    f32 const smin = from_le(perm.scale_min);
                    f32 const smax = from_le(perm.scale_max);
                    for(i32 p = pstart; p < pstart + pcount; p++)
                    {
                        if((m_vertices.size() - vbase) + span > 0xFFFF)
                            break;
                        auto const& place = placements[p];
                        Vecf3 const at    = *corner[group.cache_block] + place.offset();
                        Matf3 const turn =
                            Matf3(glm::rotate(Matf4(1.f), place.yaw(), Vecf3(0.f, 0.f, 1.f))) *
                            glm::mix(smin, smax, place.scale());
                        auto const  base = static_cast<u32>(m_vertices.size() - vbase);
                        for(u32 v = vmin; v <= vmax; v++)
                        {
                            auto const& src = set_verts.value()[v];
                            Vertex      out{
                                     .position = at + turn * Vecf3(from_le(src.position[0]), from_le(src.position[1]), from_le(src.position[2])),
                                     .texcoord = Vecf2(from_le(src.texcoord[0]), from_le(src.texcoord[1])),
                                     .normal   = glm::normalize(turn * Vecf3(from_le(src.normal[0]), from_le(src.normal[1]), from_le(src.normal[2]))),
                            };
                            light_of(p, out);
                            lo = glm::min(lo, out.position);
                            hi = glm::max(hi, out.position);
                            m_vertices.push_back(out);
                        }
                        for(i32 i = 0; i < count; i++)
                            list.push_back(static_cast<u16>(
                                base + from_le(set_index.value()[first + i]) - vmin));
                        models++;
                    }
                    if(list.empty())
                    {
                        m_vertices.resize(vbase);
                        continue;
                    }
                }
                m_scene.materials[mat].section_draws.push_back({
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
                    .tag    = tag,
                    .bounds = Vecf4((lo + hi) * .5f, glm::distance(lo, hi) * .5f + reach),
                });
                m_indices.insert(m_indices.end(), list.begin(), list.end());
            }
        }
        if(sprites || models)
            cDebug("H2: bsp {}: {} decorator sprites, {} decorator models", bsp_idx, sprites, models);
    }

    /* The first sky's model (empty palette slots skipped: the main menu's
     * is the seventh), kept around the camera; scaled into the far plane
     * since its radius is in the thousands */
    template<typename V>
    void load_sky(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::scn::scenario_t const&  scnr)
    {
        m_scene.sky_scale     = 1.f;
        m_scene.sky_clear     = {};
        m_scene.sun_direction = {0.f, 0.f, 1.f};
        m_scene.sky_flares.clear();
        auto skies   = scnr.skies.data(map.magic());
        if(skies.has_error() || skies.value().empty())
            return;
        dim::tag_t const* sky_tag = nullptr;
        for(auto const& ref : skies.value())
            if((sky_tag = map.tag_at(ref.sky.tag_id)))
                break;
        auto const* sky =
            sky_tag ? sky_tag->template data<dim::obj::sky_t>(map.magic())
                    : nullptr;
        if(!sky)
            return;
        m_scene.sky_clear = Vecf3{
            from_le(sky->clear_color[0]),
            from_le(sky->clear_color[1]),
            from_le(sky->clear_color[2])};
        if(auto lights = sky->lights.data(map.magic());
           !lights.has_error() && !lights.value().empty())
        {
            f32 const yaw   = from_le(lights.value()[0].direction[0]);
            f32 const pitch = from_le(lights.value()[0].direction[1]);
            m_scene.sun_direction = Vecf3{
                std::cos(pitch) * std::cos(yaw), std::cos(pitch) * std::sin(yaw), std::sin(pitch)};
        }
        load_sky_flares(map, pool, *sky);
        auto const* mode_tag = map.tag_at(sky->render_model.tag_id);
        if(!mode_tag || !mode_tag->matches(blam::tag_class_t::mode))
            return;
        size_t first    = m_vertices.size();
        u32    sections = append_model(map, pool, *mode_tag, Matf4(1.f), true);
        f32    radius   = 0.f;
        for(size_t i = first; i < m_vertices.size(); i++)
            radius = std::max(radius, glm::length(m_vertices[i].position));
        constexpr f32 sky_radius = 80.f; /* inside the 100 unit far plane */
        if(radius > sky_radius)
            m_scene.sky_scale = sky_radius / radius;
        cDebug(
            "H2: sky {}: {} sections, radius {}",
            map.name_of(*sky_tag),
            sections,
            radius);
    }

    /* Each sky light with a lens flare; yaw and pitch put the light around
     * the world's z axis */
    template<typename V>
    void load_sky_flares(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::obj::sky_t const&       sky)
    {
        auto lights = sky.lights.data(map.magic());
        if(lights.has_error())
            return;
        for(auto const& light : lights.value())
        {
            auto const* lens_tag = map.tag_at(light.lens_flare.tag_id);
            auto const* lens =
                lens_tag ? lens_tag->template data<dim::obj::lens_t>(map.magic())
                         : nullptr;
            auto const* bitm = lens ? map.tag_at(lens->bitmap.tag_id) : nullptr;
            if(!bitm)
                continue;
            auto reflections = lens->reflections.data(map.magic());
            if(reflections.has_error())
                continue;
            f32 const yaw   = from_le(light.direction[0]);
            f32 const pitch = from_le(light.direction[1]);
            SkyFlare  flare{
                 .direction = Vecf3{
                    std::cos(pitch) * std::cos(yaw),
                    std::cos(pitch) * std::sin(yaw),
                    std::sin(pitch)},
            };
            if(auto rad = light.radiosity.data(map.magic());
               !rad.has_error() && !rad.value().empty())
                flare.color = Vecf3{
                    from_le(rad.value()[0].color[0]),
                    from_le(rad.value()[0].color[1]),
                    from_le(rad.value()[0].color[2])};
            for(auto const& r : reflections.value())
            {
                auto image = texture_for(
                    map, pool, bitm, static_cast<size_t>(std::max<i16>(r.image, 0)));
                if(!image)
                    continue;
                flare.sprites.push_back({
                    .image    = *image,
                    .position = from_le(r.position),
                    .radius   = from_le(r.radius[1]),
                    .color    = Vecf3{
                        from_le(r.color[0]),
                        from_le(r.color[1]),
                        from_le(r.color[2])} *
                             from_le(r.brightness[1]),
                });
            }
            cDebug(
                "H2: sky light ({}, {}): flare {}, {} sprites",
                yaw,
                pitch,
                map.name_of(*lens_tag),
                flare.sprites.size());
            m_scene.sky_flares.push_back(std::move(flare));
        }
    }

    /* Index into m_scene.materials for a shad tag, created on first use. The last
     * material is always the untextured fallback. */
    template<typename V>
    i32 material_for(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::datum_index             shad_id,
        bool                         sky = false)
    {
        if(m_scene.materials.empty())
            m_scene.materials.push_back(Material{
                .name    = "<fallback>",
            });
        auto const* shad_tag = map.tag_at(shad_id);
        if(!shad_tag)
            return static_cast<i32>(m_scene.materials.size()) - 1;
        u32 key = shad_id.index() | (sky ? 0x10000u : 0u);
        if(auto it = m_material_index.find(key); it != m_material_index.end())
            return it->second;

        Material mat{
            .name    = std::string(map.name_of(*shad_tag)),
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
            /* Texture transforms (<usage>_u / _v rows) carry their scale
             * in the layer lookup */
            if(auto own = base.layer(base.usage))
                mat.uv_scale = {own->scale_u, own->scale_v};
            mat.alpha_test = base.alpha_test;
            mat.water      = base.water;
            /* Transparent templates name their blend: one_add_*, two_add_*
             * and the plasmas add, the rest alpha blend */
            auto const& tmpl = base.template_name;
            bool const  adds = tmpl.find("_add") != tmpl.npos ||
                              tmpl.find("plasma") != tmpl.npos;
            mat.additive = base.transparent && !base.water && adds;
            mat.blended  = base.transparent && !base.water && !adds;
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
            if(!mat.water && !sky)
                if(auto b = base.layer("bump_map"))
                    if(auto t = texture_for(map, pool, b->bitmap))
                    {
                        mat.bump       = *t;
                        mat.bump_scale = {b->scale_u, b->scale_v};
                    }
            if(tmpl == "water_static" || tmpl == "water_edge_blend")
                ocean_material(map, pool, base, mat);
            if(tmpl.starts_with("plasma_alpha"))
                plasma_material(map, pool, base, mat);
            if(auto env = base.layer("environment_map"))
                env_material(map, pool, base, *env, mat);
            if(sky || tmpl.find("clouds") != tmpl.npos)
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
                      : mat.ocean ? Variant::ocean
                      : mat.plasma ? Variant::plasma
                      : mat.water ? Variant::water
                      : mat.mask.sampler ? Variant::clouds
                      : static_cast<Variant>(
                            mat.detail_mode + (mat.illum.sampler ? 3 : 0));
        cDebug(
            "H2: shad {}: uv scale {}x{}, {}{}{}{}{}",
            mat.name,
            mat.uv_scale.x,
            mat.uv_scale.y,
            !mat.texture ? "no diffuse" : "textured",
            mat.detail_mode == 2   ? ", blended detail"
            : mat.detail_mode == 1 ? ", detail"
                                   : "",
            mat.bump.sampler ? ", bump" : "",
            mat.env_sampler ? ", env" : "",
            resolved ? "" : " (template unresolved)");

        m_scene.materials.insert(m_scene.materials.end() - 1, std::move(mat));
        i32 idx = static_cast<i32>(m_scene.materials.size()) - 2;
        /* Everything after the inserted slot moved by one: only the
         * fallback, which holds no cached index */
        m_material_index.emplace(key, idx);
        return idx;
    }

    /* water_static's slots by index: wave bump, sky, gradients, fresnel. The
     * edge blend (the infinite plane fading out) borrows them from the map's
     * water_static and fades by its own first slot */
    template<typename V>
    void ocean_material(
        dim::map_container<V> const&       map,
        dim::raw_pool const&               pool,
        dim::material::resolved_t const&   base,
        Material&                          mat)
    {
        std::optional<dim::material::resolved_t> other;
        auto const*                              src = &base;
        if(base.template_name == "water_edge_blend")
        {
            if(!base.slot_bitmaps.empty())
                if(auto t = texture_for(map, pool, base.slot_bitmaps[0]))
                    mat.mask = *t;
            for(auto const& tag : map.tags())
            {
                if(!tag.valid() || !tag.matches(blam::tag_class_t::shad))
                    continue;
                auto const* shad =
                    tag.template data<geo::shad_header_t>(map.magic());
                auto const* stem = shad ? map.tag_at(shad->shader_template.tag_id)
                                        : nullptr;
                if(stem && map.name_of(*stem).ends_with("\\water_static"))
                {
                    other = dim::material::resolve(map, *shad);
                    src   = &*other;
                    break;
                }
            }
        }
        if(src->slot_bitmaps.size() < 2)
            return;
        auto bump = texture_for(map, pool, src->slot_bitmaps[0]);
        auto sky  = texture_for(map, pool, src->slot_bitmaps[1]);
        if(!bump || !sky)
            return;
        mat.ocean    = true;
        mat.blended  = true;
        mat.additive = false;
        mat.texture  = bump->texture;
        mat.sampler  = bump->sampler;
        mat.detail   = *sky;
        if(!src->tilings.empty() && src->tilings[0][0] != 0.f)
            mat.uv_scale = {src->tilings[0][0], src->tilings[0][1]};
    }

    /* plasma_alpha and its active camo twin, by slot: noise A, noise B,
     * shape; pixel constants 0 and 1 are the bright and dim colours */
    template<typename V>
    void plasma_material(
        dim::map_container<V> const&     map,
        dim::raw_pool const&             pool,
        dim::material::resolved_t const& base,
        Material&                        mat)
    {
        if(base.slot_bitmaps.size() < 3 || base.pixel_constants.size() < 2)
            return;
        auto a     = texture_for(map, pool, base.slot_bitmaps[0]);
        auto b     = texture_for(map, pool, base.slot_bitmaps[1]);
        auto shape = texture_for(map, pool, base.slot_bitmaps[2]);
        if(!a || !b || !shape)
            return;
        mat.texture     = a->texture;
        mat.sampler     = a->sampler;
        mat.detail      = *b;
        mat.detail_mode = 0;
        mat.mask        = *shape;
        mat.plasma      = true;
        mat.additive    = true;
        mat.blended     = false;
        for(size_t i = 0; i < 2; i++)
        {
            auto const& c       = base.pixel_constants[i];
            mat.plasma_colors[i] = Vecf3{c[0], c[1], c[2]} / 255.f;
        }
    }

    /* The lightmap_env pass's constants: facing and glancing tints, then
     * their brightness in alpha */
    template<typename V>
    void env_material(
        dim::map_container<V> const&              map,
        dim::raw_pool const&                      pool,
        dim::material::resolved_t const&          base,
        dim::material::resolved_t::layer_t const& env,
        Material&                                 mat)
    {
        std::vector<u8> const* slots = nullptr;
        for(size_t i = 0; i < base.pass_names.size(); i++)
            if((base.pass_names[i] == "lightmap_env" ||
                base.pass_names[i] == "lightmap_tex_env") &&
               i < base.pass_pixel_slots.size() &&
               base.pass_pixel_slots[i].size() >= 4)
            {
                slots = &base.pass_pixel_slots[i];
                break;
            }
        if(!slots || !env.bitmap)
            return;
        auto constant = [&](size_t i) {
            u8 slot = (*slots)[i];
            return slot < base.pixel_constants.size() ? base.pixel_constants[slot]
                                                      : std::array<u8, 4>{};
        };
        auto key = static_cast<u16>(env.bitmap - map.tags().data());
        auto it  = m_cubes.find(key);
        if(it == m_cubes.end())
        {
            std::shared_ptr<gfx::texture_cube_t> cube;
            std::shared_ptr<gfx::sampler_t>      sampler;
            if(auto tex = upload_cubemap(*m_api, map, pool, *env.bitmap))
            {
                cube    = *tex;
                sampler = cube->sampler();
                sampler->alloc();
                sampler->set_filtering(
                    typing::Filtering::Linear,
                    typing::Filtering::Linear,
                    typing::Filtering::Linear);
            }
            it = m_cubes.emplace(key, std::make_pair(cube, sampler)).first;
        }
        if(!it->second.second)
            return;
        auto rgb = [](std::array<u8, 4> const& c) {
            return Vecf3{c[0], c[1], c[2]} / 255.f;
        };
        mat.env_texture  = it->second.first;
        mat.env_sampler  = it->second.second;
        mat.env_facing   = Vecf4(rgb(constant(0)), constant(2)[3] / 255.f);
        mat.env_glancing = Vecf4(rgb(constant(1)), constant(3)[3] / 255.f);
    }

    /* Uploaded once per bitm tag; detail maps are shared by many shaders */
    template<typename V>
    std::optional<Texture> texture_for(
        dim::map_container<V> const& map,
        dim::raw_pool const&         pool,
        dim::tag_t const*            bitm_tag,
        size_t                       image = 0)
    {
        if(!bitm_tag || !bitm_tag->matches(blam::tag_class_t::bitm))
            return std::nullopt;
        auto key = static_cast<u32>(bitm_tag - map.tags().data()) |
                   static_cast<u32>(image << 16);
        if(auto it = m_textures.find(key); it != m_textures.end())
            return it->second;
        auto shared = shared_key(map, pool, *bitm_tag, image);
        if(shared)
            if(auto it = m_shared_textures.find(*shared);
               it != m_shared_textures.end())
            {
                it->second.generation = m_scene.generation;
                m_textures.emplace(key, it->second.texture);
                return it->second.texture;
            }
        auto tex = upload_bitmap(*m_api, map, pool, *bitm_tag, image);
        if(!tex)
            return std::nullopt;
        Texture out = world::sampled(*tex);
        m_textures.emplace(key, out);
        if(shared)
            m_shared_textures[*shared] = {out, m_scene.generation};
        return out;
    }

    void place_camera(Vecf3 const& position, f32 yaw)
    {
        world::ensure_local_player(m_container);
        world::place_local_cameras(m_container, position, yaw);
    }

    compo::EntityContainer& m_container;
    H2Scene&                m_scene;
    H2ShellData&            m_shell;
    H2SoundBank&            m_bank;
    gfx::system*            m_api{nullptr};
    std::optional<MapLoadEvent> m_source;
    bool                        m_scripted_shown{false};
    std::vector<std::string> m_font_table;
    std::vector<std::pair<std::string, std::shared_ptr<comp_app::FileMapper::Resource>>>
        m_font_files;

    std::vector<Vertex>              m_vertices;
    std::vector<u16>                 m_indices;
    std::unordered_map<u32, i32>     m_material_index; /* shad tag -> slot */
    struct placed_t
    {
        dim::tag_t const* model;
        Vecf3             position;
        Quatf             rotation;
        i32               section;
    };
    std::vector<placed_t> m_placed;
    /* (jmad, mode) -> H2Scene::animations slot */
    std::unordered_map<u64, std::optional<u32>> m_animation_index;
    std::unordered_map<u32, Texture> m_textures; /* bitm tag | image << 16 -> gpu */
    std::unordered_map<
        u16,
        std::pair<std::shared_ptr<gfx::texture_cube_t>, std::shared_ptr<gfx::sampler_t>>>
        m_cubes; /* bitm tag -> cube, unusable ones kept empty */
    struct SharedTexture
    {
        Texture texture;
        u32     generation;
    };
    std::unordered_map<u64, SharedTexture> m_shared_textures; /* side slot:offset */
    std::array<std::shared_ptr<dim::raw_source>, 3> m_side_sources;
    std::vector<std::unique_ptr<LightmapImage>>        m_lightmaps;
    std::vector<std::vector<LightBucket>>              m_buckets;        /* per BSP */
    std::vector<std::unordered_map<u32, SceneryLight>> m_scenery_lights; /* per BSP */
    world::ColumnGrid<LightTriangle>                   m_light_triangles;
    u32 m_below_lit_objects{0}, m_unlit_objects{0};
    LightmapAtlas                                      m_lightmap_atlas;
    std::set<std::string>                              m_unknown_passes;
    u32  m_objects_total{0}, m_objects_clustered{0};
    u32  m_model_fail_logged{0};
    bool m_normal_order_logged{false};
};

void alloc_h2(compo::EntityContainer& e)
{
    auto& scene = e.register_subsystem_inplace<H2Scene>();
    e.register_subsystem_inplace<H2Visibility>();
    auto& shell  = e.register_subsystem_inplace<H2ShellData>();
    auto& bank   = e.register_subsystem_inplace<H2SoundBank>();
    auto& loader = e.register_subsystem_inplace<H2MapLoader>(
        std::ref(e), std::ref(scene), std::ref(shell), std::ref(bank));
    world::consume_map_loads(e, loader);
    alloc_h2_occluder(e);
    alloc_h2_renderer(e);
    alloc_h2_shell(e);
}

} // namespace h2
