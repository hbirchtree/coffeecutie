#include "loading.h"
#include "bitmap_cache.h"
#include "blam/volta/blam_globals.h"
#include "blam/volta/blam_mod2.h"
#include "blam/volta/blam_scenario.h"
#include "blam/volta/blam_stl.h"
#include "blam/volta/blam_tag_ref.h"
#include "blam_files.h"
#include "caching.h"
#include "caching_item.h"
#include "components.h"
#include "data.h"
#include "map_marker.h"
#include "materials.h"
#include "selected_version.h"
#include "shader_cache.h"
#include "types.h"
#include <optional>
#include <set>

template<typename Ver>
using ResourceLoaderManifest = compo::SubsystemManifest<
    type_list_t<
        AnimationPlayback,
        BspReference,
        DebugDraw,
        DepthInfo,
        DrawState,
        Model,
        MultiplayerSpawn,
        NetworkInfo,
        ObjectSpawn,
        ShaderData,
        SubModel,
        TriggerVolume,
        Visibility,
        WorldInfo,
        const PlayerCamera,
        PlayerInfo>,
    type_list_t<
        BitmapCache<Ver>,
        BlamFiles<Ver>,
        BlamResources,
        BSPCache<Ver>,
        DebugMarkers,
        ModelCache<Ver>,
        ShaderCache<Ver>,
        const LoadingStatus>,
    empty_list_t>;

