#pragma once

#include "caching.h"
#include "components.h"
#include "data.h"

template<typename Version>
void load_scenario_bsp(compo::EntityContainer& e, BlamData<Version>& data)
{
    ProfContext _(__FUNCTION__);

    using namespace Components;

    auto&                 magic     = data.map_container.magic;
    BSPCache<Version>&    bsp_cache = e.subsystem_cast<BSPCache<Version>>();
    ShaderCache<Version>& shader_cache
        = e.subsystem_cast<ShaderCache<Version>>();
    BlamResources& gpu = e.subsystem_cast<BlamResources>();

    {
        auto vert                = gpu.bsp_buf->map(0);
        auto index               = gpu.bsp_index->map(0);
        auto light               = gpu.bsp_light_buf->map(0);
        auto lines               = gpu.debug_lines->map(0);
        bsp_cache.vert_buffer    = Bytes::ofContainer(vert);
        bsp_cache.element_buffer = Bytes::ofContainer(index);
        bsp_cache.light_buffer   = Bytes::ofContainer(light);
        bsp_cache.portal_buffer
            = semantic::mem_chunk<Vecf3>::ofContainer(lines);
    }

    Vector<generation_idx_t> bsp_meshes;

    /* Start loading up vertex data */
    blam::scn::scenario<Version> const* scenario = data.scenario;
    for(auto const& bsp : scenario->bsp_info.data(magic).value())
    {
        bsp_meshes.push_back(bsp_cache.predict(bsp));
        break;
    }

    gpu.bsp_buf->unmap();
    gpu.bsp_index->unmap();
    gpu.bsp_light_buf->unmap();
    gpu.debug_lines->unmap();

    EntityRecipe bsp;
    bsp.components = {
        type_hash_v<BspReference>(),
        type_hash_v<ShaderData>(),
    };
    bsp.tags = ObjectBsp | ObjectGC;

    for(auto const& mesh_id : bsp_meshes)
    {
        if(!mesh_id.valid())
            continue;

        auto const& mesh = bsp_cache.find(mesh_id)->second;

        for(auto const& group : mesh.groups)
            for(BSPItem::Mesh const& mesh : group.meshes)
            {
                if(!mesh.shader.valid())
                    continue;

                auto          mesh_ent = e.create_entity(bsp);
                BspReference& bsp_ref  = mesh_ent.get<BspReference>();

                bsp_ref.shader   = mesh.shader;
                bsp_ref.lightmap = mesh.light_bitm;
                bsp_ref.bsp      = mesh_id;
                bsp_ref.visible  = true;
                bsp_ref.draw.data.push_back(mesh.draw);
                bsp_ref.current_pass = Pass_Opaque;
                bsp_ref.draw.call    = {
                       .indexed   = true,
                       .instanced = true,
                };
                bsp_ref.shader = mesh.shader;

                ShaderData&       shader_ = mesh_ent.get<ShaderData>();
                ShaderItem const& shader_it
                    = shader_cache.find(mesh.shader)->second;
                shader_.shader     = shader_it.header;
                shader_.shader_tag = shader_it.tag;
                shader_.shader_id  = mesh.shader;

                bsp_ref.current_pass = shader_.get_render_pass(shader_cache);
            }
    }
}

template<typename T, typename Version>
void load_objects(
    blam::scn::reflex_group<T> const& group,
    BlamData<Version>&                data,
    EntityContainer&                  e,
    u32                               tags)
{
    ProfContext _(__FUNCTION__);

    using namespace Components;

    EntityRecipe parent;
    parent.components = {
        type_hash_v<Model>(),
        type_hash_v<ObjectSpawn>(),
    };
    parent.tags = tags;

    EntityRecipe submodel;
    submodel.components = {
        type_hash_v<SubModel>(),
        type_hash_v<ShaderData>(),
        type_hash_v<MeshTrackingData>(),
    };
    submodel.tags = tags | ObjectMod2;

    auto& model_cache  = e.subsystem_cast<ModelCache<Version>>();
    auto& shader_cache = e.subsystem_cast<ShaderCache<Version>>();

    //    auto obj_names
    //        =
    //        data.scenario->objects.object_names.data(data.map_container.magic);
    auto magic   = data.map_container.magic;
    auto index   = blam::tag_index_view(data.map_container);
    auto palette = group.palette.data(magic).value();

    for(T const& instance : group.instances.data(magic).value())
    {
        if(instance.ref == -1 || !palette[instance.ref][0].valid())
            continue;

        auto instance_it = index.find(palette[instance.ref][0]);

        if(instance_it == index.end())
            continue;

        auto instance_tag = &(*instance_it);

        if(!instance_tag->valid())
            continue;

        auto instance_obj
            = instance_tag->template data<blam::scn::object>(magic).value();

        ModelAssembly mesh_data = model_cache.predict_regions(
            instance_obj[0].model.to_plain(), blam::mod2::lod_low_ext);

        auto         parent_ = e.create_entity(parent);
        Model&       model   = parent_.get<Model>();
        ObjectSpawn& spawn   = parent_.get<ObjectSpawn>();

        spawn.tag    = instance_tag;
        spawn.header = &instance;
        model.tag    = &(*index.find(instance_obj[0].model.to_plain()));
        model.initialize(&instance);

        for(auto const& model_ : mesh_data.models)
        {
            ModelItem<Version> const& modelit
                = model_cache.find(model_.at(blam::mod2::lod_low_ext))->second;

            for(auto const& sub : modelit.mesh.sub)
            {
                if(!sub.shader.valid())
                    continue;

                auto submod = e.create_entity(submodel);
                model.models.push_back(submod);
                SubModel& submod_ = submod.get<SubModel>();

                submod_.parent = parent_.id();
                submod_.initialize<Version>(
                    model_.at(blam::mod2::lod_low_ext), sub);

                ShaderData&       shader_ = submod.get<ShaderData>();
                ShaderItem const& shader_it
                    = shader_cache.find(sub.shader)->second;
                shader_.initialize(shader_it, submod_);

                submod_.current_pass = shader_.get_render_pass(shader_cache);
            }
        }
    }
}

