#pragma once

#include "caching.h"
#include "components.h"
#include "data.h"
#include "map_marker.h"

template<typename Version>
void load_scenario_bsp(
    compo::EntityContainer& e, MapChangedEvent<Version>& data)
{
    ProfContext _(__FUNCTION__);

    using namespace compo;

    auto&                 magic     = data.container.magic;
    BSPCache<Version>&    bsp_cache = e.subsystem_cast<BSPCache<Version>>();
    ShaderCache<Version>& shader_cache =
        e.subsystem_cast<ShaderCache<Version>>();
    BlamResources& gpu           = e.subsystem_cast<BlamResources>();
    DebugMarkers&  debug_markers = e.subsystem_cast<DebugMarkers>();

    {
        bsp_cache.vert_buffer    = gpu.bsp_buf->map<byte_t>(0);
        bsp_cache.element_buffer = gpu.bsp_index->map<blam::vert::face>(0);
        bsp_cache.light_buffer   = gpu.bsp_light_buf->map<byte_t>(0);

        debug_markers.portal_buffer = gpu.debug_lines->map<Vecf3>(0);
        debug_markers.portal_color_buffer =
            gpu.debug_line_colors->map<Vecf3>(0);
        debug_markers.portal_ptr       = reserved_debug_points;
        debug_markers.portal_color_ptr = reserved_debug_colors;
        bsp_cache.debug_markers        = &debug_markers;
    }

    /* Start loading up vertex data */
    blam::scn::scenario<Version> const* scenario = data.scenario;

    EntityRecipe trigger_obj;
    trigger_obj.components = {
        type_hash_v<TriggerVolume>(),
        type_hash_v<DebugDraw>(),
    };
    trigger_obj.tags = ObjectScriptObject | ObjectTriggerVolume | ObjectGC;

    auto trigger_vols = scenario->trigger_volumes.data(magic).value();
    for(blam::scn::trigger_volume const& trigger : trigger_vols)
    {
        auto [origin, second] = trigger.box.points();

        auto           trig   = e.create_entity(trigger_obj);
        TriggerVolume& volume = trig.get<TriggerVolume>();
        DebugDraw&     draw   = trig.get<DebugDraw>();

        draw = debug_markers.create_box(origin, second, Vecf3(1, 0, 0.5f));

        volume.trigger_volume = &trigger;
    }

    EntityRecipe map_marker;
    map_marker.components = {
        type_hash_v<DebugDraw>(),
    };
    map_marker.tags = ObjectGC;

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
            auto  marker = e.create_entity(map_marker);
            auto& draw   = marker.get<DebugDraw>();

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
            auto  marker = e.create_entity(map_marker);
            auto& draw   = marker.get<DebugDraw>();

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
        auto  marker = e.create_entity(map_marker);
        auto& draw   = marker.get<DebugDraw>();

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
        auto  marker = e.create_entity(map_marker);
        auto& draw   = marker.get<DebugDraw>();
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
        cDebug(" - Cutscene flag: {}", flag.position);
        auto  marker = e.create_entity(map_marker);
        auto& draw   = marker.get<DebugDraw>();
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
        cDebug(" - Camera pos: {}", cam.position);
        auto  marker = e.create_entity(map_marker);
        auto& draw   = marker.get<DebugDraw>();
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

    gpu.bsp_buf->unmap();
    gpu.bsp_index->unmap();
    gpu.bsp_light_buf->unmap();
    gpu.debug_lines->unmap();
    gpu.debug_line_colors->unmap();

    EntityRecipe bsp_;
    bsp_.components = {
        type_hash_v<BspReference>(),
        type_hash_v<ShaderData>(),
        type_hash_v<DepthInfo>(),
    };
    bsp_.tags = ObjectBsp | ObjectGC;

    for(auto const& mesh_id : bsp_meshes)
    {
        auto const& bsp = bsp_cache.get(mesh_id);
        for(auto const& group : bsp.groups)
            for(BSPItem::Mesh const& mesh : group.meshes)
            {
                auto          mesh_ent = e.create_entity(bsp_);
                BspReference& bsp_ref  = mesh_ent.get<BspReference>();

                bsp_ref.shader         = mesh.shader;
                bsp_ref.lightmap       = mesh.light_bitm;
                bsp_ref.bsp            = mesh_id;
                bsp_ref.cluster_idx    = mesh.cluster_idx;
                bsp_ref.subcluster_idx = mesh.subcluster_idx;
                bsp_ref.visible        = true;
                bsp_ref.sort_center =
                    mesh.mesh ? mesh.mesh->centroid : Vecf3{0};
                bsp_ref.draw.data.push_back(mesh.draw);

                ShaderData&       shader_   = mesh_ent.get<ShaderData>();
                ShaderItem const& shader_it = shader_cache.get(mesh.shader);
                shader_.shader              = shader_it.header;
                shader_.shader_tag          = shader_it.tag;
                shader_.shader_id           = mesh.shader;

                //                DepthInfo&    depth    =
                //                mesh_ent.get<DepthInfo>(); depth.position =
                //                bsp.

                bsp_ref.current_pass = shader_.get_render_pass(shader_cache);
            }
        // break;
    }
}

