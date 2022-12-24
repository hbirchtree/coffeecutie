#include "data.h"
#include "rendering.h"
#include "resource_creation.h"

#if defined(FEATURE_ENABLE_ASIO)
#include <coffee/asio/net_profiling.h>
#endif

#if defined(FEATURE_ENABLE_ImGui)
#include "bsp_component.h"
#include "debug_ui_system.h"
#include "script_component.h"
#include <coffee/imgui/imgui_binding.h>
#endif

using halo_version = blam::pc_version_t;

template<typename Version>
void load_scenario_bsp(EntityContainer& e, BlamData<Version>& data)
{
    ProfContext _(__FUNCTION__);

    using namespace Components;

    auto&                 magic     = data.map_container.magic;
    BSPCache<Version>&    bsp_cache = e.subsystem_cast<BSPCache<Version>>();
    ShaderCache<Version>& shader_cache
        = e.subsystem_cast<ShaderCache<Version>>();

    {
        auto vert                = data.bsp_buf->map(0);
        auto index               = data.bsp_index->map(0);
        auto light               = data.bsp_light_buf->map(0);
        bsp_cache.vert_buffer    = Bytes::ofContainer(vert);
        bsp_cache.element_buffer = Bytes::ofContainer(index);
        bsp_cache.light_buffer   = Bytes::ofContainer(light);
    }

    Vector<generation_idx_t> bsp_meshes;

    /* Start loading up vertex data */
    auto scenario = data.scenario;
    for(auto const& bsp : scenario->bsp_info.data(magic).value())
        bsp_meshes.push_back(bsp_cache.predict(bsp));

    data.bsp_buf->unmap();
    data.bsp_index->unmap();
    data.bsp_light_buf->unmap();

    EntityRecipe bsp;
    bsp.components = {type_hash_v<BspReference>(), type_hash_v<ShaderData>()};
    bsp.tags       = ObjectBsp;

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
    parent.components = {type_hash_v<Model>(), type_hash_v<ObjectSpawn>()};
    parent.tags       = tags;

    EntityRecipe submodel;
    submodel.components = {type_hash_v<SubModel>(), type_hash_v<ShaderData>()};
    submodel.tags       = tags | ObjectMod2;

    auto& model_cache  = e.subsystem_cast<ModelCache<Version>>();
    auto& shader_cache = e.subsystem_cast<ShaderCache<Version>>();

    auto obj_names
        = data.scenario->objects.object_names.data(data.map_container.magic);
    auto magic   = data.map_container.magic;
    auto index   = blam::tag_index_view(data.map_container);
    auto palette = group.palette.data(magic).value();

    for(T const& instance : group.instances.data(magic).value())
    {
        if(instance.ref == -1 || !palette[instance.ref][0].valid())
            continue;

        auto instance_tag = (*index.find(palette[instance.ref][0]));

        if(!instance_tag->valid())
            continue;

        auto instance_obj
            = instance_tag->template data<blam::scn::object>(magic).value();

        ModelAssembly mesh_data
            = model_cache.predict_regions(instance_obj[0].model.to_plain());

        auto         parent_ = e.create_entity(parent);
        Model&       model   = parent_.get<Model>();
        ObjectSpawn& spawn   = parent_.get<ObjectSpawn>();

        spawn.tag    = instance_tag;
        spawn.header = &instance;
        model.tag    = *index.find(instance_obj[0].model.to_plain());
        model.initialize(&instance);

        for(auto const& model_ : mesh_data.models)
        {
            ModelItem<Version> const& modelit
                = model_cache.find(model_.at(blam::mod2::lod_high_ext))->second;

            for(auto const& sub : modelit.mesh.sub)
            {
                if(!sub.shader.valid())
                    continue;

                auto submod = e.create_entity(submodel);
                model.models.push_back(submod);
                SubModel& submod_ = submod.get<SubModel>();

                submod_.parent = parent_;
                submod_.initialize<Version>(
                    model_.at(blam::mod2::lod_high_ext), sub);

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
    equip.components = {type_hash_v<Model>(), type_hash_v<MultiplayerSpawn>()};
    equip.tags       = tags;

    EntityRecipe submodel;
    submodel.components = {type_hash_v<SubModel>(), type_hash_v<ShaderData>()};
    submodel.tags       = tags | ObjectMod2;

    for(blam::scn::multiplayer_equipment const& equipment_ref :
        equipment.value())
    {
        auto item_coll_tag = index.find(equipment_ref.item);

        if(item_coll_tag == index.end())
            continue;

        blam::scn::item_collection const& item_coll
            = (*item_coll_tag)
                  ->template data<blam::scn::item_collection>(magic)
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
                          ->template data<blam::scn::item>(magic)
                          .value()[0];

                auto              set    = e.create_entity(equip);
                Model&            model_ = set.get<Model>();
                MultiplayerSpawn& spawn  = set.get<MultiplayerSpawn>();

                spawn.item       = &item;
                spawn.spawn      = &equipment_ref;
                spawn.collection = &item_coll;
                model_.initialize(&equipment_ref);
                model_.tag = *index.find(item.model);

                ModelAssembly models
                    = model_cache.predict_regions(item.model.to_plain());

                for(auto const& model : models.models)
                {
                    ModelItem<Version> const& modelit
                        = model_cache.find(model.at(blam::mod2::lod_high_ext))
                              ->second;

                    for(auto const& sub : modelit.mesh.sub)
                    {
                        auto submod = e.create_entity(submodel);
                        model_.models.push_back(submod);
                        SubModel& submod_ = submod.get<SubModel>();
                        submod_.parent    = set;
                        submod_.initialize<Version>(
                            model.at(blam::mod2::lod_high_ext), sub);

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

    auto& model_cache  = e.subsystem_cast<ModelCache<Version>>();
    auto& shader_cache = e.subsystem_cast<ShaderCache<Version>>();

    {
        ProfContext _("Buffer mapping");
        auto        vert           = data.model_buf->map(0);
        auto        index          = data.model_index->map(0);
        model_cache.vert_buffer    = Bytes::ofContainer(vert);
        model_cache.element_buffer = Bytes::ofContainer(index);
    }

    auto scenario = data.scenario;
    auto magic    = data.map_container.magic;

    auto pipeline = data.model_pipeline;

    load_objects(scenario->objects.scenery, data, e, ObjectScenery);
    load_objects(scenario->objects.vehicles, data, e, ObjectVehicle);
    load_objects(scenario->objects.bipeds, data, e, ObjectBiped);
    load_objects(scenario->objects.equips, data, e, ObjectEquipment);
    load_objects(scenario->objects.weapon_spawns, data, e, ObjectEquipment);
    load_objects(scenario->objects.machines, data, e, ObjectDevice);
    load_objects(scenario->objects.light_fixtures, data, e, ObjectLightFixture);

    load_multiplayer_equipment(data, e, ObjectEquipment);

    blam::tag_index_view index(data.map_container);

    using namespace Components;

    EntityRecipe skybox_base;
    skybox_base.tags       = ObjectSkybox;
    skybox_base.components = {type_hash_v<Model>()};
    EntityRecipe skybox_model;
    skybox_model.tags = ObjectSkybox | ObjectMod2;
    skybox_model.components
        = {type_hash_v<SubModel>(), type_hash_v<ShaderData>()};

    auto   skybox_ent = e.create_entity(skybox_base);
    Model& skybox_mod = skybox_ent.get<Model>();

    for(auto const& skybox : data.scenario->info.skyboxes.data(magic).value())
    {
        auto        skybox_tag = *index.find(skybox);
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
            submod_.parent = skybox_ent;

            ShaderData& shader  = submod.get<ShaderData>();
            ShaderItem& shader_ = shader_cache.find(sub.shader)->second;

            shader.initialize(shader_, submod_);
        }

        cDebug("Skybox");
    }

    data.model_buf->unmap();
    data.model_index->unmap();
}

i32 blam_main(i32, cstring_w*)
{
    auto q = rq::runtime_queue::CreateNewQueue("Blam Graphics!");
#if defined(FEATURE_ENABLE_ASIO)
    C_UNUSED(auto _ = Net::RegisterProfiling());
#endif

    comp_app::app_error app_ec;
    auto&               e = comp_app::createContainer();
    comp_app::configureDefaults(e);

#if defined(FEATURE_ENABLE_GLeamRHI)
    auto& glConfig
        = e.service<comp_app::AppLoader>()->config<comp_app::GLConfig>();
    glConfig.swapInterval = 1;
    if constexpr(compile_info::debug_mode)
        glConfig.profile |= comp_app::GLConfig::Debug;
#endif

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
            ProfContext _(__FUNCTION__);

            auto& gfx        = e.register_subsystem_inplace<gfx::system>();
            auto  load_error = gfx.load(gfx::api::load_options_t{
                 .api_version = 0x430,
                 .api_type    = gfx::api_type_t::core,
            });

            if(load_error)
            {
                cWarning(
                    "Failed to initialize gfx::api: {0}",
                    magic_enum::enum_name(load_error.value()));
            }

            gfx.debug().enable();
            gfx.debug().add_callback([](gfx::group::debug_severity sev,
                                        std::string_view const&    msg) {
                if(sev == gfx::group::debug_severity::notification)
                    return;
                cDebug("GL: {0}", msg);
            });

            e.register_component_inplace<Model>();
            e.register_component_inplace<SubModel>();
            e.register_component_inplace<BspReference>();
            e.register_component_inplace<ObjectSpawn>();
            e.register_component_inplace<MultiplayerSpawn>();
            e.register_component_inplace<ShaderData>();

            auto& imgui = e.register_subsystem_inplace<imgui::ImGuiSystem>(
                std::ref(gfx));
            e.register_subsystem_inplace<BlamDebugUi>();
            e.register_subsystem_inplace<BlamBspWidget<halo_version>>(&data);

            {
                comp_app::app_error ec;
                imgui.load(e, ec);
                auto frame_ui = e.create_entity({
                    .components = { typeid(imgui::ImGuiWidget).hash_code(), },
                });
                frame_ui.get<imgui::ImGuiWidget>()
                    = imgui::widgets::StatsMenu();
            }

            {
                auto& bitm_cache
                    = e.register_subsystem_inplace<BitmapCache<halo_version>>(
                        std::ref(data.map_container),
                        blam::magic_data_t(
                            C_OCAST<Bytes>(data.bitmap_file).view),
                        &gfx);
                auto& shader_cache
                    = e.register_subsystem_inplace<ShaderCache<halo_version>>(
                        std::ref(data.map_container), std::ref(bitm_cache));
                auto& bsp_cache
                    = e.register_subsystem_inplace<BSPCache<halo_version>>(
                        std::ref(data.map_container),
                        std::ref(bitm_cache),
                        std::ref(shader_cache));
                auto& model_cache
                    = e.register_subsystem_inplace<ModelCache<halo_version>>(
                        std::ref(data.map_container),
                        std::ref(bitm_cache),
                        std::ref(shader_cache),
                        &gfx);
            }

            blam::tag_index_view index(data.map_container);

            auto& magic = data.map_container.magic;

            data.scenario
                = data.map_container.tags->scenario(data.map_container.map)
                      .value()
                      ->data<blam::scn::scenario<halo_version>>(magic)
                      .value();

            e.register_subsystem_inplace<BlamScript<halo_version>>(
                std::ref(data.map_container), data.scenario, magic);

            if(e.service<comp_app::WindowInfo>())
            {
                auto map_name      = data.map_container.map->full_mapname();
                auto window_config = e.service<comp_app::WindowInfo>();
                window_config->setName(
                    Strings::fmt("{0} : {1}", window_config->name(), map_name));
            }

            create_resources(e, data);

            {
                auto pipeline = gfx.alloc_program();
                pipeline->add(
                    gfx::program_t::stage_t::Vertex,
                    gfx.alloc_shader("map.vert"_rsc.data()));
                pipeline->add(
                    gfx::program_t::stage_t::Fragment,
                    gfx.alloc_shader("map.frag"_rsc.data()));
                if(auto res = pipeline->compile(); res.has_error())
                {
                    auto [msg] = res.error();
                    cFatal("Failed to compile bsp shader: {0}", msg);
                }
                data.bsp_pipeline = pipeline;
            }
            {
                auto pipeline = gfx.alloc_program();
                pipeline->add(
                    gfx::program_t::stage_t::Vertex,
                    gfx.alloc_shader("scenery.vert"_rsc.data()));
                pipeline->add(
                    gfx::program_t::stage_t::Fragment,
                    gfx.alloc_shader("scenery.frag"_rsc.data()));
                if(auto res = pipeline->compile(); res.has_error())
                {
                    auto [msg] = res.error();
                    cFatal("Failed to compile scenery shader: {0}", msg);
                }
                data.model_pipeline = pipeline;
            }
            {
                auto pipeline = gfx.alloc_program();
                pipeline->add(
                    gfx::program_t::stage_t::Vertex,
                    gfx.alloc_shader("map.vert"_rsc.data()));
                pipeline->add(
                    gfx::program_t::stage_t::Fragment,
                    gfx.alloc_shader(
                        "map_senv.frag"_rsc.data(),
                        {{"MICRO_BLEND", "1"}, {"PRIMARY_BLEND", "1"}}));
                if(auto res = pipeline->compile(); res.has_error())
                {
                    auto [msg] = res.error();
                    cFatal("Failed to compile senv shader: {0}", msg);
                }
                data.senv_micro_pipeline = pipeline;
            }
            {
                auto pipeline = gfx.alloc_program();
                pipeline->add(
                    gfx::program_t::stage_t::Vertex,
                    gfx.alloc_shader("map.vert"_rsc.data()));
                pipeline->add(
                    gfx::program_t::stage_t::Fragment,
//                    gfx.alloc_shader("white.frag"_rsc.data()));
                    gfx.alloc_shader("wireframe.frag"_rsc.data()));
                if(auto res = pipeline->compile(); res.has_error())
                {
                    auto [msg] = res.error();
                    cFatal("Failed to compile wireframe shader: {0}", msg);
                }
                data.wireframe_pipeline = pipeline;
            }

            //            data.model_pipeline      = scenery_pipeline;
            //            data.senv_micro_pipeline =
            //            &gfx.alloc_standard_pipeline<2>(
            //                {{"map.vert"_rsc, "map_senv.frag"_rsc}},
            //                GFX::SHD::Constants{{"MICRO_BLEND", 1},
            //                {"PRIMARY_BLEND", 1}});
            //            data.wireframe_pipeline =
            //            &gfx.alloc_standard_pipeline<2>(
            //                {{"map.vert"_rsc, "wireframe.frag"_rsc}});

            //            pipeline.build_state();
            //            pipeline.get_state();
            //            data.model_pipeline->build_state();
            //            data.model_pipeline->get_state();

            load_scenario_bsp(e, data);
            load_scenario_scenery(e, data);

            /* Move the camera to a player spawn location */
            {
                auto spawn_locations = data.scenario->mp.player_spawns
                                           .data(data.map_container.magic)
                                           .value();

                auto transform =
                    //                    GenTransform(data.camera) *

                    //                    typing::vectors::matrixify(typing::vectors::normalize_quat(
                    //                        Quatf(1, -math::pi_f / 4, 0, 0)))
                    //                        *
                    typing::vectors::scale(Matf4(), {10});

                if(!spawn_locations.empty())
                {
                    data.camera.position
                        = (Vecf4(spawn_locations[0].pos + Vecf3(0, 0, 1), 1)
                           * -1.f)
                          * transform;
                    // TODO: Fix facing of camera here
                    cDebug("Facing of player: {0}", spawn_locations[0].rot);
                    data.camera.rotation = typing::vectors::euler(
                        Vecf3{0, spawn_locations[0].rot, -90.f});
                }
            }

            data.std_camera->up_direction = {0, 0, 1};

            {
                ProfContext _("Texture allocation");
                e.subsystem_cast<BitmapCache<halo_version>>()
                    .allocate_storage();
            }

            e.register_subsystem_inplace<MeshRenderer<halo_version>>(
                &gfx, std::ref(data));

            //            data.model_matrix_store->bindrange(0, 0, 4_MB, ec);

            //            data.wireframe_pipeline->set_constant(
            //                "camera", Bytes::From(data.camera_matrix));
            //            data.wireframe_pipeline->set_constant(
            //                "render_distance",
            //                Bytes::From(data.wireframe_distance));
            //            data.wireframe_pipeline->build_state();

            //            {
            //                auto pipeline = data.model_pipeline;
            //                pipeline->set_constant(
            //                    "camera", Bytes::From(data.camera_matrix));

            //                pipeline->set_sampler(
            //                    "bc1_tex",
            //                    data.bitm_cache
            //                        .get_bucket(
            //                            PixDesc(CompFmt(PixFmt::DXTn,
            //                            CompFlags::DXT1)))
            //                        .sampler->handle()
            //                        .bind(0));
            //                pipeline->set_sampler(
            //                    "bc3_tex",
            //                    data.bitm_cache
            //                        .get_bucket(
            //                            PixDesc(CompFmt(PixFmt::DXTn,
            //                            CompFlags::DXT3)))
            //                        .sampler->handle()
            //                        .bind(1));
            //                pipeline->set_sampler(
            //                    "bc5_tex",
            //                    data.bitm_cache
            //                        .get_bucket(
            //                            PixDesc(CompFmt(PixFmt::DXTn,
            //                            CompFlags::DXT5)))
            //                        .sampler->handle()
            //                        .bind(2));

            //                pipeline->build_state();
            //                pipeline->get_state();
            //            }

            //            e.register_subsystem_inplace<MeshRenderer<halo_version>::tag_type>(
            //                std::ref(data));

            //            GFX::RASTSTATE cull_disable;
            //            cull_disable.m_doCull  = true;
            //            cull_disable.m_culling =
            //            (u32)typing::graphics::VertexFace::Front;
            //            GFX::SetRasterizerState(cull_disable);

            //            GFX::DEPTSTATE depth_enable;
            //            depth_enable.m_test = true;
            //            GFX::SetDepthState(depth_enable);

            //            runtime_queue_error rqec;
            //            RuntimeQueue::QueuePeriodic(
            //                RuntimeQueue::GetCurrentQueue(rqec),
            //                Chrono::milliseconds(10),
            //                [&data]() {
            //                    data.wireframe_distance += 0.05f;
            //                    data.wireframe_distance
            //                        = CMath::fmod(data.wireframe_distance,
            //                        400.f);
            //                },
            //                rqec);
        },
        [](EntityContainer&        e,
           BlamData<halo_version>& data,
           time_point const&,
           duration const&) {
            e.subsystem_cast<gfx::system>().default_rendertarget()->clear(
                Vecf4{Vecf3{0.1f}, 1.f}, 1.0, 0);

            data.std_camera->tick();
            if(e.service<comp_app::ControllerInput>())
                data.controller_camera(
                    e.service<comp_app::ControllerInput>()->state(0));

            using namespace typing::vectors::scene;
            {
                data.camera.aspect
                    = e.service<comp_app::Windowing>()->size().aspect();
                data.camera.zVals = {0.01f, 20000.f};

                data.camera_matrix
                    = GenPerspective(data.camera) * GenTransform(data.camera)
                      * typing::vectors::scale(Matf4(), Vecf3(10));
            }
        },
        [](EntityContainer&, BlamData<halo_version>&, time_point const&) {

        });

    return comp_app::ExecLoop<comp_app::BundleData>::exec(e);
}

COFFEE_APPLICATION_MAIN(blam_main)