template<typename Version>
void load_multiplayer_equipment(
    BlamData<Version>& data,

    EntityContainer& e,
    u32              tags)
{
    using namespace Components;

    blam::tag_index_view index(data.map_container);
    auto const&          magic = data.map_container.magic;

    auto equipment = data.scenario->mp.equipment.data(magic);

    if(equipment.has_error())
        return;

    auto& model_cache  = e.subsystem_cast<ModelCache<Version>>();
    auto& shader_cache = e.subsystem_cast<ShaderCache<Version>>();

    EntityRecipe equip;
    equip.components = {
        type_hash_v<Model>(),
        type_hash_v<MultiplayerSpawn>(),
    };
    equip.tags = tags;

    EntityRecipe submodel;
    submodel.components = {
        type_hash_v<SubModel>(),
        type_hash_v<ShaderData>(),
        type_hash_v<MeshTrackingData>(),
    };
    submodel.tags = tags | ObjectMod2;

    for(blam::scn::multiplayer_equipment const& equipment_ref :
        equipment.value())
    {
        auto item_coll_tag = index.find(equipment_ref.item);

        if(item_coll_tag == index.end())
            continue;

        blam::scn::item_collection const& item_coll
            = (*item_coll_tag)
                  .template data<blam::scn::item_collection>(magic)
                  .value()[0];

        for(blam::scn::item_permutation const& item_perm :
            item_coll.items.data(magic).value())
        {
            switch(item_perm.item.tag_class)
            {
            case blam::tag_class_t::weap:
            case blam::tag_class_t::eqip: {
                blam::scn::item const& item
                    = (*index.find(item_perm.item))
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

                ModelAssembly models = model_cache.predict_regions(
                    item.model.to_plain(), blam::mod2::lod_low_ext);

                for(auto const& model : models.models)
                {
                    ModelItem<Version> const& modelit
                        = model_cache.find(model.at(blam::mod2::lod_low_ext))
                              ->second;

                    for(auto const& sub : modelit.mesh.sub)
                    {
                        auto submod = e.create_entity(submodel);
                        model_.models.push_back(submod);
                        SubModel& submod_ = submod.get<SubModel>();
                        submod_.parent    = set.id();
                        submod_.initialize<Version>(
                            model.at(blam::mod2::lod_low_ext), sub);

                        ShaderData&       shader_ = submod.get<ShaderData>();
                        ShaderItem const& shader_it
                            = shader_cache.find(sub.shader)->second;
                        shader_.initialize(shader_it, submod_);

                        submod_.current_pass
                            = shader_.get_render_pass(shader_cache);
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
void load_scenario_scenery(EntityContainer& e, BlamData<Version>& data)
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
    auto                                magic    = data.map_container.magic;

    auto pipeline = gpu.model_pipeline;

    load_objects(scenario->objects.scenery, data, e, ObjectScenery | ObjectGC);
    load_objects(scenario->objects.vehicles, data, e, ObjectVehicle | ObjectGC);
    load_objects(scenario->objects.bipeds, data, e, ObjectBiped | ObjectGC);
    load_objects(scenario->objects.equips, data, e, ObjectEquipment | ObjectGC);
    load_objects(
        scenario->objects.weapon_spawns, data, e, ObjectEquipment | ObjectGC);
    load_objects(scenario->objects.machines, data, e, ObjectDevice | ObjectGC);
    load_objects(
        scenario->objects.light_fixtures,
        data,
        e,
        ObjectLightFixture | ObjectGC);

    load_multiplayer_equipment(data, e, ObjectEquipment | ObjectGC);

    blam::tag_index_view index(data.map_container);

    using namespace Components;

    EntityRecipe skybox_base;
    skybox_base.tags       = ObjectSkybox | ObjectGC;
    skybox_base.components = {
        type_hash_v<Model>(),
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

    for(auto const& skybox : data.scenario->info.skyboxes.data(magic).value())
    {
        auto        skybox_tag = &(*index.find(skybox));
        auto const& skybox_
            = skybox_tag->template data<blam::scn::skybox>(magic).value()[0];

        skybox_mod.tag = skybox_tag;

        auto model = model_cache.predict(skybox_.model.to_plain(), 0);

        if(!model.valid())
        {
            cDebug("Invalid skybox");
            continue;
        }

        ModelItem<Version>& model_ = model_cache.find(model)->second;

        for(auto const& sub : model_.mesh.sub)
        {
            auto      submod  = e.create_entity(skybox_model);
            SubModel& submod_ = submod.get<SubModel>();

            submod_.initialize<Version>(model, sub);
            skybox_mod.models.push_back(submod);
            submod_.parent = skybox_ent.id();

            ShaderData& shader  = submod.get<ShaderData>();
            ShaderItem& shader_ = shader_cache.find(sub.shader)->second;

            shader.initialize(shader_, submod_);
        }

        cDebug("Skybox");
    }

    gpu.model_buf->unmap();
    gpu.model_index->unmap();
}