template<typename Ver>
struct ResourceLoader
    : compo::
          RestrictedSubsystem<ResourceLoader<Ver>, ResourceLoaderManifest<Ver>>
{
    using type  = ResourceLoader<Ver>;
    using Proxy = compo::proxy_of<ResourceLoaderManifest<Ver>>;

    blam::tag_index_view<Ver> index;
    GameEventBus*             game_bus{nullptr};

    std::vector<SpawnBSPEvent>         pending_bsps;
    std::vector<SpawnBipedEvent>       pending_bipeds;
    std::vector<SpawnEquipmentEvent>   pending_equipment;
    std::vector<SpawnModelEvent>       pending_models;
    std::vector<MountModelEvent>       pending_mounts;
    std::vector<SpawnObjectEvent>      pending_objects;
    std::vector<u32>                   pending_despawns;
    std::optional<ClusterChangedEvent> pending_cluster_change;

    /*! Parts are kept here so they can go when the player entity does */
    struct player_biped_t
    {
        u32              load_generation{};
        std::vector<u64> parts;
    };

    std::map<u64, player_biped_t> player_bipeds;

    struct
    {
        u32                                           load_generation{};
        blam::tagref_typed_t<blam::tag_class_t::mod2> model{};
        PlayerInfo::biped_shape_t                     shape{};
    } biped_model;

    std::shared_ptr<GameEventBus::queue_type<SpawnBSPEvent>> spawn_bsp_queue;
    std::shared_ptr<GameEventBus::queue_type<SpawnBipedEvent>>
        spawn_biped_queue;
    std::shared_ptr<GameEventBus::queue_type<SpawnEquipmentEvent>>
        spawn_equip_queue;
    std::shared_ptr<GameEventBus::queue_type<SpawnModelEvent>>
        spawn_model_queue;
    std::shared_ptr<GameEventBus::queue_type<MountModelEvent>>
        mount_model_queue;
    std::shared_ptr<GameEventBus::queue_type<SpawnObjectEvent>>
        spawn_object_queue;
    std::shared_ptr<GameEventBus::queue_type<DespawnObjectEvent>>
        despawn_object_queue;
    std::shared_ptr<GameEventBus::queue_type<ClusterChangedEvent>>
        cluster_queue;

    struct
    {
        u32 load_generation{};

        i16 skybox_id{-1};
        i16 weather_id{-1};
    } current;

    /* ModelCache::predict_impl copies vertices straight into vert_buffer /
     * element_buffer, so the model buffers have to be mapped for the whole
     * span of any predict() call. Mapped once around the frame's batch —
     * mapping per request would cost a GPU sync each time. */
    struct model_buffer_scope_t
    {
        model_buffer_scope_t(BlamResources& gpu, ModelCache<Ver>& cache)
            : m_gpu(gpu)
            , m_cache(cache)
        {
            auto vert              = m_gpu.model_buf->map(0);
            auto index             = m_gpu.model_index->map(0);
            m_cache.vert_buffer    = Bytes::ofContainer(vert);
            m_cache.element_buffer = Bytes::ofContainer(index);
        }

        ~model_buffer_scope_t()
        {
            m_gpu.model_buf->unmap();
            m_gpu.model_index->unmap();
            m_cache.vert_buffer    = {};
            m_cache.element_buffer = {};
        }

        BlamResources&   m_gpu;
        ModelCache<Ver>& m_cache;
    };

    ResourceLoader()
    {
        this->priority = 700;
    }

    bool main_thread_only() const override
    {
        return true;
    }

    void start_restricted(Proxy& p, compo::time_point const& t)
    {
        BlamFiles<Ver>& files = p.template subsystem<BlamFiles<Ver>>();
        if(current.load_generation != files.load_generation)
        {
            // Purge resources
            current.load_generation = files.load_generation;
            current.skybox_id       = -1;
            current.weather_id      = -1;
        }
        BlamResources&    resources  = p.template subsystem<BlamResources>();
        BitmapCache<Ver>& bitm_cache = p.template subsystem<BitmapCache<Ver>>();
        BSPCache<Ver>&    bsp_cache  = p.template subsystem<BSPCache<Ver>>();
        ModelCache<Ver>&  model_cache = p.template subsystem<ModelCache<Ver>>();
        ShaderCache<Ver>& shader_cache =
            p.template subsystem<ShaderCache<Ver>>();

        index = blam::tag_index_view<Ver>(files.container);

        cluster_queue->poll();
        mount_model_queue->poll();
        // spawn_biped_queue->poll();
        spawn_bsp_queue->poll();
        // spawn_equip_queue->poll();
        spawn_model_queue->poll();
        spawn_object_queue->poll();
        despawn_object_queue->poll();

        reconcile_player_bipeds(p, files);

        if(!pending_cluster_change && pending_bsps.empty() &&
           pending_models.empty() && pending_mounts.empty() &&
           pending_objects.empty() && pending_despawns.empty())
            return;

        /* Everything below can reach ModelCache::predict(), so the model
         * buffers stay mapped across the whole batch. */
        model_buffer_scope_t model_buffers(resources, model_cache);

        // Pending cluster first so we can use the shared model loading after
        if(pending_cluster_change)
        {
            if(pending_cluster_change->bsp)
            {
                load_world_lighting(
                    p,
                    pending_cluster_change->bsp->clusters
                        .at(pending_cluster_change->cluster)
                        .cluster->sky);
            }
            pending_cluster_change.reset();
        }

        for(auto const& bsp_load : pending_bsps)
        {
            // load full-level debug markers
            load_debug_shapes(p);
            load_scenario_bsp(p, bsp_load.section_id);
            load_scenario_objects(p);
        }
        pending_bsps.clear();

        for(auto const& model_load : pending_models)
            load_model(p, model_load.model);
        for(auto const& model_mount : pending_mounts)
            mount_model(p, model_mount);
        pending_models.clear();
        pending_mounts.clear();

        /* Spawns first, so a replacement exists before its impostor goes */
        for(auto const& object : pending_objects)
            spawn_object(p, object);
        pending_objects.clear();

        for(auto net_id : pending_despawns)
            despawn_object(p, net_id);
        pending_despawns.clear();
    }

    /* Sun direction/colour and fog for the world UBO, taken from the
     * scenario's skybox palette. Scenario-level state, so it does not wait
     * for the objects that used to be loaded alongside it. */
    void load_world_lighting(Proxy& p, i16 skybox_id = -1)
    {
        if(skybox_id == current.skybox_id)
            return;

        BlamResources&    resources = p.template subsystem<BlamResources>();
        ShaderCache<Ver>& shader_cache =
            p.template subsystem<ShaderCache<Ver>>();

        compo::EntityRef<Proxy> skybox_item;
        for(auto skybox : p.select(ObjectSkybox))
        {
            if(!p.template get<Model>(skybox.id()))
                continue;
            skybox_item = skybox;
        }

        if(!skybox_item.exists())
        {
            p.create_entity(shared_recipes::skybox_model);
            for(auto skybox : p.select(ObjectSkybox))
            {
                if(!p.template get<Model>(skybox.id()))
                    continue;
                skybox_item = skybox;
            }
        }

        if(!skybox_item.exists())
        {
            cWarning("No skybox entity with a Model component");
            return;
        }

        Model& skybox_mod = skybox_item.template get<Model>();
        WorldInfo& world_info = skybox_item.template get<WorldInfo>();

        auto& data        = p.template subsystem<BlamFiles<Ver>>();
        auto& model_cache = p.template subsystem<ModelCache<Ver>>();

        auto const* scenario = data.container.scenario().value_or(nullptr);

        if(!scenario)
            return;

        auto const&          magic = data.container.magic;
        blam::tag_index_view index(data.container);

        current.skybox_id = skybox_id;

        auto skyboxes = scenario->info.skyboxes.data(magic).value();
        if(skybox_id != -1 && skybox_id < skyboxes.size())
        {
            auto const&              skybox     = skyboxes[skybox_id];
            auto                     skybox_tag = *index.tag_of(skybox);
            blam::scn::skybox const& skybox_ =
                skybox_tag->template data<blam::scn::skybox>(magic).value()[0];

            world_info.skybox = &skybox_;

            Span<const blam::scn::skybox::light> lights =
                skybox_.lights.data(magic).value();

            Span<materials::world_data> world_data =
                resources.world_store->map<materials::world_data>(0);
            if(world_data.empty())
            {
                resources.world_store->unmap();
                cWarning("Skybox update without a mapped world buffer");
                return;
            }

            for(auto& slot : world_data[0].lighting)
                slot = {};
            for(auto const& [i, light] : stl_types::const_enumerate(lights))
            {
                if(i >= std::size(world_data[0].lighting))
                    break;
                f32   yaw   = light.radiosity.direction.x;
                f32   pitch = light.radiosity.direction.y;
                Vecf3 rotation{
                    std::cos(pitch) * std::cos(yaw),
                    std::cos(pitch) * std::sin(yaw),
                    std::sin(pitch),
                };
                /* .w carries the light's interior/exterior flags  (1 =
                 * exteriors, 2 = interiors). */
                world_data[0].lighting[i].light_direction = Vecf4{
                    rotation,
                    static_cast<f32>(light.radiosity.flags),
                };
                world_data[0].lighting[i].light_color = Vecf4{
                    light.radiosity.color,
                    light.radiosity.power,
                };
            }

            world_data[0].fog.indoor_color =
                Vecf4(skybox_.indoor_fog.color, skybox_.indoor_fog.density);
            world_data[0].fog.indoor_ambient = Vecf4(
                skybox_.indoor_ambient.color, skybox_.indoor_ambient.power);
            world_data[0].fog.outdoor_color =
                Vecf4(skybox_.outdoor_fog.color, skybox_.outdoor_fog.density);
            world_data[0].fog.outdoor_ambient = Vecf4(
                skybox_.outdoor_ambient.color, skybox_.outdoor_ambient.power);

            world_data[0].fog.distances = Vecf4(
                skybox_.indoor_fog.start_distance,
                skybox_.indoor_fog.opaque_distance,
                skybox_.outdoor_fog.start_distance,
                skybox_.outdoor_fog.opaque_distance);

            if(skybox_.outdoor_fog.opaque_distance < 1)
                world_data[0].fog.distances.w = 1000.f;

            resources.world_store->unmap();

            if(skybox_.model.valid())
                skybox_mod.tag = *index.tag_of(skybox_.model);
            skybox_mod.origin_object = skybox_tag;
            skybox_mod.transform     = glm::identity<Matf4>();

            if(!skybox_mod.parts.empty())
            {
                std::set<u64> stale;
                for(auto const& part : skybox_mod.parts)
                    stale.insert(part.id());
                p.remove_entity_if([&stale](compo::Entity const& e) {
                    return stale.contains(e.id);
                });
                skybox_mod.parts.clear();
            }

            ModelAssembly assem = model_cache.predict_regions(
                skybox_.model, blam::mod2::mod2_lod::lod_high_ext);

            if(assem.models.empty())
            {
                cDebug("Invalid skybox");
                return;
            }

            skybox_mod.model = assem.models.at(0);

            for(auto const& part_id : assem.models)
            {
                ModelItem<Ver>& part = model_cache.get(part_id);
                skybox_mod.model     = part_id;

                for(typename ModelItem<Ver>::SubModel const& region :
                    part.mesh.sub)
                {
                    if(!region.shader.valid())
                        continue;

                    auto submod =
                        p.create_entity(shared_recipes::skybox_submodel);
                    skybox_mod.parts.push_back(submod);
                    SubModel& submodel  = submod.template get<SubModel>();
                    submodel.parent     = skybox_item.id();
                    DrawState& sub_draw = submod.template get<DrawState>();
                    submodel.initialize<Ver>(part_id, region, sub_draw);

                    ShaderData& shader_ = submod.template get<ShaderData>();
                    ShaderItem const& shader_it =
                        shader_cache.get(region.shader);
                    shader_.initialize(shader_it, submodel);

                    sub_draw.current_pass =
                        shader_.get_render_pass(shader_cache, true);

                    // Annotate DrawState with the shader info
                    auto shader_name = index.name_of(*shader_.shader_tag);
                    for(auto& draw : sub_draw.draw.data)
                        draw.debug_identifier = fmt::format(
                            "{} {}",
                            shader_.shader_tag->tagclass[0].str(),
                            shader_name);
                }
            }
        }
    }

    void load_debug_shapes(Proxy& p)
    {
        BlamFiles<Ver>& files         = p.template subsystem<BlamFiles<Ver>>();
        DebugMarkers&   debug_markers = p.template subsystem<DebugMarkers>();

        auto&                           container = files.container;
        auto const&                     magic     = container.magic;
        blam::scn::scenario<Ver> const* scenario = container.scenario().value();

        compo::EntityRecipe map_marker  = shared_recipes::gc_marker;
        compo::EntityRecipe trigger_obj = shared_recipes::trigger_volume;

        debug_markers.map(debug_axes_verts, debug_axes_colors);

        auto trigger_vols = scenario->trigger_volumes.data(magic).value();
        for(blam::scn::trigger_volume const& trigger : trigger_vols)
        {
            Vecf3 origin = trigger.position;
            Vecf3 second = trigger.position + trigger.extents;

            auto           trig   = p.create_entity(trigger_obj);
            TriggerVolume& volume = trig.template get<TriggerVolume>();
            DebugDraw&     draw   = trig.template get<DebugDraw>();

            draw = debug_markers.create_box(origin, second, Vecf3(1, 0, 0.5f));

            volume.trigger_volume = &trigger;
        }

        auto player_profiles =
            scenario->player_start.profiles.data(magic).value();
        for(blam::scn::player_starting_profile const& profile : player_profiles)
        {
            cDebug(" - Profile: {}", profile.name.str());
        }

        // auto platoons = scenario->ai.platoons.data(magic).value();
        // for(blam::scn::ai::platoon const& platoon : platoons)
        // {
        //     cDebug(" - Platoon: {}", platoon.unknown[0]);
        // }

        auto encounters = scenario->ai.encounters.data(magic).value();
        for(blam::scn::ai::encounter const& enc : encounters)
        {
            // cDebug(" - Encounter: {}", enc.text.str());
            auto platoons         = enc.platoons.data(magic).value();
            auto firing_positions = enc.firing_positions.data(magic).value();
            auto squads           = enc.squads.data(magic).value();
            auto start_locs       = enc.start_locations.data(magic).value();
            // for(auto const& platoon : platoons)
            //     cDebug("   - Platoon: {}", platoon.name.str());
            for(auto const& firing_pos : firing_positions)
            {
                // cDebug("   - Firing pos: {}", firing_pos.position);
                auto  marker = p.create_entity(map_marker);
                auto& draw   = marker.template get<DebugDraw>();

                draw = debug_markers.create_marker(
                    std::array<Vecf3, 5>{{
                        firing_pos.position + Vecf3{.1f, .1f, 0},
                        firing_pos.position + Vecf3{-.1f, -.1f, 0},
                        firing_pos.position + Vecf3{0, 0, 0},
                        firing_pos.position + Vecf3{.1f, -.1f, 0},
                        firing_pos.position + Vecf3{-.1f, .1f, 0},
                    }},
                    Vecf3{1.f, 0, 0});
            }
            for(auto const& loc : start_locs)
            {
                // cDebug("   - Start location: {}", loc.position);
                auto  marker = p.create_entity(map_marker);
                auto& draw   = marker.template get<DebugDraw>();

                draw = debug_markers.create_marker(
                    std::array<Vecf3, 5>{{
                        loc.position + Vecf3{0, 0, .3f},
                        loc.position + Vecf3{0, 0, 0},
                        loc.position + Vecf3{.1f, .1f, .1f},
                        loc.position + Vecf3{0, 0, 0},
                        loc.position + Vecf3{-.1f, -.1f, .1f},
                    }},
                    Vecf3{0, 1.f, 0});
            }
            for(auto const& squad : squads)
            {
                // cDebug("   - Squad: {}", squad.name.str());
                // auto locations = squad.
            }
        }

        auto mp_flags = scenario->netgame.flags.data(magic).value();
        for(blam::scn::multiplayer_flag const& flag : mp_flags)
        {
            // cDebug("MP flag: {}", flag.pos);
            auto  marker = p.create_entity(map_marker);
            auto& draw   = marker.template get<DebugDraw>();

            draw = debug_markers.create_marker(
                std::array<Vecf3, 5>{{
                    flag.pos,
                    flag.pos + Vecf3{0, 0, 1.f},
                    flag.pos + Vecf3{-0.1f, 0, 1.f},
                    flag.pos + Vecf3{-0.1f, 0, .9f},
                    flag.pos + Vecf3{0, 0, .9f},
                }},
                Vecf3{0, 0.5f, 0.5f});
        }

        auto spawns = scenario->player_start.locations.data(magic).value();
        for(blam::scn::player_starting_location const& spawn : spawns)
        {
            // cDebug(" - Spawn: @{}", spawn.pos);
            auto  marker = p.create_entity(map_marker);
            auto& draw   = marker.template get<DebugDraw>();
            draw         = debug_markers.create_marker(
                std::array<Vecf3, 6>{{
                    spawn.pos,
                    spawn.pos + Vecf3{0, 0, 1.f},
                    spawn.pos + Vecf3{-0.2f, 0, 1.1f},
                    spawn.pos + Vecf3{0, 0, 1.2f},
                    spawn.pos + Vecf3{0.2f, 0, 1.1f},
                    spawn.pos + Vecf3{0, 0, 1.f},
                }},
                spawn.team_index == 0   ? Vecf3{1.f, 0, 0}
                        : spawn.team_index == 1 ? Vecf3{0, 0, 1.f}
                                                : Vecf3{0.5f, 1.f, 0});
        }

        auto cutscene_flags = scenario->cutscene.flags.data(magic).value();
        for(blam::scn::cutscene_flag const& flag : cutscene_flags)
        {
            // cDebug(" - Cutscene flag: {}", flag.position);
            auto  marker = p.create_entity(map_marker);
            auto& draw   = marker.template get<DebugDraw>();
            draw         = debug_markers.create_marker(
                std::array<Vecf3, 5>{{
                    flag.position,
                    flag.position + Vecf3{0.2f, 0, 0.2f},
                    flag.position + Vecf3{0, 0, 0.4f},
                    flag.position + Vecf3{-0.2f, 0, 0.2f},
                    flag.position,
                }},
                Vecf3{0.5f, 1.f, 0});
        }

        auto cutscene_cameras =
            scenario->cutscene.camera_points.data(magic).value();
        for(blam::scn::cutscene_camera_position const& cam : cutscene_cameras)
        {
            // cDebug(" - Camera pos: {}", cam.position);
            auto  marker = p.create_entity(map_marker);
            auto& draw   = marker.template get<DebugDraw>();
            draw         = debug_markers.create_marker(
                std::array<Vecf3, 7>{{
                    cam.position,
                    cam.position + Vecf3{-.1f, .1f, -.1f},
                    cam.position + Vecf3{-.1f, -.1f, -.1f},
                    cam.position,
                    cam.position + Vecf3{-.1f, .1f, .1f},
                    cam.position + Vecf3{-.1f, -.1f, .1f},
                    cam.position,
                }},
                Vecf3{0.5f, 1.f, 0});
        }

        debug_markers.unmap();
    }

    void load_scenario_bsp(Proxy& p, u32 section)
    {
        ProfContext _(__FUNCTION__);

        BlamFiles<Ver>&   files      = p.template subsystem<BlamFiles<Ver>>();
        BitmapCache<Ver>& bitm_cache = p.template subsystem<BitmapCache<Ver>>();
        BSPCache<Ver>&    bsp_cache  = p.template subsystem<BSPCache<Ver>>();
        BlamResources&    gpu        = p.template subsystem<BlamResources>();
        DebugMarkers&     debug_markers = p.template subsystem<DebugMarkers>();
        ShaderCache<Ver>& shader_cache =
            p.template subsystem<ShaderCache<Ver>>();

        /* Continue load_debug_shapes' cursor: the two-arg map() would reset it
         * and overwrite the markers it just wrote. */
        debug_markers.map();
        bsp_cache.debug_markers = &debug_markers;

        auto&       container = files.container;
        auto const& magic     = container.magic;

        using namespace compo;

        {
            bsp_cache.vert_buffer    = gpu.bsp_buf->map<byte_t>(0);
            bsp_cache.element_buffer = gpu.bsp_index->map<blam::vert::face>(0);
            bsp_cache.light_buffer   = gpu.bsp_light_buf->map<byte_t>(0);
        }

        /* Start loading up vertex data */
        blam::scn::scenario<Ver> const* scenario = container.scenario().value();

        auto trigger_vols = scenario->trigger_volumes.data(magic).value();

        /* Structure BSP switching: collect the scenario's switch triggers so
         * the occluder can track the active section, and start in the section
         * the first player spawn belongs to. */
        if(auto switches = scenario->bsp_switch_triggers.data(magic);
           switches.has_value())
        {
            for(blam::scn::bsp_trigger const& sw : switches.value())
            {
                if(sw.trigger_volume < 0 ||
                   static_cast<size_t>(sw.trigger_volume) >=
                       trigger_vols.size())
                    continue;
                bsp_cache.bsp_switches.push_back({
                    .volume      = &trigger_vols[sw.trigger_volume],
                    .source      = sw.source,
                    .destination = sw.destination,
                });
                cDebug(
                    "BSP switch: {} → {} via volume '{}'",
                    sw.source.index,
                    sw.destination.index,
                    trigger_vols[sw.trigger_volume].name.str());
            }
        }
        std::vector<generation_idx_t> bsp_meshes;
        if(auto bsps = scenario->bsp_info.data(magic); bsps.has_value())
        {
            u32 i{};
            for(blam::bsp::info const& bsp : bsps.value())
            {
                cDebug("- BSP info #{}", ++i);
                bsp_meshes.push_back(bsp_cache.predict(bsp));
            }
        }

        /* Initial active section: trust the first spawn's bsp_index unless its
         * position resolves into a different section's BSP tree (b40's first
         * spawn claims section 0 but sits in section 3). */
        {
            auto locations = scenario->player_start.locations.data(magic);
            if(locations.has_value() && !locations.value().empty())
            {
                auto const& loc = locations.value()[0];
                bsp_cache.active_section =
                    static_cast<libc_types::i16>(loc.bsp_index);
                for(auto& [id, item] : bsp_cache.m_cache)
                    if(item.find_cluster_tree(loc.pos).has_value())
                    {
                        bsp_cache.active_section = item.section_idx;
                        break;
                    }
            }
            cDebug("Initial BSP section: {}", bsp_cache.active_section);
        }

        gpu.bsp_buf->unmap();
        gpu.bsp_index->unmap();
        gpu.bsp_light_buf->unmap();
        debug_markers.unmap();

        EntityRecipe bsp_ = shared_recipes::bsp;

        for(auto const& mesh_id : bsp_meshes)
        {
            auto const& bsp = bsp_cache.get(mesh_id);
            for(auto const& group : bsp.groups)
                for(BSPItem::Mesh const& mesh : group.meshes)
                {
                    auto          mesh_ent = p.create_entity(bsp_);
                    BspReference& bsp_ref =
                        mesh_ent.template get<BspReference>();

                    bsp_ref.shader         = mesh.shader;
                    bsp_ref.lightmap       = mesh.light_bitm;
                    bsp_ref.bsp            = mesh_id;
                    bsp_ref.cluster_idx    = mesh.cluster_idx;
                    bsp_ref.subcluster_idx = mesh.subcluster_idx;
                    bsp_ref.clusters       = mesh.clusters;
                    bsp_ref.bmin           = mesh.bmin;
                    bsp_ref.bmax           = mesh.bmax;
                    bsp_ref.has_bounds     = mesh.has_bounds;

                    bsp_ref.sort_center =
                        mesh.mesh ? mesh.mesh->centroid : Vecf3{0};
                    DrawState& bsp_draw = mesh_ent.template get<DrawState>();
                    bsp_draw.draw.data.push_back(mesh.draw);

                    ShaderData& shader_ = mesh_ent.template get<ShaderData>();
                    ShaderItem const& shader_it = shader_cache.get(mesh.shader);
                    shader_.shader              = shader_it.header;
                    shader_.shader_tag          = shader_it.tag;
                    shader_.shader_id           = mesh.shader;

                    bsp_draw.current_pass =
                        shader_.get_render_pass(shader_cache);
                    bsp_draw.draw.data.back().debug_identifier = fmt::format(
                        "{} {}",
                        shader_it.tag->tagclass.front().str(),
                        shader_it.tag->to_name().to_string(shader_cache.magic));
                }
            // break;
        }
        load_world_lighting(p, 0);
    }

    /* A device machine's power comes from the scenario device group it points
     * at. Most machines reference no group (index -1), and objects that are not
     * devices have none at all; both report -1 so the shader falls back rather
     * than treating them as unpowered. */
    template<typename T>
    libc_types::f32 device_power(BlamFiles<Ver>& files, T const& instance)
    {
        if constexpr(std::is_same_v<T, blam::scn::machine_spawn> ||
            std::is_same_v<T, blam::scn::light_fixture_spawn> ||
            std::is_same_v<T, blam::scn::control>)
        {
            auto const* scenario = files.container.scenario().value_or(nullptr);
            if(!scenario || instance.power_group < 0)
                return -1.f;
            auto groups =
                scenario->objects.device_groups.data(files.container.magic);
            if(groups.has_error() ||
               static_cast<size_t>(instance.power_group) >=
                   groups.value().size())
                return -1.f;
            return groups.value()[instance.power_group].initial_value;
        } else
            return -1.f;
    }

    /* One scenario object palette (scenery, vehicles, bipeds, ...). Static
     * groups are built here on every peer; dynamic ones are raised as
     * server-owned SpawnObjectEvents. */
    template<typename T>
    void load_objects(
        Proxy&                            p,
        blam::scn::reflex_group<T> const& group,
        ScenarioGroup                     group_id,
        u32                               tags)
    {
        ProfContext _(__FUNCTION__);

        BlamFiles<Ver>& files = p.template subsystem<BlamFiles<Ver>>();

        auto const& magic = files.container.magic;

        auto palette_opt   = group.palette.data(magic);
        auto instances_opt = group.instances.data(magic);
        if(palette_opt.has_error() || instances_opt.has_error())
            return;

        auto palette   = palette_opt.value();
        auto instances = instances_opt.value();

        cDebug(
            "load_objects: {} instances, {} palette entries",
            instances.size(),
            palette.size());

        bool const dynamic = group_id >= ScenarioGroup::Vehicle;

        for(u32 i = 0; i < instances.size(); ++i)
        {
            auto const net_id = SpawnObjectEvent::scenario_net_id(group_id, i);
            if(!dynamic)
            {
                build_scenario_object(p, palette, instances[i], tags, net_id);
                continue;
            }
            auto const& instance = instances[i];
            if(instance.ref < 0 ||
               static_cast<size_t>(instance.ref) >= palette.size())
                continue;
            raise_spawn(palette[instance.ref][0], instance.pos, net_id);
        }
    }

    void raise_spawn(
        blam::tagref_t const& object, Vecf3 const& position, u32 net_id)
    {
        GameEvent        ev{.type = GameEvent::SpawnObject};
        SpawnObjectEvent spawn{
            .object       = object,
            .position     = position,
            .net_id       = net_id,
            .server_owned = true,
        };
        game_bus->inject(ev, &spawn);
    }

    /* Resolve the instance's model, apply its idle animation frame, and build
     * the parent + submodel entities */
    template<typename Palette, typename T>
    void build_scenario_object(
        Proxy&         p,
        Palette const& palette,
        T const&       instance,
        u32            tags,
        u32            net_id)
    {
        using namespace compo;

        BlamFiles<Ver>&  files       = p.template subsystem<BlamFiles<Ver>>();
        ModelCache<Ver>& model_cache = p.template subsystem<ModelCache<Ver>>();

        auto const& magic = files.container.magic;

        EntityRecipe parent = shared_recipes::model;
        parent.tags         = parent.tags | tags;

        EntityRecipe submodel = shared_recipes::submodel;
        submodel.tags         = submodel.tags | (tags & SubObjectMask);

        if(instance.ref == -1 ||
           static_cast<size_t>(instance.ref) >= palette.size() ||
           !palette[instance.ref][0].valid())
            return;

        blam::tagref_t const& tagref = palette[instance.ref][0];

        auto instance_it = index.tag_of(tagref);

        if(!instance_it.has_value())
            return;

        auto const* instance_tag = *instance_it;

        if(!instance_tag->valid())
            return;

        blam::scn::object const* instance_obj =
            instance_tag->template data<blam::scn::object>(magic).value();

        auto model_it = index.find(instance_obj[0].model);

        if(model_it == index.end())
            return;

        ModelAssembly mesh_data =
            model_cache.predict_regions(instance_obj[0].model, model_lod);

        auto idle = find_idle_animation(p, instance_obj[0].anim_graph);

        /* Only objects that actually animate carry the component, so
         * static scenery pays neither the state nor a bone slot. */
        EntityRecipe recipe = parent;
        if(idle)
            recipe.components.push_back(
                compo::type_hash_v<AnimationPlayback>());

        auto parent_ = p.create_entity(recipe);
        if(idle)
            parent_.template get<AnimationPlayback>().layers[0] = *idle;

        Model&       model = parent_.template get<Model>();
        ObjectSpawn& spawn = parent_.template get<ObjectSpawn>();
        DepthInfo&   depth = parent_.template get<DepthInfo>();

        spawn.tag           = instance_tag;
        spawn.header        = &instance;
        spawn.power         = device_power(files, instance);
        model.tag           = &(*model_it);
        model.model         = mesh_data.models.at(0);
        model.origin_object = instance_tag;
        model.initialize(&instance);
        depth.position = model.position;

        NetworkInfo& netinfo = parent_.template get<NetworkInfo>();
        netinfo.object       = tagref;
        netinfo.instance_id  = net_id;

        for(auto const& model_ : mesh_data.models)
            build_submodels(p, parent_, model, model_, submodel);
    }

    /* Calls fn(group, tags) with the scenario palette group_id names */
    template<typename Fn>
    static void with_scenario_group(
        blam::scn::scenario<Ver> const& scenario, ScenarioGroup group_id, Fn&& fn)
    {
        auto const& objects = scenario.objects;
        switch(group_id)
        {
        case ScenarioGroup::Scenery:
            fn(objects.scenery, ObjectScenery | PositioningStatic);
            break;
        case ScenarioGroup::LightFixture:
            fn(objects.light_fixtures, ObjectLightFixture | PositioningStatic);
            break;
        case ScenarioGroup::Machine:
            fn(objects.machines, ObjectDevice | PositioningStatic);
            break;
        case ScenarioGroup::Control:
            fn(objects.controls,
               ObjectDevice | ObjectControl | PositioningStatic);
            break;
        case ScenarioGroup::Vehicle:
            fn(objects.vehicles, ObjectVehicle | PositioningDynamic);
            break;
        case ScenarioGroup::Biped:
            fn(objects.bipeds, ObjectBiped | PositioningDynamic);
            break;
        case ScenarioGroup::Equipment:
            fn(objects.equips, ObjectEquipment | PositioningDynamic);
            break;
        case ScenarioGroup::Weapon:
            fn(objects.weapon_spawns, ObjectEquipment | PositioningDynamic);
            break;
        default:
            break;
        }
    }

    /* The idle animation in an object's animation graph. Scans unit weapons
     * for "stand * idle*" with frame data, falling back to weapons[0] idle —
     * weapons[0] may be a vehicle-driver slot for some bipeds. */
    std::optional<AnimationLayer> find_idle_animation(
        Proxy& p, blam::tagref_t const& anim_graph)
    {
        if(!anim_graph.valid())
            return std::nullopt;

        BlamFiles<Ver>& files = p.template subsystem<BlamFiles<Ver>>();

        auto const& magic = files.container.magic;

        auto antr_it = index.find(anim_graph);
        if(antr_it == index.end())
            return std::nullopt;

        auto antr_data = (*antr_it).template data<blam::antr::header>(magic);
        if(!antr_data.has_value())
            return std::nullopt;

        auto const* antr_hdr = &antr_data.value()[0];
        u32         anim_idx = 0;

        auto all_anims_opt = antr_hdr->animations.data(magic);
        if(auto units_opt = antr_hdr->units.data(magic);
           units_opt.has_value() && !units_opt.value().empty() &&
           all_anims_opt.has_value())
        {
            auto all_anims      = all_anims_opt.value();
            u32  fallback       = 0;
            bool found_fallback = false;
            bool found_stand    = false;
            for(auto const& unit : units_opt.value())
            {
                if(found_stand)
                    break;
                auto wpn_opt = unit.weapons.data(magic);
                if(!wpn_opt.has_value())
                    continue;
                for(auto const& wpn : wpn_opt.value())
                {
                    auto ai_opt = wpn.animations.data(magic);
                    if(!ai_opt.has_value() ||
                       ai_opt.value().size() <= blam::antr::unit_weapon::idle)
                        continue;
                    i16 idx =
                        ai_opt.value()[blam::antr::unit_weapon::idle].animation;
                    if(idx < 0 || static_cast<u32>(idx) >=
                                      static_cast<u32>(all_anims.size()))
                        continue;
                    if(!found_fallback)
                    {
                        fallback       = static_cast<u32>(idx);
                        found_fallback = true;
                    }
                    auto nm = all_anims[idx].name.str();
                    if(nm.find("stand") != std::string_view::npos &&
                       nm.find("idle") != std::string_view::npos &&
                       all_anims[idx].frame_size > 0)
                    {
                        anim_idx    = static_cast<u32>(idx);
                        found_stand = true;
                        break;
                    }
                }
            }
            if(!found_stand && found_fallback)
                anim_idx = fallback;
        }

        return AnimationLayer{
            .graph     = antr_hdr,
            .animation = anim_idx,
            .loop      = true,
        };
    }

    /* One submodel entity per shaded region of a loaded model. */
    void build_submodels(
        Proxy&                                    p,
        compo::EntityRef<compo::EntityContainer>& parent_,
        Model&                                    model,
        generation_idx_t const&                   model_id,
        compo::EntityRecipe const&                submodel)
    {
        ShaderCache<Ver>& shader_cache =
            p.template subsystem<ShaderCache<Ver>>();
        ModelCache<Ver>& model_cache = p.template subsystem<ModelCache<Ver>>();

        ModelItem<Ver>& modelit = model_cache.get(model_id);
        for(auto const& sub : modelit.mesh.sub)
        {
            if(!sub.shader.valid())
            {
                cWarning(
                    "Model part dropped, shader unresolved: {}",
                    modelit.tag ? index.name_of(*modelit.tag)
                                : std::string_view("<unknown>"));
                continue;
            }

            auto submod = p.create_entity(submodel);
            model.parts.push_back(submod);
            SubModel& submod_ = submod.template get<SubModel>();

            submod_.parent      = parent_.id();
            DrawState& sub_draw = submod.template get<DrawState>();
            submod_.template initialize<Ver>(model_id, sub, sub_draw);

            ShaderData&       shader_   = submod.template get<ShaderData>();
            ShaderItem const& shader_it = shader_cache.get(sub.shader);
            shader_.initialize(shader_it, submod_);

            sub_draw.current_pass = shader_.get_render_pass(shader_cache);

            // Annotate DrawState with the shader info
            auto shader_name = index.name_of(*shader_.shader_tag);
            for(auto& draw : sub_draw.draw.data)
                draw.debug_identifier = fmt::format(
                    "{} {}",
                    shader_.shader_tag->tagclass[0].str(),
                    shader_name);
        }
    }

    /* Item permutations of a netgame equipment entry's item collection */
    using item_span =
        typename decltype(blam::scn::item_collection::items)::span_type;

    std::optional<std::pair<blam::scn::item_collection const*, item_span>>
    netgame_items(
        blam::scn::multiplayer_equipment const& entry,
        blam::map_ptr const&                    magic)
    {
        auto item_coll_tag = index.find(entry.item);
        if(item_coll_tag == index.end())
            return std::nullopt;
        auto item_coll_data =
            (*item_coll_tag).template data<blam::scn::item_collection>(magic);
        if(item_coll_data.has_error())
            return std::nullopt;
        auto const* item_coll = &item_coll_data.value()[0];
        auto        perms     = item_coll->items.data(magic);
        if(perms.has_error())
            return std::nullopt;
        return std::make_pair(item_coll, perms.value());
    }

    /* Netgame weapon/equipment spawns, which live outside the object palettes
     * and are keyed by item collections. Raised as server-owned events. */
    void load_multiplayer_equipment(Proxy& p)
    {
        BlamFiles<Ver>& files = p.template subsystem<BlamFiles<Ver>>();

        auto const& magic    = files.container.magic;
        auto const* scenario = files.container.scenario().value_or(nullptr);
        if(!scenario)
            return;

        auto equipment = scenario->netgame.equipment.data(magic);
        if(equipment.has_error())
            return;

        for(u32 e = 0; e < equipment.value().size(); ++e)
        {
            auto const& entry = equipment.value()[e];
            auto        items = netgame_items(entry, magic);
            if(!items)
                continue;
            for(u32 k = 0; k < items->second.size(); ++k)
            {
                auto const& item = items->second[k].item;
                if(item.tag_class != blam::tag_class_t::weap &&
                   item.tag_class != blam::tag_class_t::eqip)
                    continue;
                raise_spawn(
                    item,
                    entry.pos,
                    SpawnObjectEvent::scenario_net_id(
                        ScenarioGroup::NetgameEquipment, e, k));
            }
        }
    }

    void build_netgame_item(Proxy& p, u32 entry_idx, u32 perm_idx, u32 net_id)
    {
        using namespace compo;

        BlamFiles<Ver>&  files       = p.template subsystem<BlamFiles<Ver>>();
        ModelCache<Ver>& model_cache = p.template subsystem<ModelCache<Ver>>();

        auto const& magic    = files.container.magic;
        auto const* scenario = files.container.scenario().value_or(nullptr);
        if(!scenario)
            return;

        auto equipment = scenario->netgame.equipment.data(magic);
        if(equipment.has_error() || entry_idx >= equipment.value().size())
            return;
        blam::scn::multiplayer_equipment const& equipment_ref =
            equipment.value()[entry_idx];

        auto items = netgame_items(equipment_ref, magic);
        if(!items || perm_idx >= items->second.size())
            return;
        blam::scn::item_permutation const& item_perm = items->second[perm_idx];

        if(item_perm.item.tag_class != blam::tag_class_t::weap &&
           item_perm.item.tag_class != blam::tag_class_t::eqip)
            return;

        auto item_data    = index.template data<blam::scn::item>(item_perm.item);
        auto item_tag_opt = index.tag_of(item_perm.item);
        if(!item_data.has_value() || !item_tag_opt.has_value())
            return;

        blam::scn::item const& item     = *item_data.value();
        blam::tag_t const*     item_tag = *item_tag_opt;

        if(!item.model.valid())
            return;

        u32 const tags = ObjectEquipment | PositioningDynamic;

        EntityRecipe equip = shared_recipes::multiplayer_spawn;
        equip.tags         = equip.tags | tags;

        EntityRecipe submodel = shared_recipes::submodel;
        submodel.tags         = submodel.tags | (tags & SubObjectMask);

        auto              set    = p.create_entity(equip);
        Model&            model_ = set.template get<Model>();
        MultiplayerSpawn& spawn  = set.template get<MultiplayerSpawn>();

        spawn.item       = &item;
        spawn.spawn      = &equipment_ref;
        spawn.collection = items->first;
        model_.initialize(&equipment_ref);
        model_.tag           = *index.tag_of(item.model);
        model_.origin_object = item_tag;

        NetworkInfo& netinfo = set.template get<NetworkInfo>();
        netinfo.object       = item_perm.item;
        netinfo.instance_id  = net_id;

        ModelAssembly models = model_cache.predict_regions(item.model, model_lod);

        for(auto const& model : models.models)
        {
            model_.model = model;
            build_submodels(p, set, model_, model, submodel);
        }
    }

    /* A scenario-placed object addressed by its scenario net id */
    void spawn_scenario_object(Proxy& p, u32 net_id)
    {
        auto const group = SpawnObjectEvent::scenario_group(net_id);
        u32 const  idx   = (net_id >> 8) & 0xFFFF;
        u32 const  sub   = net_id & 0xFF;

        if(group == ScenarioGroup::NetgameEquipment)
            return build_netgame_item(p, idx, sub, net_id);

        BlamFiles<Ver>& files    = p.template subsystem<BlamFiles<Ver>>();
        auto const*     scenario = files.container.scenario().value_or(nullptr);
        if(!scenario)
            return;
        auto const& magic = files.container.magic;

        with_scenario_group(
            *scenario, group, [&](auto const& group_data, u32 tags) {
                auto palette   = group_data.palette.data(magic);
                auto instances = group_data.instances.data(magic);
                if(palette.has_error() || instances.has_error() ||
                   idx >= instances.value().size())
                    return;
                build_scenario_object(
                    p,
                    palette.value(),
                    instances.value()[idx],
                    tags,
                    net_id);
            });
    }

    /* Every static and dynamic object the scenario places in the world. */
    void load_scenario_objects(Proxy& p)
    {
        ProfContext _(__FUNCTION__);

        BlamFiles<Ver>& files = p.template subsystem<BlamFiles<Ver>>();

        auto const* scenario = files.container.scenario().value_or(nullptr);
        if(!scenario)
            return;

        for(auto group_id = ScenarioGroup::Scenery;
            group_id != ScenarioGroup::NetgameEquipment;
            group_id = static_cast<ScenarioGroup>(
                static_cast<u32>(group_id) + 1))
            with_scenario_group(
                *scenario, group_id, [&](auto const& group_data, u32 tags) {
                    load_objects(p, group_data, group_id, tags);
                });

        if(files.container.map->map_type == blam::maptype_t::multiplayer)
            load_multiplayer_equipment(p);
    }

    void queue_spawn(SpawnBSPEvent& bsp)
    {
    }

    void queue_spawn(SpawnBipedEvent& bsp)
    {
    }

    void queue_spawn(SpawnModelEvent& model)
    {
    }

    void queue_spawn(SkyboxChangedEvent& skybox)
    {
    }

    void queue_spawn(WeatherChangedEvent& weather)
    {
    }

    /* One model spawned on request, outside the scenario's palettes. Placed
     * at the origin; the caller positions it through its Model component. */
    void load_model(Proxy& p, blam::tagref_t const& model_ref)
    {
        using namespace compo;

        if(!model_ref.valid())
            return;

        ModelCache<Ver>& model_cache = p.template subsystem<ModelCache<Ver>>();

        auto model_it = index.find(model_ref);
        if(model_it == index.end())
            return;

        ModelAssembly mesh_data =
            model_cache.predict_regions(model_ref, model_lod);
        if(mesh_data.models.empty())
        {
            cWarning("Failed to load requested model");
            return;
        }

        EntityRecipe parent   = shared_recipes::model;
        EntityRecipe submodel = shared_recipes::submodel;

        auto   ent   = p.create_entity(parent);
        Model& model = ent.template get<Model>();

        model.tag       = &(*model_it);
        model.model     = mesh_data.models.at(0);
        model.transform = glm::identity<Matf4>();

        for(auto const& model_id : mesh_data.models)
            build_submodels(p, ent, model, model_id, submodel);
    }

    static u32 object_class_tags(blam::tag_t const& tag)
    {
        using blam::tag_class_t;
        if(tag.matches(tag_class_t::bipd))
            return ObjectBiped | ObjectUnit;
        if(tag.matches(tag_class_t::vehi))
            return ObjectVehicle | ObjectUnit;
        if(tag.matches(tag_class_t::ctrl))
            return ObjectControl;
        if(tag.matches(tag_class_t::lifi))
            return ObjectLightFixture;
        if(tag.matches(tag_class_t::mach))
            return ObjectDevice;
        if(tag.matches(tag_class_t::scen))
            return ObjectScenery;
        if(tag.matches(tag_class_t::weap) || tag.matches(tag_class_t::eqip) ||
           tag.matches(tag_class_t::garb))
            return ObjectEquipment;
        return ObjectObject;
    }

    /* Any object tag, outside the scenario's palettes. Bipeds go through
     * here like everything else; tying one to a player is a separate step. */
    void spawn_object(Proxy& p, SpawnObjectEvent const& spawn)
    {
        using namespace compo;

        if(spawn.net_id != 0)
            for(auto ent : p.template select<NetworkInfo>())
                if(ent.template get<NetworkInfo>().instance_id == spawn.net_id)
                    return;

        if(SpawnObjectEvent::scenario_group(spawn.net_id) !=
           ScenarioGroup::None)
            return spawn_scenario_object(p, spawn.net_id);

        BlamFiles<Ver>&  files       = p.template subsystem<BlamFiles<Ver>>();
        ModelCache<Ver>& model_cache = p.template subsystem<ModelCache<Ver>>();

        auto tag_it = index.find(spawn.object.tag_id);
        if(tag_it == index.end() || !(*tag_it).valid() ||
           !(*tag_it).matches(blam::tag_class_t::obje))
        {
            cWarning(
                "Cannot spawn tag_id={}: not an object", spawn.object.tag_id);
            return;
        }
        blam::tag_t const& object_tag = *tag_it;
        if(spawn.object.tag_class != blam::tag_class_t::none &&
           !object_tag.matches(spawn.object.tag_class))
        {
            cWarning(
                "Cannot spawn {}: tag class {} expected, found {}",
                index.name_of(object_tag),
                blam::to_string(spawn.object.tag_class),
                blam::to_string(object_tag.tag_class()));
            return;
        }

        auto object_data =
            object_tag.template data<blam::scn::object>(files.container.magic);
        if(object_data.has_error())
            return;
        blam::scn::object const& object = object_data.value()[0];

        auto model_it = index.find(object.model);
        if(model_it == index.end())
            return;

        ModelAssembly mesh_data =
            model_cache.predict_regions(object.model, model_lod);
        if(mesh_data.models.empty())
        {
            cWarning("Failed to load model for {}", index.name_of(object_tag));
            return;
        }

        u32 const tags = object_class_tags(object_tag);

        EntityRecipe recipe = shared_recipes::model;
        recipe.tags         = recipe.tags | tags | PositioningDynamic;

        EntityRecipe submodel = shared_recipes::submodel;
        submodel.tags         = submodel.tags | (tags & SubObjectMask);

        auto idle = find_idle_animation(p, object.anim_graph);
        if(idle)
            recipe.components.push_back(
                compo::type_hash_v<AnimationPlayback>());

        auto ent = p.create_entity(recipe);
        if(idle)
            ent.template get<AnimationPlayback>().layers[0] = *idle;

        Model& model        = ent.template get<Model>();
        model.tag           = &(*model_it);
        model.model         = mesh_data.models.at(0);
        model.origin_object = &object_tag;
        model.position      = spawn.position;
        model.rotation      = spawn.rotation;
        model.update_matrix();

        ent.template get<ObjectSpawn>().tag    = &object_tag;
        ent.template get<DepthInfo>().position = model.position;

        NetworkInfo& netinfo = ent.template get<NetworkInfo>();
        netinfo.object       = object_tag.as_ref();
        netinfo.instance_id  = spawn.net_id;

        for(auto const& model_id : mesh_data.models)
            build_submodels(p, ent, model, model_id, submodel);
    }

    void despawn_object(Proxy& p, u32 net_id)
    {
        std::set<u64> doomed;
        for(auto ent : p.template select<Model, NetworkInfo>())
        {
            auto [model, net] = ent.components();
            if(net.instance_id != net_id)
                continue;
            doomed.insert(ent.id());
            for(auto const& part : model.parts)
                doomed.insert(part.id());
        }
        if(!doomed.empty())
            p.remove_entity_if([&doomed](compo::Entity const& e) {
                return doomed.contains(e.id);
            });
    }

    /*! The biped a player spawns as on this map */
    blam::scn::biped const* player_biped(BlamFiles<Ver> const& files)
    {
        auto const& magic    = files.container.magic;
        auto        globals_ = index.tag_of("globals\\globals");
        if(!globals_.has_value())
        {
            cWarning("Failed to find globals object");
            return nullptr;
        }
        auto globals =
            (*globals_)->template data<blam::globals::globals>(magic);
        if(!globals.has_value())
            return nullptr;
        blam::tagref_typed_t<blam::tag_class_t::biped> unit{};
        if(files.container.map->map_type == blam::maptype_t::multiplayer)
            if(auto mp = globals.value()->multiplayer.data(magic);
               mp.has_value() && !mp.value().empty())
                unit = mp.value()[0].unit;
        if(!unit.valid())
            if(auto sp = globals.value()->player.data(magic);
               sp.has_value() && !sp.value().empty())
                unit = sp.value()[0].unit;
        if(auto unit_ = index.find(unit); unit_ != index.end())
            if(auto biped = unit_->template data<blam::scn::biped>(magic);
               biped.has_value())
                return biped.value();
        cWarning("Got not biped model :(");
        return nullptr;
    }

    /* Models follow biped_in_play(), and are remounted after a map load */
    void reconcile_player_bipeds(Proxy& p, BlamFiles<Ver> const& files)
    {
        LoadingStatus const* loading;
        p.subsystem(loading);
        if(loading->loaded_map != LoadingStatus::loaded)
            return;
        if(biped_model.load_generation != files.load_generation)
        {
            biped_model = {.load_generation = files.load_generation};
            if(auto const* biped = player_biped(files))
            {
                biped_model.model = biped->model;
                if(biped->collision_radius > 0.f &&
                   biped->standing_collision_height > 0.f &&
                   biped->standing_camera_height > 0.f)
                    biped_model.shape = {
                        .radius     = biped->collision_radius,
                        .height     = biped->standing_collision_height,
                        .eye_height = biped->standing_camera_height,
                    };
                else
                    cWarning("Biped has no collision size, using defaults");
            }
        }

        std::set<u64> live;
        for(auto player :
            p.template select<PlayerCamera, PlayerInfo, NetworkInfo, Model>())
        {
            auto [cam, info, net, model] = player.components();
            if(!biped_model.model.valid() || !biped_in_play(info, cam, net))
                continue;
            info.biped = biped_model.shape;
            live.insert(player.id());
            /* Once per load, so a model that fails to mount isn't retried */
            auto& biped = player_bipeds[player.id()];
            if(biped.load_generation == files.load_generation)
                continue;
            biped.load_generation = files.load_generation;
            pending_mounts.push_back({
                .model     = biped_model.model,
                .entity_id = player.id(),
            });
        }

        std::set<u64> doomed;
        for(auto it = player_bipeds.begin(); it != player_bipeds.end();)
        {
            if(live.contains(it->first))
            {
                ++it;
                continue;
            }
            doomed.insert(it->second.parts.begin(), it->second.parts.end());
            if(Model* model = p.template get<Model>(it->first))
            {
                model->parts.clear();
                model->tag   = nullptr;
                model->model = {};
            }
            it = player_bipeds.erase(it);
        }
        if(!doomed.empty())
            p.remove_entity_if([&doomed](compo::Entity const& e) {
                return doomed.contains(e.id);
            });
    }

    void mount_model(Proxy& p, MountModelEvent const& mount)
    {
        if(!mount.model.valid())
            return;
        /* The player may have left since */
        if(!p.template get<Model>(mount.entity_id))
            return;
        ModelCache<Ver>& model_cache = p.template subsystem<ModelCache<Ver>>();

        auto model_it = index.find(mount.model);
        if(model_it == index.end())
            return;

        ModelAssembly mesh = model_cache.predict_regions(
            model_it->as_ref(), blam::mod2::lod_high_ext);
        if(mesh.models.empty())
        {
            cWarning(
                "Failed to mount model {} to entity {}",
                index.name_of(mount.model),
                mount.entity_id);
            return;
        }

        auto   target = p.ref(mount.entity_id);
        Model& model  = target.template get<Model>();

        /* After a map load the old parts are already gone */
        if(!model.parts.empty())
        {
            std::set<u64> old_parts;
            for(auto const& part : model.parts)
                old_parts.insert(part.id());
            model.parts.clear();
            p.remove_entity_if([&old_parts](compo::Entity const& e) {
                return old_parts.contains(e.id);
            });
        }

        model.tag       = &(*model_it);
        model.model     = mesh.models.at(0);
        model.transform = glm::identity<Matf4>();

        for(auto const& model_id : mesh.models)
            build_submodels(
                p, target, model, model_id, shared_recipes::submodel);

        if(auto biped = player_bipeds.find(mount.entity_id);
           biped != player_bipeds.end())
        {
            biped->second.parts.clear();
            for(auto const& part : model.parts)
                biped->second.parts.push_back(part.id());
        }
    }
};

