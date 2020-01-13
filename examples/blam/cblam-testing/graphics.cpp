#include "data.h"
#include "rendering.h"
#include "resource_creation.h"

using halo_version = blam::pc_version_t;

template<typename Version>
void load_scenario_bsp(EntityContainer& e, BlamData<Version>& data)
{
    ProfContext _(__PRETTY_FUNCTION__);

    using namespace Components;

    auto& magic = data.map_container.magic;

    data.bsp_cache.vert_buffer    = data.bsp_buf->map();
    data.bsp_cache.element_buffer = data.bsp_index->map();

    Vector<generation_idx_t> bsp_meshes;

    /* Start loading up vertex data */
    auto scenario = data.scenario;
    for(auto const& bsp : scenario->bsp_info.data(magic))
    {
        bsp_meshes.push_back(data.bsp_cache.predict(bsp));
    }

    data.bsp_buf->unmap();
    data.bsp_index->unmap();

    EntityRecipe bsp;
    bsp.components = {type_hash_v<BspTag>(), type_hash_v<ShaderTag>()};
    bsp.tags       = ObjectBsp;

    for(auto const& mesh_id : bsp_meshes)
    {
        if(!mesh_id.valid())
            continue;

        auto const& mesh = data.bsp_cache.find(mesh_id)->second;

        for(auto const& group : mesh.groups)
            for(BSPItem::Mesh const& mesh : group.meshes)
            {
                if(!mesh.shader.valid())
                    continue;

                auto          mesh_ent = e.ref(e.create_entity(bsp));
                BspReference& bsp_ref  = mesh_ent.get<BspTag>();

                bsp_ref.shader       = mesh.shader;
                bsp_ref.bsp          = mesh_id;
                bsp_ref.visible      = true;
                bsp_ref.draw.draw    = mesh.draw;
                bsp_ref.current_pass = Pass_Opaque;
                bsp_ref.draw.call =
                    GFX::D_CALL().withIndexing().withInstancing();
                bsp_ref.shader = mesh.shader;

                ShaderData&       shader_ = mesh_ent.get<ShaderTag>();
                ShaderItem const& shader_it =
                    data.shader_cache.find(mesh.shader)->second;
                shader_.shader     = shader_it.header;
                shader_.shader_tag = shader_it.tag;
                shader_.shader_id  = mesh.shader;

                bsp_ref.current_pass =
                    shader_.get_render_pass(data.shader_cache);
            }
    }
}

