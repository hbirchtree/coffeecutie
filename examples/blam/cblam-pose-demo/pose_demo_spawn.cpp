#include "pose_demo_spawn.h"

#include "components.h"
#include "materials.h"
#include "pose_config.h"

#include <blam/volta/blam_antr.h>
#include <blam/volta/blam_scenario.h>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <magic_enum/magic_enum.hpp>

#include <platforms/sysinfo.h>

generation_idx_t g_pose_demo_biped_model{};
u64              g_pose_demo_biped_entity{};
Matf4            g_pose_demo_biped_base_transform{1.f};

std::vector<SpawnedAttachment> g_pose_demo_attachments;

f32 g_pose_demo_mic_volume{0.f};

static void spawn_attached_weapon(
    compo::EntityContainer&            e,
    blam::map_container<halo_version>& container,
    std::string_view                   tag_name,
    generation_idx_t&                  out_model_id,
    u64&                               out_entity_id)
{
    using namespace compo;

    auto                 magic = container.magic;
    blam::tag_index_view index(container);

    auto it = index.find(tag_name);
    if(it == index.end())
    {
        cWarning("pose_demo: weapon tag '{}' not found", tag_name);
        return;
    }
    blam::tag_t const& weapon_tag = *it;

    auto obj_opt = weapon_tag.data<blam::scn::object>(magic);
    if(!obj_opt.has_value())
    {
        cWarning("pose_demo: failed to read '{}' object data", tag_name);
        return;
    }
    blam::scn::object const* instance_obj = obj_opt.value();

    auto& model_cache  = e.subsystem_cast<ModelCache<halo_version>>();
    auto& shader_cache = e.subsystem_cast<ShaderCache<halo_version>>();

    auto model_it = index.find(instance_obj[0].model);
    if(model_it == index.end())
    {
        cWarning("pose_demo: '{}' model tag not found", tag_name);
        return;
    }

    ModelAssembly mesh_data = model_cache.predict_regions(
        instance_obj[0].model, blam::mod2::lod_high_ext);
    if(mesh_data.models.empty() || !mesh_data.models.at(0).valid())
    {
        cWarning("pose_demo: predict_regions failed for '{}'", tag_name);
        return;
    }
    out_model_id = mesh_data.models.at(0);

    static blam::scn::biped_spawn s_synth_spawn{};
    s_synth_spawn.pos = Vecf3(0.f, 0.f, 0.f);
    s_synth_spawn.rot = Vecf3(0.f, 0.f, 0.f);

    EntityRecipe parent_recipe;
    parent_recipe.components = {
        type_hash_v<Model>(),
        type_hash_v<Visibility>(),
        type_hash_v<NetworkInfo>(),
        type_hash_v<ObjectSpawn>(),
        type_hash_v<DepthInfo>(),
    };
    parent_recipe.tags = ObjectGC;

    EntityRecipe submodel_recipe;
    submodel_recipe.components = {
        type_hash_v<SubModel>(),
        type_hash_v<DrawState>(),
        type_hash_v<ShaderData>(),
        type_hash_v<MeshTrackingData>(),
    };
    submodel_recipe.tags = ObjectMod2 | ObjectGC;

    auto         parent_ = e.create_entity(parent_recipe);
    Model&       model   = parent_.get<Model>();
    ObjectSpawn& spawn   = parent_.get<ObjectSpawn>();
    DepthInfo&   depth   = parent_.get<DepthInfo>();

    spawn.tag           = &weapon_tag;
    spawn.header        = &s_synth_spawn;
    model.tag           = &(*model_it);
    model.model         = mesh_data.models.at(0);
    model.origin_object = &weapon_tag;
    model.initialize(&s_synth_spawn);
    depth.position = model.position;

    NetworkInfo& netinfo = parent_.get<NetworkInfo>();
    netinfo.instance_id  = 2;

    out_entity_id = parent_.id();

    u32 parts_created = 0, parts_skipped = 0;
    for(auto const& model_ : mesh_data.models)
    {
        ModelItem<halo_version>& modelit = model_cache.get(model_);
        for(auto const& sub : modelit.mesh.sub)
        {
            if(!sub.shader.valid())
            {
                ++parts_skipped;
                continue;
            }
            ++parts_created;

            auto submod = e.create_entity(submodel_recipe);
            model.parts.push_back(submod);
            SubModel& submod_ = submod.get<SubModel>();

            submod_.parent = parent_.id();
            DrawState& submod__draw = submod.get<DrawState>();
            submod_.initialize<halo_version>(model_, sub, submod__draw);

            ShaderData&       shader_   = submod.get<ShaderData>();
            ShaderItem const& shader_it = shader_cache.get(sub.shader);
            shader_.initialize(shader_it, submod_);

            submod__draw.current_pass = shader_.get_render_pass(shader_cache);
        }
    }

    cDebug(
        "pose_demo: spawned '{}' ({} parts created, {} skipped)",
        tag_name,
        parts_created,
        parts_skipped);
}