void alloc_resource_loader(compo::EntityContainer& e)
{
    auto& loader = e.register_subsystem_inplace<ResourceLoader<halo_version>>();

    auto& game_bus         = e.subsystem_cast<GameEventBus>();
    loader.game_bus        = &game_bus;
    loader.spawn_bsp_queue = game_bus.addQueuedEventFunction<SpawnBSPEvent>(
        0, [&loader](GameEvent&, SpawnBSPEvent* spawn) {
            loader.pending_bsps.push_back(*spawn);
        });
    loader.spawn_model_queue = game_bus.addQueuedEventFunction<SpawnModelEvent>(
        0, [&loader](GameEvent&, SpawnModelEvent* spawn) {
            loader.pending_models.push_back(*spawn);
        });
    loader.mount_model_queue = game_bus.addQueuedEventFunction<MountModelEvent>(
        0, [&loader](GameEvent&, MountModelEvent* spawn) {
            loader.pending_mounts.push_back(*spawn);
        });
    /* Above 0 so Networking's stamping handler runs first and the queued copy
     * carries the replicated net_id */
    loader.spawn_object_queue =
        game_bus.addQueuedEventFunction<SpawnObjectEvent>(
            100, [&loader](GameEvent&, SpawnObjectEvent* spawn) {
                if(!spawn->deferred)
                    loader.pending_objects.push_back(*spawn);
            });
    loader.despawn_object_queue =
        game_bus.addQueuedEventFunction<DespawnObjectEvent>(
            100, [&loader](GameEvent&, DespawnObjectEvent* despawn) {
                loader.pending_despawns.push_back(despawn->net_id);
            });
    loader.spawn_biped_queue = game_bus.addQueuedEventFunction<SpawnBipedEvent>(
        0, [&loader](GameEvent&, SpawnBipedEvent* spawn) {
            loader.pending_bipeds.push_back(*spawn);
        });
    loader.spawn_equip_queue =
        game_bus.addQueuedEventFunction<SpawnEquipmentEvent>(
            0, [&loader](GameEvent&, SpawnEquipmentEvent* spawn) {
                loader.pending_equipment.push_back(*spawn);
            });
    loader.cluster_queue = game_bus.addQueuedEventFunction<ClusterChangedEvent>(
        0, [&loader](GameEvent&, ClusterChangedEvent* change) {
            loader.pending_cluster_change = *change;
        });
}