template<typename T, typename Version>
void load_objects(
    blam::reflex_group<T> const& group,
    BlamData<Version>&           data,
    EntityContainer&             e,
    u32                          tags)
{
    ProfContext _(__PRETTY_FUNCTION__);

    using namespace Components;

    EntityRecipe parent;
    parent.components = {type_hash_v<ModelTag>(),
                         type_hash_v<ObjectSpawnTag>()};
    parent.tags       = tags;

    EntityRecipe submodel;
    submodel.components = {type_hash_v<SubModelTag>(),
                           type_hash_v<ShaderTag>()};
    submodel.tags       = tags | ObjectMod2;

    auto obj_names = data.scenario->object_names.data(data.map_container.magic);
    auto magic     = data.map_container.magic;
    auto index     = blam::tag_index_view(data.map_container);
    auto palette   = group.palette.data(magic);

    for(T const& instance : group.instances.data(magic))
    {
        auto instance_tag = (*index.find(palette[instance.ref][0]));

        if(!instance_tag->valid())
            continue;

        auto instance_obj =
            instance_tag->template to_reflexive<blam::scn::object>().data(
                magic);

        ModelAssembly mesh_data =
            data.model_cache.predict_regions(instance_obj[0].model.to_plain());

        auto         parent_ = e.ref(e.create_entity(parent));
        Model&       model   = parent_.get<ModelTag>();
        ObjectSpawn& spawn   = parent_.get<ObjectSpawnTag>();

        spawn.tag    = instance_tag;
        spawn.header = &instance;
        model.tag    = *index.find(instance_obj[0].model.to_plain());
        model.initialize(&instance);

        for(auto const& model_ : mesh_data.models)
        {
            ModelItem const& modelit =
                data.model_cache.find(model_.at(blam::mod2::lod_high_ext))
                    ->second;

            for(auto const& sub : modelit.mesh.sub)
            {
                if(!sub.shader.valid())
                    continue;

                auto submod = e.ref(e.create_entity(submodel));
                model.models.push_back(submod);
                SubModel& submod_ = submod.get<SubModelTag>();

                submod_.parent = parent_;
                submod_.shader = sub.shader;
                submod_.initialize(
                    model_.at(blam::mod2::lod_high_ext), sub, {});

                ShaderData&       shader_ = submod.get<ShaderTag>();
                ShaderItem const& shader_it =
                    data.shader_cache.find(sub.shader)->second;
                shader_.shader     = shader_it.header;
                shader_.shader_tag = shader_it.tag;
                shader_.shader_id  = sub.shader;

                submod_.current_pass =
                    shader_.get_render_pass(data.shader_cache);
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

    EntityRecipe equip;
    equip.components = {type_hash_v<ModelTag>(),
                        type_hash_v<MultiplayerSpawnTag>()};
    equip.tags       = tags;

    EntityRecipe submodel;
    submodel.components = {type_hash_v<SubModelTag>()};
    submodel.tags       = tags | ObjectMod2;

    for(blam::scn::multiplayer_equipment const& equipment_ref :
        data.scenario->mp_equipment.data(magic))
    {
        auto item_coll_tag = index.find(equipment_ref.item);

        if(item_coll_tag == index.end())
            continue;

        blam::scn::item_collection const& item_coll =
            (*item_coll_tag)
                ->to_reflexive<blam::scn::item_collection>()
                .data(magic)[0];

        for(blam::scn::item_permutation const& item_perm :
            item_coll.items.data(magic))
        {
            switch(item_perm.item.tag_class)
            {
            case blam::tag_class_t::weap:
            case blam::tag_class_t::eqip:
            {
                blam::scn::item const& item =
                    (*index.find(item_perm.item))
                        ->to_reflexive<blam::scn::item>()
                        .data(magic)[0];

                auto              set    = e.ref(e.create_entity(equip));
                Model&            model_ = set.get<ModelTag>();
                MultiplayerSpawn& spawn  = set.get<MultiplayerSpawnTag>();

                spawn.item       = &item;
                spawn.spawn      = &equipment_ref;
                spawn.collection = &item_coll;
                model_.initialize(&equipment_ref);
                model_.tag = *index.find(item.model.to_plain());

                ModelAssembly models =
                    data.model_cache.predict_regions(item.model.to_plain());

                for(auto const& model : models.models)
                {
                    ModelItem const& modelit =
                        data.model_cache
                            .find(model.at(blam::mod2::lod_high_ext))
                            ->second;

                    for(auto const& sub : modelit.mesh.sub)
                    {
                        auto submod = e.ref(e.create_entity(submodel));
                        model_.models.push_back(submod);
                        SubModel& submod_ = submod.get<SubModelTag>();
                        submod_.parent    = set;
                        submod_.initialize(
                            model.at(blam::mod2::lod_high_ext), sub, {});
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
    ProfContext _(__PRETTY_FUNCTION__);

    {
        ProfContext _("Buffer mapping");
        data.model_cache.vert_buffer    = data.model_buf->map();
        data.model_cache.element_buffer = data.model_index->map();
    }

    auto scenario = data.scenario;
    auto magic    = data.map_container.magic;

    auto pipeline = data.model_pipeline;

    pipeline->build_state();

    load_objects(scenario->scenery, data, e, ObjectScenery);
    load_objects(scenario->vehicles, data, e, ObjectVehicle);
    load_objects(scenario->bipeds, data, e, ObjectBiped);
    load_objects(scenario->equips, data, e, ObjectEquipment);
    load_objects(scenario->weapon_spawns, data, e, ObjectEquipment);
    load_objects(scenario->machines, data, e, ObjectDevice);
    load_objects(scenario->light_fixtures, data, e, ObjectLightFixture);

    load_multiplayer_equipment(data, e, ObjectEquipment);

    blam::tag_index_view index(data.map_container);

    using namespace Components;

    EntityRecipe skybox_base;
    skybox_base.tags       = ObjectSkybox;
    skybox_base.components = {type_hash_v<ModelTag>()};
    EntityRecipe skybox_model;
    skybox_model.tags       = ObjectSkybox | ObjectMod2;
    skybox_model.components = {type_hash_v<SubModelTag>()};

    auto   skybox_ent = e.ref(e.create_entity(skybox_base));
    Model& skybox_mod = skybox_ent.get<ModelTag>();

    for(auto const& skybox : data.scenario->skyboxes.data(magic))
    {
        auto        skybox_tag = *index.find(skybox);
        auto const& skybox_ =
            skybox_tag->template to_reflexive<blam::scn::skybox>().data(
                magic)[0];

        skybox_mod.tag = skybox_tag;

        auto model = data.model_cache.predict(skybox_.model.to_plain(), 0);

        if(!model.valid())
        {
            cDebug("Invalid skybox");
            continue;
        }

        ModelItem& model_ = data.model_cache.find(model)->second;

        for(ModelItem::SubModel const& sub : model_.mesh.sub)
        {
            auto      submod  = e.ref(e.create_entity(skybox_model));
            SubModel& submod_ = submod.get<SubModelTag>();

            submod_.initialize(model, sub, {});
            skybox_mod.models.push_back(submod);
            submod_.parent = skybox_ent;
        }

        cDebug("Skybox");
    }

    data.model_buf->unmap();
    data.model_index->unmap();
}

i32 blam_main(i32, cstring_w*)
{
    RuntimeQueue::CreateNewQueue("Blam Graphics!");

    comp_app::app_error app_ec;
    auto&               e = comp_app::createContainer();
    comp_app::configureDefaults(e);

    cDebug(
        "Buffer budget: {0} / {1} MB",
        memory_budget::grand_total,
        memory_budget::grand_total / Unit_MB);

    comp_app::addDefaults(e, *e.service<comp_app::AppLoader>(), app_ec);
    comp_app::AppContainer<BlamData<halo_version>>::addTo(
        e,
        [](EntityContainer&        e,
           BlamData<halo_version>& data,
           time_point const&) {
            ProfContext _(__PRETTY_FUNCTION__);

            e.register_component_inplace<ModelStore>();
            e.register_component_inplace<ModelParentStore>();
            e.register_component_inplace<BspStore>();
            e.register_component_inplace<ObjectSpawnStore>();
            e.register_component_inplace<MultiplayerSpawnStore>();
            e.register_component_inplace<ShaderDataStore>();

            blam::tag_index_view index(data.map_container);

            auto& magic = data.map_container.magic;

            data.scenario =
                data.map_container.tags->scenario(data.map_container.map)
                    .to_reflexive<blam::scn::scenario<blam::hsc::bc::v2>>()
                    .data(magic)
                    .data;

            if(e.service<comp_app::WindowInfo>())
            {
                auto map_name      = data.map_container.map->full_mapname();
                auto window_config = e.service<comp_app::WindowInfo>();
                window_config->setName(
                    window_config->name() + " : " + map_name);
            }

            create_resources(e, data);

            auto& gfx      = e.subsystem_cast<GFX_ALLOC>();
            auto& pipeline = gfx.alloc_standard_pipeline<2>(
                {{"map.vert"_rsc, "map.frag"_rsc}});

            auto& scenery_pipeline = gfx.alloc_standard_pipeline<2>(
                {{"scenery.vert"_rsc, "scenery.frag"_rsc}});

            data.bsp_pipeline        = &pipeline;
            data.model_pipeline      = &scenery_pipeline;
            data.senv_micro_pipeline = &gfx.alloc_standard_pipeline<2>(
                {{"map.vert"_rsc, "map_senv.frag"_rsc}});

            auto& senv_micro = *data.senv_micro_pipeline;

            pipeline.build_state();
            pipeline.get_state();
            data.model_pipeline->build_state();
            data.model_pipeline->get_state();

            load_scenario_bsp(e, data);
            load_scenario_scenery(e, data);

            {
                ProfContext _("Texture allocation");
                data.bitm_cache.allocate_storage();
            }

            GFX::ERROR ec;
            data.model_matrix_store->bindrange(0, 0, 4_MB, ec);
            data.material_store->bindrange(1, 0, 2_MB, ec);
            data.material_store->bindrange(2, 2_MB, 2_MB, ec);

            {
                senv_micro.set_constant(
                    "view", Bytes::From(data.camera_matrix));
                senv_micro.set_sampler(
                    "base",
                    data.bitm_cache
                        .get_bucket(
                            PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT1)))
                        .sampler->handle()
                        .bind(0));
                senv_micro.set_sampler(
                    "micro",
                    data.bitm_cache
                        .get_bucket(
                            PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT1)))
                        .sampler->handle()
                        .bind(1));

                senv_micro.build_state();
                senv_micro.get_state();
            }

            {
                auto pipeline = data.model_pipeline;
                pipeline->set_constant(
                    "camera", Bytes::From(data.camera_matrix));

                pipeline->set_sampler(
                    "bc1_tex",
                    data.bitm_cache
                        .get_bucket(
                            PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT1)))
                        .sampler->handle()
                        .bind(0));
                pipeline->set_sampler(
                    "bc3_tex",
                    data.bitm_cache
                        .get_bucket(
                            PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT3)))
                        .sampler->handle()
                        .bind(1));
                pipeline->set_sampler(
                    "bc5_tex",
                    data.bitm_cache
                        .get_bucket(
                            PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT5)))
                        .sampler->handle()
                        .bind(2));

                pipeline->build_state();
                pipeline->get_state();
            }

            pipeline.set_constant("view", Bytes::From(data.camera_matrix));
            pipeline.set_sampler(
                "bc1_tex",
                data.bitm_cache
                    .get_bucket(PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT1)))
                    .sampler->handle()
                    .bind(0));
            pipeline.set_sampler(
                "bc3_tex",
                data.bitm_cache
                    .get_bucket(PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT3)))
                    .sampler->handle()
                    .bind(1));
            pipeline.set_sampler(
                "bc5_tex",
                data.bitm_cache
                    .get_bucket(PixDesc(CompFmt(PixFmt::DXTn, CompFlags::DXT5)))
                    .sampler->handle()
                    .bind(2));

            pipeline.build_state();
            pipeline.get_state();

            e.register_subsystem_inplace<
                MeshRenderer<halo_version>::tag_type,
                MeshRenderer<halo_version>>(std::ref(data));

            GFX::RASTSTATE cull_disable;
            cull_disable.m_doCull  = true;
            cull_disable.m_culling = (u32)typing::graphics::VertexFace::Front;
            GFX::SetRasterizerState(cull_disable);

            GFX::DEPTSTATE depth_enable;
            depth_enable.m_test = true;
            GFX::SetDepthState(depth_enable);
        },
        [](EntityContainer&        e,
           BlamData<halo_version>& data,
           time_point const&,
           duration const&) {
            GFX::DefaultFramebuffer()->clear(0, {0.5f, 0.5f, 0.9f, 1.f}, 1.0);

            data.std_camera->tick();
            if(e.service<comp_app::ControllerInput>())
                data.controller_camera(
                    e.service<comp_app::ControllerInput>()->state(0));

            using namespace typing::vectors::scene;
            {
                data.camera.aspect =
                    e.service<comp_app::Windowing>()->size().aspect();
                data.camera.zVals = {0.01f, 1000.f};

                data.camera_matrix =
                    GenPerspective(data.camera) * GenTransform(data.camera) *
                    typing::vectors::scale(Matf4(), {10}) *
                    typing::vectors::matrixify(typing::vectors::normalize_quat(
                        Quatf(1, -pi / 4, 0, 0)));
            }
        },
        [](EntityContainer&        e,
           BlamData<halo_version>& data,
           time_point const&) {

        });

    return comp_app::ExecLoop<comp_app::BundleData>::exec(e);
}

COFFEE_APPLICATION_MAIN(blam_main)