constexpr auto model_lod = blam::mod2::lod_high_ext;

template<typename T, typename Version>
void load_objects(
    blam::scn::reflex_group<T> const& group,
    MapChangedEvent<Version>&         data,
    EntityContainer&                  e,
    u32                               tags)
{
    ProfContext _(__FUNCTION__);

    using namespace compo;

    EntityRecipe parent;
    parent.components = {
        type_hash_v<Model>(),
        type_hash_v<NetworkInfo>(),
        type_hash_v<ObjectSpawn>(),
        type_hash_v<DepthInfo>(),
    };
    parent.tags = tags | ObjectGC;

    EntityRecipe submodel;
    submodel.components = {
        type_hash_v<SubModel>(),
        type_hash_v<ShaderData>(),
        type_hash_v<MeshTrackingData>(),
    };
    submodel.tags = (tags & SubObjectMask) | ObjectMod2 | ObjectGC;

    auto& model_cache  = e.subsystem_cast<ModelCache<Version>>();
    auto& shader_cache = e.subsystem_cast<ShaderCache<Version>>();

    //    auto obj_names
    //        =
    //        data.scenario->objects.object_names.data(data.container.magic);
    auto magic   = data.container.magic;
    auto index   = blam::tag_index_view(data.container);
    auto palette = group.palette.data(magic).value();

    auto instances   = group.instances.data(magic).value();
    u32  instance_id = 0;
    cDebug(
        "load_objects: {} instances, {} palette entries",
        instances.size(),
        palette.size());
    for(T const& instance : instances)
    {
        if(instance.ref == -1 || !palette[instance.ref][0].valid())
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

        /* Apply idle animation frame 0 from the object's animation graph */
        {
            blam::tagref_t const& anim_graph = instance_obj[0].anim_graph;
            if(anim_graph.valid())
            {
                auto antr_it = index.find(anim_graph);
                if(antr_it != index.end())
                {
                    auto antr_data =
                        (*antr_it)
                            .template data<blam::antr::header>(magic);
                    if(antr_data.has_value())
                    {
                        auto const* antr_hdr = &antr_data.value()[0];
                        u32         anim_idx = 0;
                        /* Find idle: scan unit weapons for "stand * idle*" with
                         * frame data; fall back to weapons[0] idle if none found.
                         * weapons[0] may be a vehicle-driver slot for some bipeds. */
                        auto all_anims_opt = antr_hdr->animations.data(magic);
                        if(auto units_opt = antr_hdr->units.data(magic);
                           units_opt.has_value() && !units_opt.value().empty() &&
                           all_anims_opt.has_value())
                        {
                            auto all_anims  = all_anims_opt.value();
                            u32  fallback   = 0;
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
                                        ai_opt.value()[blam::antr::unit_weapon::idle]
                                            .animation;
                                    if(idx < 0 ||
                                       static_cast<u32>(idx) >= static_cast<u32>(all_anims.size()))
                                        continue;
                                    if(!found_fallback)
                                    {
                                        fallback        = static_cast<u32>(idx);
                                        found_fallback  = true;
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
                        {
                            auto const& selected = ai_opt.value()[anim_idx];
                            anim_frame_count = static_cast<u32>(selected.frame_count);
                        }

                        for(auto const& mid : mesh_data.models)
                        {
                            model_cache.apply_animation(
                                mid, antr_hdr, anim_idx, 0);
                            auto& mitem           = model_cache.get(mid);
                            mitem.antr_hdr        = antr_hdr;
                            mitem.anim_idx        = anim_idx;
                            mitem.anim_frame_count = anim_frame_count;
                        }
                    }
                }
            }
        }

        auto         parent_ = e.create_entity(parent);
        Model&       model   = parent_.get<Model>();
        ObjectSpawn& spawn   = parent_.get<ObjectSpawn>();
        DepthInfo&   depth   = parent_.get<DepthInfo>();

        spawn.tag           = instance_tag;
        spawn.header        = &instance;
        model.tag           = &(*model_it);
        model.model         = mesh_data.models.at(0);
        model.origin_object = instance_tag;
        model.initialize(&instance);
        depth.position = model.position;

        auto mod_name = model.tag->to_name().to_string(magic);
        auto tag_name = instance_tag->to_name().to_string(magic);

        NetworkInfo& netinfo = parent_.get<NetworkInfo>();
        netinfo.object       = tagref;
        netinfo.instance_id  = ++instance_id;

        for(auto const& model_ : mesh_data.models)
        {
            ModelItem<Version>& modelit = model_cache.get(model_);
            for(auto const& sub : modelit.mesh.sub)
            {
                if(!sub.shader.valid())
                    continue;

                auto submod = e.create_entity(submodel);
                model.parts.push_back(submod);
                SubModel& submod_ = submod.get<SubModel>();

                submod_.parent = parent_.id();
                submod_.initialize<Version>(model_, sub);

                ShaderData&       shader_   = submod.get<ShaderData>();
                ShaderItem const& shader_it = shader_cache.get(sub.shader);
                shader_.initialize(shader_it, submod_);

                submod_.current_pass = shader_.get_render_pass(shader_cache);
            }
        }
    }
}

template<typename Version>
void load_multiplayer_equipment(
    MapChangedEvent<Version>& data, EntityContainer& e, u32 tags)
{
    using namespace compo;

    blam::tag_index_view index(data.container);
    auto const&          magic = data.container.magic;

    auto equipment = data.scenario->netgame.equipment.data(magic);

    if(equipment.has_error())
        return;

    auto& model_cache  = e.subsystem_cast<ModelCache<Version>>();
    auto& shader_cache = e.subsystem_cast<ShaderCache<Version>>();

    EntityRecipe equip;
    equip.components = {
        type_hash_v<Model>(),
        type_hash_v<NetworkInfo>(),
        type_hash_v<MultiplayerSpawn>(),
    };
    equip.tags = tags | ObjectGC;

    EntityRecipe submodel;
    submodel.components = {
        type_hash_v<SubModel>(),
        type_hash_v<ShaderData>(),
        type_hash_v<MeshTrackingData>(),
    };
    submodel.tags = (tags & SubObjectMask) | ObjectMod2 | ObjectGC;

    u32 instance_id = 0;
    for(blam::scn::multiplayer_equipment const& equipment_ref :
        equipment.value())
    {
        auto item_coll_tag = index.find(equipment_ref.item);

        if(item_coll_tag == index.end())
            continue;

        blam::scn::item_collection const& item_coll =
            (*item_coll_tag)
                .template data<blam::scn::item_collection>(magic)
                .value()[0];

        auto perms = item_coll.items.data(magic).value();
        for(blam::scn::item_permutation const& item_perm : perms)
        {
            switch(item_perm.item.tag_class)
            {
            case blam::tag_class_t::weap:
            case blam::tag_class_t::eqip: {
                blam::scn::item const& item =
                    *index.template data<blam::scn::item>(item_perm.item)
                         .value();
                blam::tag_t const* item_tag =
                    index.tag_of(item_perm.item).value();

                if(!item.model.valid())
                    continue;

                auto              set    = e.create_entity(equip);
                Model&            model_ = set.get<Model>();
                MultiplayerSpawn& spawn  = set.get<MultiplayerSpawn>();

                spawn.item       = &item;
                spawn.spawn      = &equipment_ref;
                spawn.collection = &item_coll;
                model_.initialize(&equipment_ref);
                model_.tag           = *index.tag_of(item.model);
                model_.origin_object = item_tag;

                NetworkInfo& netinfo = set.get<NetworkInfo>();
                netinfo.object       = item_perm.item;
                netinfo.instance_id  = ++instance_id;

                ModelAssembly models =
                    model_cache.predict_regions(item.model, model_lod);

                for(auto const& model : models.models)
                {
                    ModelItem<Version>& modelit = model_cache.get(model);
                    model_.model                = model;

                    for(auto const& sub : modelit.mesh.sub)
                    {
                        if(!sub.shader.valid())
                            continue;
                        auto submod = e.create_entity(submodel);
                        model_.parts.push_back(submod);
                        SubModel& submod_ = submod.get<SubModel>();
                        submod_.parent    = set.id();
                        submod_.initialize<Version>(model, sub);

                        ShaderData&       shader_ = submod.get<ShaderData>();
                        ShaderItem const& shader_it =
                            shader_cache.get(sub.shader);
                        shader_.initialize(shader_it, submod_);

                        submod_.current_pass =
                            shader_.get_render_pass(shader_cache);
                    }
                }
                break;
            }
            default:
                break;
            }
        }
    }
}

template<typename Version>
void preload_player_bipeds(EntityContainer& e, MapChangedEvent<Version>& data)
{
    blam::tag_index_view tag_index(data.container);
    ModelCache<Version>* model_cache;
    e.subsystem(model_cache);

    typename decltype(tag_index)::iterator bipeds[2] = {
        tag_index.find("characters\\cyborg_mp\\cyborg_mp"),
        tag_index.find("characters\\cyborg\\cyborg")};
    for(u32 i = 0; i < 2; ++i)
    {
        auto it = bipeds[i];
        if(it == tag_index.end())
            continue;
        blam::scn::biped const* biped =
            (*it).template data<blam::scn::biped>(data.container.magic);
        model_cache->predict_regions(biped->model);
    }
}

template<typename Version>
void load_scenario_scenery(EntityContainer& e, MapChangedEvent<Version>& data)
{
    ProfContext _(__FUNCTION__);

    auto&          model_cache  = e.subsystem_cast<ModelCache<Version>>();
    auto&          shader_cache = e.subsystem_cast<ShaderCache<Version>>();
    BlamResources& gpu          = e.subsystem_cast<BlamResources>();

    {
        ProfContext _("Buffer mapping");
        auto        vert           = gpu.model_buf->map(0);
        auto        index          = gpu.model_index->map(0);
        model_cache.vert_buffer    = Bytes::ofContainer(vert);
        model_cache.element_buffer = Bytes::ofContainer(index);
    }

    blam::scn::scenario<Version> const* scenario = data.scenario;
    auto                                magic    = data.container.magic;

    auto pipeline = gpu.model_pipeline;

    load_objects(
        scenario->objects.scenery, data, e, ObjectScenery | PositioningStatic);
    load_objects(
        scenario->objects.light_fixtures,
        data,
        e,
        ObjectLightFixture | PositioningStatic);
    load_objects(
        scenario->objects.machines, data, e, ObjectDevice | PositioningStatic);

    load_objects(
        scenario->objects.vehicles,
        data,
        e,
        ObjectVehicle | PositioningDynamic);
    load_objects(
        scenario->objects.bipeds, data, e, ObjectBiped | PositioningDynamic);
    load_objects(
        scenario->objects.equips,
        data,
        e,
        ObjectEquipment | PositioningDynamic);
    load_objects(
        scenario->objects.weapon_spawns,
        data,
        e,
        ObjectEquipment | PositioningDynamic);
    load_objects(
        scenario->objects.controls,
        data,
        e,
        ObjectControl | PositioningDynamic);

    if(data.container.map->map_type == blam::maptype_t::multiplayer)
    {
        load_multiplayer_equipment(
            data, e, ObjectEquipment | PositioningDynamic);
        // scenario->mp.player_start_profiles;
    }

    blam::tag_index_view index(data.container);

    using namespace compo;

    EntityRecipe skybox_base;
    skybox_base.tags       = ObjectSkybox | ObjectGC;
    skybox_base.components = {
        type_hash_v<Model>(),
        type_hash_v<Light>(),
    };
    EntityRecipe skybox_model;
    skybox_model.tags       = ObjectSkybox | ObjectMod2 | ObjectGC;
    skybox_model.components = {
        type_hash_v<SubModel>(),
        type_hash_v<ShaderData>(),
        type_hash_v<MeshTrackingData>(),
    };

    auto   skybox_ent = e.create_entity(skybox_base);
    Model& skybox_mod = skybox_ent.get<Model>();

    Span<materials::world_data> world_data =
        gpu.world_store->map<materials::world_data>(0);

    auto skyboxes = data.scenario->info.skyboxes.data(magic).value();
    for(auto const& skybox : skyboxes)
    {
        auto                     skybox_tag = *index.tag_of(skybox);
        blam::scn::skybox const& skybox_ =
            skybox_tag->template data<blam::scn::skybox>(magic).value()[0];

        Span<const blam::scn::skybox::light> lights =
            skybox_.lights.data(magic).value();

        for([[maybe_unused]] auto const& light : lights)
        {
            // direction: {yaw, pitch} in radians. Halo uses Y-up:
            // Y = sin(pitch) (elevation), X/Z are horizontal plane.
            f32   yaw   = light.radiosity.direction.x;
            f32   pitch = light.radiosity.direction.y;
            Vecf3 rotation{
                std::cos(pitch) * std::sin(yaw),
                std::sin(pitch),
                std::cos(pitch) * std::cos(yaw),
            };
            world_data[0].lighting[0].light_direction = Vecf4{
                rotation,
                light.radiosity.test_distance,
            };
            world_data[0].lighting[0].light_color = Vecf4{
                light.radiosity.color,
                light.radiosity.power,
            };
            // TODO: Find out how objects are identified as being
            // interior or exterior in the world
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

        if(skybox_.model.valid())
            skybox_mod.tag = *index.tag_of(skybox_.model);
        skybox_mod.origin_object = skybox_tag;
        skybox_mod.transform     = glm::identity<Matf4>();

        ModelAssembly assem = model_cache.predict_regions(
            skybox_.model, blam::mod2::mod2_lod::lod_high_ext);

        if(assem.models.empty())
        {
            cDebug("Invalid skybox");
            continue;
        }

        skybox_mod.model = assem.models.at(0);

        for(auto const& part_id : assem.models)
        {
            ModelItem<Version>& part = model_cache.get(part_id);
            skybox_mod.model         = part_id;

            for(typename ModelItem<Version>::SubModel const& region :
                part.mesh.sub)
            {
                if(!region.shader.valid())
                    continue;

                auto submod = e.create_entity(skybox_model);
                skybox_mod.parts.push_back(submod);
                SubModel& submodel = submod.get<SubModel>();
                submodel.parent    = skybox_ent.id();
                submodel.initialize<Version>(part_id, region);

                ShaderData&       shader_   = submod.get<ShaderData>();
                ShaderItem const& shader_it = shader_cache.get(region.shader);
                shader_.initialize(shader_it, submodel);

                submodel.current_pass =
                    shader_.get_render_pass(shader_cache, true);
            }
        }
    }

    gpu.model_buf->unmap();
    gpu.model_index->unmap();
    gpu.world_store->unmap();
}
