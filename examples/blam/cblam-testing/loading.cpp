#include "loading.h"
#include "bitmap_cache.h"
#include "blam/volta/blam_scenario.h"
#include "blam/volta/blam_stl.h"
#include "blam/volta/blam_tag_ref.h"
#include "blam_files.h"
#include "caching.h"
#include "components.h"
#include "data.h"
#include "map_marker.h"
#include "materials.h"
#include "selected_version.h"
#include "shader_cache.h"
#include "types.h"

template<typename Ver>
using ResourceLoaderManifest = compo::SubsystemManifest<
    type_list_t<
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
        Visibility
    >,
    type_list_t<
        BitmapCache<Ver>,
        BlamFiles<Ver>,
        BlamResources,
        BSPCache<Ver>,
        DebugMarkers,
        ModelCache<Ver>,
        ShaderCache<Ver>
    >,
    empty_list_t
>;

template<typename Ver>
struct ResourceLoader
    : compo::RestrictedSubsystem<ResourceLoader<Ver>, ResourceLoaderManifest<Ver>>
{
    using type = ResourceLoader<Ver>;
    using Proxy = compo::proxy_of<ResourceLoaderManifest<Ver>>;

    blam::tag_index_view<Ver> index;

    std::vector<SpawnBSPEvent> pending_bsps;
    std::vector<SpawnBipedEvent> pending_bipeds;
    std::vector<SpawnEquipmentEvent> pending_equipment;
    std::vector<SpawnModelEvent> pending_models;
    std::optional<ClusterChangedEvent> pending_cluster_change;

    std::shared_ptr<GameEventBus::queue_type<SpawnBSPEvent>>       spawn_bsp_queue;
    std::shared_ptr<GameEventBus::queue_type<SpawnBipedEvent>>     spawn_biped_queue;
    std::shared_ptr<GameEventBus::queue_type<SpawnEquipmentEvent>> spawn_equip_queue;
    std::shared_ptr<GameEventBus::queue_type<SpawnModelEvent>>     spawn_model_queue;
    std::shared_ptr<GameEventBus::queue_type<ClusterChangedEvent>> cluster_queue;

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
        BlamFiles<Ver>& files          = p.template subsystem<BlamFiles<Ver>>();
        if(current.load_generation != files.load_generation)
        {
            // Purge resources
            current.load_generation = files.load_generation;
            current.skybox_id  = -1;
            current.weather_id = -1;
        }
        BlamResources& resources       = p.template subsystem<BlamResources>();
        BitmapCache<Ver>& bitm_cache   = p.template subsystem<BitmapCache<Ver>>();
        BSPCache<Ver>& bsp_cache       = p.template subsystem<BSPCache<Ver>>();
        ModelCache<Ver>& model_cache   = p.template subsystem<ModelCache<Ver>>();
        ShaderCache<Ver>& shader_cache = p.template subsystem<ShaderCache<Ver>>();

        index = blam::tag_index_view<Ver>(files.container);

        cluster_queue->poll();
        // spawn_biped_queue->poll();
        spawn_bsp_queue->poll();
        // spawn_equip_queue->poll();
        spawn_model_queue->poll();

        if(!pending_cluster_change && pending_bsps.empty() &&
           pending_models.empty())
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
                    pending_cluster_change->bsp->clusters.at(
                        pending_cluster_change->cluster).cluster->sky);
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
        pending_models.clear();
    }

    /* Sun direction/colour and fog for the world UBO, taken from the
     * scenario's skybox palette. Scenario-level state, so it does not wait
     * for the objects that used to be loaded alongside it. */
    void load_world_lighting(Proxy& p, i16 skybox_id = -1)
    {
        if(skybox_id == current.skybox_id)
            return;

        BlamResources& resources       = p.template subsystem<BlamResources>();
        ShaderCache<Ver>& shader_cache = p.template subsystem<ShaderCache<Ver>>();

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

        auto& data = p.template subsystem<BlamFiles<Ver>>();
        auto& model_cache = p.template subsystem<ModelCache<Ver>>();

        auto const* scenario = data.container.scenario().value_or(nullptr);

        if(!scenario)
            return;

        auto const& magic = data.container.magic;
        blam::tag_index_view index(data.container);

        current.skybox_id = skybox_id;

        auto skyboxes = scenario->info.skyboxes.data(magic).value();
        if(skybox_id != -1 && skybox_id < skyboxes.size())
        {
            auto const& skybox = skyboxes[skybox_id];
            auto                     skybox_tag = *index.tag_of(skybox);
            blam::scn::skybox const& skybox_ =
                skybox_tag->template data<blam::scn::skybox>(magic).value()[0];

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
                /* .w carries the light's interior/exterior flags  (1 = exteriors, 2 = interiors). */
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
            world_data[0].fog.indoor_ambient =
                Vecf4(skybox_.indoor_ambient.color, skybox_.indoor_ambient.power);
            world_data[0].fog.outdoor_color =
                Vecf4(skybox_.outdoor_fog.color, skybox_.outdoor_fog.density);
            world_data[0].fog.outdoor_ambient =
                Vecf4(skybox_.outdoor_ambient.color, skybox_.outdoor_ambient.power);

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
                skybox_mod.model         = part_id;

                for(typename ModelItem<Ver>::SubModel const& region :
                    part.mesh.sub)
                {
                    if(!region.shader.valid())
                        continue;

                    auto submod = p.create_entity(shared_recipes::skybox_submodel);
                    skybox_mod.parts.push_back(submod);
                    SubModel& submodel  = submod.template get<SubModel>();
                    submodel.parent     = skybox_item.id();
                    DrawState& sub_draw = submod.template get<DrawState>();
                    submodel.initialize<Ver>(part_id, region, sub_draw);

                    ShaderData&       shader_   = submod.template get<ShaderData>();
                    ShaderItem const& shader_it = shader_cache.get(region.shader);
                    shader_.initialize(shader_it, submodel);

                    sub_draw.current_pass =
                        shader_.get_render_pass(shader_cache, true);

                    // Annotate DrawState with the shader info
                    auto shader_name = index.name_of(*shader_.shader_tag);
                    for(auto& draw : sub_draw.draw.data)
                        draw.debug_identifier = fmt::format(
                            "{} {}",
                            shader_name,
                            shader_.shader_tag->tagclass[0].str());
                }
            }
        }
    }

    void load_debug_shapes(Proxy& p)
    {
        BlamFiles<Ver>& files          = p.template subsystem<BlamFiles<Ver>>();
        DebugMarkers& debug_markers    = p.template subsystem<DebugMarkers>();

        auto& container                          = files.container;
        auto const& magic                        = container.magic;
        blam::scn::scenario<Ver> const* scenario = container.scenario().value();

        compo::EntityRecipe map_marker = shared_recipes::gc_marker;
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

        auto player_profiles = scenario->player_start.profiles.data(magic).value();
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

        BlamFiles<Ver>& files          = p.template subsystem<BlamFiles<Ver>>();
        BitmapCache<Ver>& bitm_cache   = p.template subsystem<BitmapCache<Ver>>();
        BSPCache<Ver>& bsp_cache       = p.template subsystem<BSPCache<Ver>>();
        BlamResources& gpu             = p.template subsystem<BlamResources>();
        DebugMarkers& debug_markers    = p.template subsystem<DebugMarkers>();
        ShaderCache<Ver>& shader_cache = p.template subsystem<ShaderCache<Ver>>();

        /* Continue load_debug_shapes' cursor: the two-arg map() would reset it
         * and overwrite the markers it just wrote. */
        debug_markers.map();
        bsp_cache.debug_markers = &debug_markers;

        auto& container = files.container;
        auto const& magic = container.magic;

        using namespace compo;

        {
            bsp_cache.vert_buffer    = gpu.bsp_buf->map<byte_t>(0);
            bsp_cache.element_buffer = gpu.bsp_index->map<blam::vert::face>(0);
            bsp_cache.light_buffer   = gpu.bsp_light_buf->map<byte_t>(0);
        }

        /* Start loading up vertex data */
        blam::scn::scenario<Ver> const* scenario = container.scenario().value();

        auto trigger_vols = scenario->trigger_volumes.data(magic).value();

        /* Structure BSP switching: collect the scenario's switch triggers so the
         * occluder can track the active section, and start in the section the
         * first player spawn belongs to. */
        if(auto switches = scenario->bsp_switch_triggers.data(magic);
           switches.has_value())
        {
            for(blam::scn::bsp_trigger const& sw : switches.value())
            {
                if(sw.trigger_volume < 0 ||
                   static_cast<size_t>(sw.trigger_volume) >= trigger_vols.size())
                    continue;
                bsp_cache.bsp_switches.push_back({
                    .volume      = &trigger_vols[sw.trigger_volume],
                    .source      = sw.source,
                    .destination = sw.destination,
                });
                cDebug(
                    "BSP switch: {} → {} via volume '{}'",
                    sw.source,
                    sw.destination,
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
                    BspReference& bsp_ref  = mesh_ent.template get<BspReference>();

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

                    ShaderData&       shader_   = mesh_ent.template get<ShaderData>();
                    ShaderItem const& shader_it = shader_cache.get(mesh.shader);
                    shader_.shader              = shader_it.header;
                    shader_.shader_tag          = shader_it.tag;
                    shader_.shader_id           = mesh.shader;

                    //                DepthInfo&    depth    =
                    //                mesh_ent.get<DepthInfo>(); depth.position =
                    //                bsp.

                    bsp_draw.current_pass = shader_.get_render_pass(shader_cache);
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
        if constexpr(std::is_same_v<T, blam::scn::device_machine_spawn>)
        {
            auto const* scenario = files.container.scenario().value_or(nullptr);
            if(!scenario || instance.power_group < 0)
                return -1.f;
            auto groups = scenario->objects.device_groups.data(
                files.container.magic);
            if(groups.has_error() ||
               static_cast<size_t>(instance.power_group) >=
                   groups.value().size())
                return -1.f;
            return groups.value()[instance.power_group].initial_value;
        } else
            return -1.f;
    }

    /* One scenario object palette (scenery, vehicles, bipeds, ...): resolve
     * each instance's model, apply its idle animation frame, and build the
     * parent + submodel entities. */
    template<typename T>
    void load_objects(
        Proxy& p, blam::scn::reflex_group<T> const& group, u32 tags)
    {
        ProfContext _(__FUNCTION__);

        using namespace compo;

        BlamFiles<Ver>&   files        = p.template subsystem<BlamFiles<Ver>>();
        ModelCache<Ver>&  model_cache  = p.template subsystem<ModelCache<Ver>>();
        ShaderCache<Ver>& shader_cache = p.template subsystem<ShaderCache<Ver>>();

        auto const& magic = files.container.magic;

        EntityRecipe parent = shared_recipes::model;
        parent.tags         = parent.tags | tags;

        EntityRecipe submodel = shared_recipes::submodel;
        submodel.tags         = submodel.tags | (tags & SubObjectMask);

        auto palette_opt = group.palette.data(magic);
        auto instances_opt = group.instances.data(magic);
        if(palette_opt.has_error() || instances_opt.has_error())
            return;

        auto palette   = palette_opt.value();
        auto instances = instances_opt.value();

        u32 instance_id = 0;
        cDebug(
            "load_objects: {} instances, {} palette entries",
            instances.size(),
            palette.size());

        for(T const& instance : instances)
        {
            if(instance.ref == -1 ||
               static_cast<size_t>(instance.ref) >= palette.size() ||
               !palette[instance.ref][0].valid())
                continue;

            blam::tagref_t const& tagref = palette[instance.ref][0];

            auto instance_it = index.tag_of(tagref);

            if(!instance_it.has_value())
                continue;

            auto const* instance_tag = *instance_it;

            if(!instance_tag->valid())
                continue;

            blam::scn::object const* instance_obj =
                instance_tag->template data<blam::scn::object>(magic).value();

            auto model_it = index.find(instance_obj[0].model);

            if(model_it == index.end())
                continue;

            ModelAssembly mesh_data =
                model_cache.predict_regions(instance_obj[0].model, model_lod);

            apply_idle_animation(p, instance_obj[0].anim_graph, mesh_data);

            auto         parent_ = p.create_entity(parent);
            Model&       model   = parent_.template get<Model>();
            ObjectSpawn& spawn   = parent_.template get<ObjectSpawn>();
            DepthInfo&   depth   = parent_.template get<DepthInfo>();

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
            netinfo.instance_id  = ++instance_id;

            for(auto const& model_ : mesh_data.models)
                build_submodels(p, parent_, model, model_, submodel);
        }
    }

    /* Idle animation frame 0 from an object's animation graph. Scans unit
     * weapons for "stand * idle*" with frame data, falling back to weapons[0]
     * idle — weapons[0] may be a vehicle-driver slot for some bipeds. */
    void apply_idle_animation(
        Proxy&                p,
        blam::tagref_t const& anim_graph,
        ModelAssembly const&  mesh_data)
    {
        if(!anim_graph.valid())
            return;

        BlamFiles<Ver>&  files       = p.template subsystem<BlamFiles<Ver>>();
        ModelCache<Ver>& model_cache = p.template subsystem<ModelCache<Ver>>();

        auto const& magic = files.container.magic;

        auto antr_it = index.find(anim_graph);
        if(antr_it == index.end())
            return;

        auto antr_data = (*antr_it).template data<blam::antr::header>(magic);
        if(!antr_data.has_value())
            return;

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
                       ai_opt.value().size() <=
                           blam::antr::unit_weapon::idle)
                        continue;
                    i16 idx =
                        ai_opt.value()[blam::antr::unit_weapon::idle].animation;
                    if(idx < 0 ||
                       static_cast<u32>(idx) >=
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

        u32 anim_frame_count = 0;
        if(auto ai_opt = antr_hdr->animations.data(magic);
           ai_opt.has_value() &&
           anim_idx < static_cast<u32>(ai_opt.value().size()))
            anim_frame_count =
                static_cast<u32>(ai_opt.value()[anim_idx].frame_count);

        for(auto const& mid : mesh_data.models)
        {
            model_cache.apply_animation(mid, antr_hdr, anim_idx, 0);
            auto& mitem            = model_cache.get(mid);
            mitem.antr_hdr         = antr_hdr;
            mitem.anim_idx         = anim_idx;
            mitem.anim_frame_count = anim_frame_count;
        }
    }

    /* One submodel entity per shaded region of a loaded model. */
    void build_submodels(
        Proxy&                        p,
        compo::EntityRef<compo::EntityContainer>& parent_,
        Model&                        model,
        generation_idx_t const&       model_id,
        compo::EntityRecipe const&    submodel)
    {
        ShaderCache<Ver>& shader_cache = p.template subsystem<ShaderCache<Ver>>();
        ModelCache<Ver>&  model_cache  = p.template subsystem<ModelCache<Ver>>();

        ModelItem<Ver>& modelit = model_cache.get(model_id);
        for(auto const& sub : modelit.mesh.sub)
        {
            if(!sub.shader.valid())
                continue;

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
                            shader_name,
                            shader_.shader_tag->tagclass[0].str());
        }
    }

    /* Netgame weapon/equipment spawns, which live outside the object palettes
     * and are keyed by item collections. */
    void load_multiplayer_equipment(Proxy& p, u32 tags)
    {
        using namespace compo;

        BlamFiles<Ver>&   files        = p.template subsystem<BlamFiles<Ver>>();
        ModelCache<Ver>&  model_cache  = p.template subsystem<ModelCache<Ver>>();
        ShaderCache<Ver>& shader_cache = p.template subsystem<ShaderCache<Ver>>();

        auto const& magic    = files.container.magic;
        auto const* scenario = files.container.scenario().value_or(nullptr);
        if(!scenario)
            return;

        auto equipment = scenario->netgame.equipment.data(magic);
        if(equipment.has_error())
            return;

        EntityRecipe equip = shared_recipes::multiplayer_spawn;
        equip.tags         = equip.tags | tags;

        EntityRecipe submodel = shared_recipes::submodel;
        submodel.tags         = submodel.tags | (tags & SubObjectMask);

        u32 instance_id = 0;
        for(blam::scn::multiplayer_equipment const& equipment_ref :
            equipment.value())
        {
            auto item_coll_tag = index.find(equipment_ref.item);

            if(item_coll_tag == index.end())
                continue;

            auto item_coll_data =
                (*item_coll_tag)
                    .template data<blam::scn::item_collection>(magic);
            if(item_coll_data.has_error())
                continue;
            blam::scn::item_collection const& item_coll =
                item_coll_data.value()[0];

            auto perms_opt = item_coll.items.data(magic);
            if(perms_opt.has_error())
                continue;

            for(blam::scn::item_permutation const& item_perm :
                perms_opt.value())
            {
                switch(item_perm.item.tag_class)
                {
                case blam::tag_class_t::weap:
                case blam::tag_class_t::eqip: {
                    auto item_data =
                        index.template data<blam::scn::item>(item_perm.item);
                    auto item_tag_opt = index.tag_of(item_perm.item);
                    if(!item_data.has_value() || !item_tag_opt.has_value())
                        continue;

                    blam::scn::item const& item     = *item_data.value();
                    blam::tag_t const*     item_tag = *item_tag_opt;

                    if(!item.model.valid())
                        continue;

                    auto              set    = p.create_entity(equip);
                    Model&            model_ = set.template get<Model>();
                    MultiplayerSpawn& spawn =
                        set.template get<MultiplayerSpawn>();

                    spawn.item       = &item;
                    spawn.spawn      = &equipment_ref;
                    spawn.collection = &item_coll;
                    model_.initialize(&equipment_ref);
                    model_.tag           = *index.tag_of(item.model);
                    model_.origin_object = item_tag;

                    NetworkInfo& netinfo = set.template get<NetworkInfo>();
                    netinfo.object       = item_perm.item;
                    netinfo.instance_id  = ++instance_id;

                    ModelAssembly models =
                        model_cache.predict_regions(item.model, model_lod);

                    for(auto const& model : models.models)
                    {
                        model_.model = model;
                        build_submodels(p, set, model_, model, submodel);
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
    }

    /* Every static and dynamic object the scenario places in the world. */
    void load_scenario_objects(Proxy& p)
    {
        ProfContext _(__FUNCTION__);

        BlamFiles<Ver>& files = p.template subsystem<BlamFiles<Ver>>();

        auto const* scenario = files.container.scenario().value_or(nullptr);
        if(!scenario)
            return;

        load_objects(
            p, scenario->objects.scenery, ObjectScenery | PositioningStatic);
        load_objects(
            p,
            scenario->objects.light_fixtures,
            ObjectLightFixture | PositioningStatic);
        load_objects(
            p, scenario->objects.machines, ObjectDevice | PositioningStatic);

        load_objects(
            p, scenario->objects.vehicles, ObjectVehicle | PositioningDynamic);
        load_objects(
            p, scenario->objects.bipeds, ObjectBiped | PositioningDynamic);
        load_objects(
            p, scenario->objects.equips, ObjectEquipment | PositioningDynamic);
        load_objects(
            p,
            scenario->objects.weapon_spawns,
            ObjectEquipment | PositioningDynamic);
        load_objects(
            p, scenario->objects.controls, ObjectControl | PositioningDynamic);

        if(files.container.map->map_type == blam::maptype_t::multiplayer)
            load_multiplayer_equipment(
                p, ObjectEquipment | PositioningDynamic);
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
};

void alloc_resource_loader(compo::EntityContainer& e)
{
    auto& loader = e.register_subsystem_inplace<ResourceLoader<halo_version>>();

    auto& game_bus = e.subsystem_cast<GameEventBus>();
    loader.spawn_bsp_queue = game_bus.addQueuedEventFunction<SpawnBSPEvent>(
        0, [&loader](GameEvent&, SpawnBSPEvent* spawn) {
            loader.pending_bsps.push_back(*spawn);
        });
    loader.spawn_model_queue = game_bus.addQueuedEventFunction<SpawnModelEvent>(
        0, [&loader](GameEvent&, SpawnModelEvent* spawn) {
            loader.pending_models.push_back(*spawn);
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
