#pragma once

#include "caching.h"
#include "components.h"
#include "data.h"

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
    BlamResources& gpu = e.subsystem_cast<BlamResources>();

    {
        bsp_cache.vert_buffer         = gpu.bsp_buf->map<byte_t>(0);
        bsp_cache.element_buffer      = gpu.bsp_index->map<blam::vert::face>(0);
        bsp_cache.light_buffer        = gpu.bsp_light_buf->map<byte_t>(0);
        bsp_cache.portal_buffer       = gpu.debug_lines->map<Vecf3>(0);
        bsp_cache.portal_color_buffer = gpu.debug_line_colors->map<Vecf3>(0);
    }

    /* Start loading up vertex data */
    blam::scn::scenario<Version> const* scenario = data.scenario;

    EntityRecipe trigger_obj;
    trigger_obj.components = {
        type_hash_v<TriggerVolume>(),
        type_hash_v<DebugDraw>(),
    };
    trigger_obj.tags = ObjectScriptObject | ObjectTriggerVolume | ObjectGC;

    const auto create_debug_marker =
        [&]<size_t N>(std::array<Vecf3, N> const& points, Vecf3 const& color) {
            auto vertices = bsp_cache.portal_buffer.subspan(
                bsp_cache.portal_ptr, points.size());
            std::copy(points.begin(), points.end(), vertices.begin());
            bsp_cache.portal_color_buffer[bsp_cache.portal_color_ptr] = color;

            DebugDraw draw = {
                .data =
                    {
                        .arrays =
                            {
                                .count  = points.size(),
                                .offset = bsp_cache.portal_ptr,
                            },
                    },
                .color_ptr = bsp_cache.portal_color_ptr,
            };
            bsp_cache.portal_ptr += points.size();
            bsp_cache.portal_color_ptr++;
            return draw;
        };

    auto trigger_vols = scenario->trigger_volumes.data(magic).value();
    for(blam::scn::trigger_volume const& trigger : trigger_vols)
    {
        auto [origin, second] = trigger.box.points();

        auto           trig   = e.create_entity(trigger_obj);
        TriggerVolume& volume = trig.get<TriggerVolume>();
        DebugDraw&     draw   = trig.get<DebugDraw>();

        draw = create_debug_marker(
            std::array<Vecf3, 10>{{
                origin,
                origin + Vecf3{second.x, 0, 0},
                origin + Vecf3{second.x, second.y, 0},
                origin + Vecf3{0, second.y, 0},
                origin,
                origin + Vecf3{0, 0, second.z},
                origin + Vecf3{second.x, 0, second.z},
                origin + Vecf3{second.x, second.y, second.z},
                origin + Vecf3{0, second.y, second.z},
                origin + Vecf3{0, 0, second.z},
            }},
            Vecf3(1));

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
        cDebug(" - Encounter: {}", enc.text.str());
        auto platoons         = enc.platoons.data(magic).value();
        auto firing_positions = enc.firing_positions.data(magic).value();
        auto squads           = enc.squads.data(magic).value();
        auto start_locs       = enc.start_locations.data(magic).value();
        for(auto const& platoon : platoons)
            cDebug("   - Platoon: {}", platoon.name.str());
        for(auto const& firing_pos : firing_positions)
        {
            cDebug("   - Firing pos: {}", firing_pos.position);
            auto  marker = e.create_entity(map_marker);
            auto& draw   = marker.get<DebugDraw>();

            draw = create_debug_marker(
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
            cDebug("   - Start location: {}", loc.position);
            auto  marker = e.create_entity(map_marker);
            auto& draw   = marker.get<DebugDraw>();

            draw = create_debug_marker(
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
            cDebug("   - Squad: {}", squad.name.str());
            // auto locations = squad.
        }
    }

    auto mp_flags = scenario->netgame.flags.data(magic).value();
    for(blam::scn::multiplayer_flag const& flag : mp_flags)
    {
        cDebug("MP flag: {}", flag.pos);
        auto  marker = e.create_entity(map_marker);
        auto& draw   = marker.get<DebugDraw>();

        draw = create_debug_marker(
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
        cDebug(" - Spawn: @{}", spawn.pos);
        auto  marker = e.create_entity(map_marker);
        auto& draw   = marker.get<DebugDraw>();
        draw         = create_debug_marker(
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
        draw         = create_debug_marker(
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
        draw         = create_debug_marker(
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
        for(blam::bsp::info const& bsp : bsps.value())
        {
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
        auto mesh_it = bsp_cache.find(mesh_id);
        if(mesh_it == bsp_cache.end())
        {
            cDebug("Failed to find BSP mesh: {}:{}", mesh_id.gen, mesh_id.i);
            continue;
        }
        auto const& bsp = mesh_it->second;

        for(auto const& group : bsp.groups)
            for(BSPItem::Mesh const& mesh : group.meshes)
            {
                auto          mesh_ent = e.create_entity(bsp_);
                BspReference& bsp_ref  = mesh_ent.get<BspReference>();

                bsp_ref.shader   = mesh.shader;
                bsp_ref.lightmap = mesh.light_bitm;
                bsp_ref.bsp      = mesh_id;
                bsp_ref.visible  = true;
                bsp_ref.draw.data.push_back(mesh.draw);

                ShaderData&       shader_ = mesh_ent.get<ShaderData>();
                ShaderItem const& shader_it =
                    shader_cache.find(mesh.shader)->second;
                shader_.shader     = shader_it.header;
                shader_.shader_tag = shader_it.tag;
                shader_.shader_id  = mesh.shader;

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
    parent.tags = tags;

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
    for(T const& instance : instances)
    {
        if(instance.ref == -1 || !palette[instance.ref][0].valid())
            continue;

        blam::tagref_t const& tagref = palette[instance.ref][0];

        auto instance_it = index.find(tagref);

        if(instance_it == index.end())
            continue;

        auto instance_tag = &(*instance_it);

        if(!instance_tag->valid())
            continue;

        blam::scn::object const* instance_obj =
            instance_tag->template data<blam::scn::object>(magic).value();

        auto model_it = index.find(instance_obj[0].model.to_plain());

        if(model_it == index.end())
            continue;

        ModelAssembly mesh_data = model_cache.predict_regions(
            instance_obj[0].model.to_plain(), model_lod);

        auto         parent_ = e.create_entity(parent);
        Model&       model   = parent_.get<Model>();
        ObjectSpawn& spawn   = parent_.get<ObjectSpawn>();
        DepthInfo&   depth   = parent_.get<DepthInfo>();

        spawn.tag    = instance_tag;
        spawn.header = &instance;
        model.tag    = &(*model_it);
        model.model  = mesh_data.models.at(0);
        model.initialize(&instance);
        depth.position = model.position;

        NetworkInfo& netinfo = parent_.get<NetworkInfo>();
        netinfo.object       = tagref;
        netinfo.instance_id  = ++instance_id;

        for(auto const& model_ : mesh_data.models)
        {
            ModelItem<Version> const& modelit =
                model_cache.find(model_)->second;

            for(auto const& sub : modelit.mesh.sub)
            {
                if(!sub.shader.valid())
                    continue;

                auto submod = e.create_entity(submodel);
                model.parts.push_back(submod);
                SubModel& submod_ = submod.get<SubModel>();

                submod_.parent = parent_.id();
                submod_.initialize<Version>(model_, sub);

                ShaderData&       shader_ = submod.get<ShaderData>();
                ShaderItem const& shader_it =
                    shader_cache.find(sub.shader)->second;
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
    equip.tags = tags;

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
                    (*index.find(item_perm.item))
                        .template data<blam::scn::item>(magic)
                        .value()[0];

                auto              set    = e.create_entity(equip);
                Model&            model_ = set.get<Model>();
                MultiplayerSpawn& spawn  = set.get<MultiplayerSpawn>();

                spawn.item       = &item;
                spawn.spawn      = &equipment_ref;
                spawn.collection = &item_coll;
                model_.initialize(&equipment_ref);
                model_.tag = &(*index.find(item.model));

                NetworkInfo& netinfo = set.get<NetworkInfo>();
                netinfo.object       = item_perm.item;
                netinfo.instance_id  = ++instance_id;

                ModelAssembly models = model_cache.predict_regions(
                    item.model.to_plain(), model_lod);

                for(auto const& model : models.models)
                {
                    ModelItem<Version> const& modelit =
                        model_cache.find(model)->second;
                    model_.model = model;

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
                            shader_cache.find(sub.shader)->second;
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
        auto                     skybox_tag = &(*index.find(skybox));
        blam::scn::skybox const& skybox_ =
            skybox_tag->template data<blam::scn::skybox>(magic).value()[0];

        Span<const blam::scn::skybox::light> lights =
            skybox_.lights.data(magic).value();

        for([[maybe_unused]] auto const& light : lights)
        {
            cDebug("Light: {0}", light.marker_name.str());
            Vecf3 rotation =
                glm::mat3_cast(
                    glm::quat(Vecf3{0, 0, light.radiosity.direction.x}) *
                    glm::quat(Vecf3{0, light.radiosity.direction.y, 0})) *
                Vecf3{0, 0, 1};
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

        skybox_mod.tag       = skybox_tag;
        skybox_mod.transform = glm::identity<Matf4>();

        ModelAssembly assem = model_cache.predict_regions(
            skybox_.model.to_plain(), blam::mod2::mod2_lod::lod_high_ext);

        if(assem.models.empty())
        {
            cDebug("Invalid skybox");
            continue;
        }

        skybox_mod.model = assem.models.at(0);

        for(auto const& part_id : assem.models)
        {
            ModelItem<Version> const& part = model_cache.find(part_id)->second;
            skybox_mod.model               = part_id;

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

                ShaderData&       shader_ = submod.get<ShaderData>();
                ShaderItem const& shader_it =
                    shader_cache.find(region.shader)->second;
                shader_.initialize(shader_it, submodel);

                submodel.current_pass = Pass_Sky;
            }
        }
    }

    gpu.model_buf->unmap();
    gpu.model_index->unmap();
    gpu.world_store->unmap();
}