void spawn_static_biped(
    compo::EntityContainer& e, blam::map_container<halo_version>& container)
{
    using namespace compo;

    auto                 magic = container.magic;
    blam::tag_index_view index(container);

    /* All of these came from query params before; load_pose_config() now folds
     * the query params over pose_config.json, so the same URLs still work. */
    auto const& biped      = g_pose_config.biped_tag;
    auto const& start_anim = g_pose_config.start_animation;

    auto it = index.find(biped);
    if(it == index.end())
    {
        cWarning("pose_demo: {} tag not found", biped);
        return;
    }
    blam::tag_t const& biped_tag = *it;

    auto obj_opt = biped_tag.data<blam::scn::object>(magic);
    if(!obj_opt.has_value())
    {
        cWarning("pose_demo: failed to read {} object data", biped);
        return;
    }
    blam::scn::object const* instance_obj = obj_opt.value();

    /* Marker placement per configured attachment, filled by the marker scan
     * below; index matches g_pose_config.attachments. */
    struct MarkerHit
    {
        bool  found{false};
        u16   node_idx{0};
        Matf4 local{1.f};
    };
    std::vector<MarkerHit> marker_hits(g_pose_config.attachments.size());

    /* Diagnostic bone dump — verify real bone names before trusting the
     * retarget table in pose_config.json (main.cpp:80-91 pattern). */
    {
        auto model_hdr_it = index.find(instance_obj[0].model);
        if(model_hdr_it != index.end())
        {
            auto mod2_opt =
                (*model_hdr_it).data<blam::mod2::header<halo_version>>(magic);
            if(mod2_opt.has_value())
            {
                auto const* mod2_hdr = &mod2_opt.value()[0];
                if(auto bones_opt = mod2_hdr->bones.data(magic);
                   bones_opt.has_value())
                {
                    cDebug(
                        "pose_demo: {} bone dump ({} bones)",
                        biped,
                        bones_opt.value().size());
                    u32 idx = 0;
                    for(auto const& bone : bones_opt.value())
                    {
                        cDebug(
                            " - [{}] '{}' parent={} next_child={}",
                            idx++,
                            bone.name.str(),
                            bone.parent == blam::mod2::bone::invalid_bone
                                ? -1
                                : static_cast<i32>(bone.parent),
                            bone.next_child);
                    }

                    for(auto const& bone : bones_opt.value())
                    {
                        Quatf q = glm::conjugate(bone.rotation);
                        cDebug(
                            "pose_demo: bind-local rotation '{}': "
                            "w={:.6f} x={:.6f} y={:.6f} z={:.6f}",
                            bone.name.str(),
                            q.w,
                            q.x,
                            q.y,
                            q.z);
                    }
                }

                if(auto regions_opt = mod2_hdr->regions.data(magic);
                   regions_opt.has_value())
                {
                    u32 r_idx = 0;
                    for(auto const& region : regions_opt.value())
                    {
                        cDebug(
                            "pose_demo: region [{}] '{}'",
                            r_idx,
                            region.name.str());
                        if(auto perms_opt = region.permutations.data(magic);
                           perms_opt.has_value())
                        {
                            u32 p_idx = 0;
                            for(auto const& perm : perms_opt.value())
                                cDebug(
                                    "pose_demo:   permutation [{}] '{}'",
                                    p_idx++,
                                    perm.name.str());
                        }
                        ++r_idx;
                    }
                }

                if(auto markers_opt = mod2_hdr->markers.data(magic);
                   markers_opt.has_value())
                {
                    for(auto const& marker : markers_opt.value())
                    {
                        cDebug("pose_demo: marker '{}'", marker.name.str());
                        if(auto inst_opt = marker.instances.data(magic);
                           inst_opt.has_value())
                        {
                            for(auto const& inst : inst_opt.value())
                            {
                                cDebug(
                                    "pose_demo:   instance region={} "
                                    "permutation={} "
                                    "node={} pos=({:.4f},{:.4f},{:.4f})",
                                    static_cast<i32>(inst.region_idx),
                                    static_cast<i32>(inst.permutation_idx),
                                    static_cast<i32>(inst.node_idx),
                                    inst.position.x,
                                    inst.position.y,
                                    inst.position.z);

                                /* First instance of each configured marker
                                 * wins, matching the previous single-marker
                                 * behaviour. */
                                for(size_t a = 0;
                                    a < g_pose_config.attachments.size();
                                    ++a)
                                {
                                    if(marker_hits[a].found ||
                                       marker.name.str() !=
                                           g_pose_config.attachments[a].marker)
                                        continue;
                                    Quatf marker_rot(
                                        inst.rotation.w,
                                        inst.rotation.x,
                                        inst.rotation.y,
                                        inst.rotation.z);
                                    marker_hits[a].node_idx =
                                        static_cast<u16>(inst.node_idx);
                                    marker_hits[a].local =
                                        glm::translate(
                                            Matf4(1), inst.position) *
                                        glm::mat4_cast(
                                            glm::conjugate(marker_rot));
                                    marker_hits[a].found = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    auto& model_cache  = e.subsystem_cast<ModelCache<halo_version>>();
    auto& shader_cache = e.subsystem_cast<ShaderCache<halo_version>>();

    auto model_it = index.find(instance_obj[0].model);
    if(model_it == index.end())
    {
        cWarning("pose_demo: {} model tag not found", biped);
        return;
    }

    ModelAssembly mesh_data = model_cache.predict_regions(
        instance_obj[0].model, blam::mod2::lod_high_ext);
    if(mesh_data.models.empty() || !mesh_data.models.at(0).valid())
    {
        cWarning("pose_demo: predict_regions failed for cyborg_mp");
        return;
    }
    g_pose_demo_biped_model = mesh_data.models.at(0);

    /* Idle animation frame 0 — mirrors load_objects (loading.h:380-467). Not
     * factored into a shared helper: that template is tied to real
     * scenario palette/instance spans we don't have here; duplicating this
     * self-contained scan is simpler than faking a reflex_group. */
    {
        blam::tagref_t const& anim_graph = instance_obj[0].anim_graph;
        if(anim_graph.valid())
        {
            auto antr_it = index.find(anim_graph);
            if(antr_it != index.end())
            {
                auto antr_data = (*antr_it).data<blam::antr::header>(magic);
                if(antr_data.has_value())
                {
                    auto const*        antr_hdr = &antr_data.value()[0];
                    std::optional<u32> anim_idx;
                    auto all_anims_opt = antr_hdr->animations.data(magic);
                    if(all_anims_opt.has_value())
                    {
                        cDebug("Animations:");
                        u32 idx = 0;
                        for(auto const& animation : all_anims_opt.value())
                        {
                            idx++;
                            cDebug("- {}", animation.name.str());
                            if(animation.name.str() != start_anim)
                                continue;
                            anim_idx = idx - 1;
                        }
                    }
                    u32 anim_frame_count = 0;
                    if(auto ai_opt = antr_hdr->animations.data(magic);
                       ai_opt.has_value() &&
                       anim_idx < static_cast<u32>(ai_opt.value().size()))
                    {
                        anim_frame_count = static_cast<u32>(
                            ai_opt.value()[anim_idx.value_or(0)].frame_count);
                    }
                    for(auto const& mid : mesh_data.models)
                    {
                        model_cache.apply_animation(
                            mid, antr_hdr, anim_idx.value_or(0), 0);
                        auto& mitem            = model_cache.get(mid);
                        mitem.antr_hdr         = antr_hdr;
                        mitem.anim_idx         = anim_idx.value_or(0);
                        mitem.anim_frame_count = anim_frame_count;
                    }
                }
            }
        }
    }

    /* bloodgulch has no pre-placed biped instance to reuse (see header
     * comment), so synthesize one — fixed at the world origin for this
     * isolated (no BSP/scenery) sample. */
    static blam::scn::biped_spawn s_synth_spawn{};
    s_synth_spawn.pos = Vecf3(0.f, 0.f, 0.f);
    s_synth_spawn.rot = Vecf3(0.f, 0.f, 0.f);

    EntityRecipe parent_recipe;
    parent_recipe.components = {
        type_hash_v<Model>(),
        type_hash_v<Visibility>(),
        type_hash_v<NetworkInfo>(),
        type_hash_v<ObjectSpawn>(),
        type_hash_v<DepthInfo>(),
    };
    parent_recipe.tags = ObjectGC;

    EntityRecipe submodel_recipe;
    submodel_recipe.components = {
        type_hash_v<SubModel>(),
        type_hash_v<DrawState>(),
        type_hash_v<ShaderData>(),
        type_hash_v<MeshTrackingData>(),
    };
    submodel_recipe.tags = ObjectMod2 | ObjectGC;

    auto         parent_ = e.create_entity(parent_recipe);
    Model&       model   = parent_.get<Model>();
    ObjectSpawn& spawn   = parent_.get<ObjectSpawn>();
    DepthInfo&   depth   = parent_.get<DepthInfo>();
    Visibility&  vis     = parent_.get<Visibility>();

    spawn.tag           = &biped_tag;
    spawn.header        = &s_synth_spawn;
    vis.interior        = true;
    model.tag           = &(*model_it);
    model.model         = mesh_data.models.at(0);
    model.origin_object = &biped_tag;
    model.initialize(&s_synth_spawn);
    g_pose_demo_biped_base_transform = model.transform;
    depth.position = model.position;

    g_pose_demo_biped_entity = parent_.id();

    NetworkInfo& netinfo = parent_.get<NetworkInfo>();
    netinfo.instance_id  = 1;

    u32 parts_created = 0, parts_skipped = 0;
    for(auto const& model_ : mesh_data.models)
    {
        ModelItem<halo_version>& modelit = model_cache.get(model_);
        cDebug(
            "pose_demo: model region has {} sub-parts",
            modelit.mesh.sub.size());
        for(auto const& sub : modelit.mesh.sub)
        {
            if(!sub.shader.valid())
            {
                ++parts_skipped;
                continue;
            }
            ++parts_created;

            auto submod = e.create_entity(submodel_recipe);
            model.parts.push_back(submod);
            SubModel& submod_ = submod.get<SubModel>();

            submod_.parent = parent_.id();
            DrawState& submod__draw = submod.get<DrawState>();
            submod_.initialize<halo_version>(model_, sub, submod__draw);

            ShaderData&       shader_   = submod.get<ShaderData>();
            ShaderItem const& shader_it = shader_cache.get(sub.shader);
            shader_.initialize(shader_it, submod_);

            submod__draw.current_pass = shader_.get_render_pass(shader_cache);
        }
    }

    cDebug(
        "pose_demo: spawned '{}' biped at {} ({} parts created, {} skipped)",
        biped,
        s_synth_spawn.pos,
        parts_created,
        parts_skipped);

    g_pose_demo_attachments.clear();
    for(size_t a = 0; a < g_pose_config.attachments.size(); ++a)
    {
        auto const& want = g_pose_config.attachments[a];
        if(!marker_hits[a].found)
        {
            cWarning(
                "pose_demo: marker '{}' not found on '{}', skipping '{}' "
                "attach",
                want.marker,
                biped,
                want.tag);
            continue;
        }

        SpawnedAttachment attached;
        attached.node_idx     = marker_hits[a].node_idx;
        attached.marker_local = marker_hits[a].local;
        spawn_attached_weapon(
            e, container, want.tag, attached.model, attached.entity);
        if(!attached.model.valid())
            continue;
        g_pose_demo_attachments.push_back(attached);
    }
}

void setup_fixed_camera(compo::EntityContainer& e)
{
    using namespace compo;

    auto  ref       = e.create_entity(shared_recipes::player_recipe);
    auto& info      = ref.get<PlayerInfo>();
    info.player_idx = 0;
    info.seat_idx   = 0;

    auto const& cam_config = g_pose_config.camera;

    auto& cam              = ref.get<PlayerCamera>();
    cam.keyboard.enabled   = cam_config.keyboard;
    cam.camera.position    = cam_config.position;
    cam.camera.aspect      = cam_config.aspect;
    cam.camera.fieldOfView = cam_config.field_of_view;

    cDebug(
        "pose_demo: fixed camera at {} looking toward origin",
        cam.camera.position);

    auto& gpu = e.subsystem_cast<BlamResources>();
    {
        Span<materials::world_data> world =
            gpu.world_store->map<materials::world_data>(0);
        world[0].lighting[0].light_direction = Vecf4{1, 0, 1, 10.f};
        world[0].lighting[0].light_color     = Vecf4{1, 1, 1, 1};
        world[0].fog.indoor_color            = Vecf4{1, 1, 1, 100};
        world[0].fog.distances               = Vecf4{100, 1000, 100, 1000};
    }
    gpu.world_store->unmap();
}
