#include "resource_creation.h"

#include "components.h"
#include "data.h"
#include "journal.h"
#include "map_marker.h"
#include "peripherals/constants.h"
#include "physics.h"
#include "shader_compiler.h"

#include <coffee/comp_app/services.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/core/input/standard_input_handlers.h>
#include <coffee/core/types/input/keymap_latin1.h>
#include <coffee/core/files/cfiles.h>
#include <coffee/core/input/eventhandlers.h>
#include <coffee/core/types/input/event_types.h>
#include <coffee/graphics/apis/gleam/rhi.h>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/geometric.hpp>
#include <magic_enum/magic_enum.hpp>
#include <stdexcept>
#include <url/url.h>

#if defined(FEATURE_ENABLE_ComponentBundleSetup_DummyPlug)
#include <coffee/comp_app/dummy_plug.h>
#endif

using Coffee::ProfContext;
using Coffee::Resource;
using semantic::RSCA;

using namespace Coffee::resource_literals;

void update_camera_aspect(compo::EntityContainer& e)
{
    u32 count{0};
    for(auto player : e.select<PlayerCamera, PlayerInfo>())
    {
        auto [cam, info] = player.components();
        if(!info.is_remote() && cam.is_active())
            ++count;
    }
    auto* window = e.service<comp_app::Windowing>();
    for(const auto& player : e.select<PlayerCamera>())
    {
        auto& cam = player.get<PlayerCamera>();
        if(count != 2)
            cam.camera.aspect = window->size().aspect();
        else
        {
            auto size           = window->size();
            cam.camera.aspect = static_cast<f32>(size.w) / (size.h / 2.f);
            cDebug("Assigning aspect ratio {}", cam.camera.aspect);
        }
    }
}

void create_resources(compo::EntityContainer& e)
{
    ProfContext _(__FUNCTION__);

    {
        using namespace Coffee::Display;
        using namespace Coffee::Display::EventHandlers;
        using namespace Coffee;

        auto eventhandler =
            e.service<comp_app::BasicEventBus<Input::CIEvent>>();

        eventhandler->addEventHandler(
            1024,
            ExitOn<OnKey<Input::CK_Escape>>(
                [window = e.service_ref<comp_app::Windowing>()]() mutable {
                    window.lock()->close();
                }));

        eventhandler->addEventHandler(
            1024, StandardCamera::KeyboardInput([&e] -> StandardCamera::Reg* {
                for(auto entity : e.select<
                        PlayerCamera,
                        PlayerInput,
                        PlayerInfo,
                        NetworkInfo>())
                {
                    auto [cam, input, info, net] = entity.components();
                    if(cam.keyboard.enabled && info.permissions.camera)
                    {
                        net.changes.viewport = net.changes.transform = true;
                        return &input.keys;
                    }
                }
                cWarning("No camera selected");
                return nullptr;
            }));
        eventhandler->addEventHandler(
            1024, StandardCamera::MouseInput([&e] -> Vecf2* {
                for(auto entity : e.select<
                        PlayerCamera,
                        PlayerInput,
                        PlayerInfo,
                        NetworkInfo>())
                {
                    auto [cam, input, info, net] = entity.components();
                    if(cam.keyboard.enabled && info.permissions.camera)
                    {
                        net.changes.viewport = true;
                        return &input.look_delta;
                    }
                }
                cWarning("No camera selected");
                return nullptr;
            }));
        /* Physics-mode (biped) movement is not event-driven: the camera
         * loop in graphics.cpp reads held keys every frame and sends a
         * Physics::Velocity event, so movement doesn't stutter at the OS
         * key-repeat rate. */
        auto& gbus = e.subsystem_cast<GameEventBus>();
        auto& pbus = e.subsystem_cast<PhysicsBus>();
        gbus.addEventFunction<PlayerTeleportEvent>(
            1024, [&e, &pbus](GameEvent&, PlayerTeleportEvent* teleport) {
                cDebug(
                    "Teleport event: seat={} entity={} position={} rotation={}",
                    teleport->seat_idx,
                    teleport->entity_id,
                    teleport->position,
                    teleport->rotation);
                auto player = [&e, teleport] {
                    if(teleport->entity_id != 0)
                        return e.ref(teleport->entity_id);
                    for(auto const& player : e.select<PlayerInfo>())
                    {
                        if(player.get<PlayerInfo>().seat_idx != teleport->seat_idx)
                            continue;
                        return e.ref(player.id());
                    }
                    Throw(
                        std::out_of_range(
                            "tried to teleport player, but no target"));
                }();
                auto& cam = player.get<PlayerCamera>();
                auto& net = player.get<NetworkInfo>();
                if(cam.mode.physics)
                {
                    Physics::Event     ev{Physics::Event::Translate};
                    Physics::Translate translate{
                        .entity_id = player.id(),
                        .position  = teleport->position,
                    };
                    pbus.process(ev, &translate);
                } else
                {
                    auto& input = player.get<PlayerInput>();
                    input.position = teleport->position;
                    input.rotation = teleport->rotation;
                    net.changes.viewport = net.changes.transform = true;
                }
            });
        eventhandler->addEventFunction<CIControllerConnectEvent>(
            1024, [&e](CIEvent& ev, CIControllerConnectEvent* connect) {
                auto* controllers = e.service<comp_app::ControllerInput>();
                auto* window      = e.service<comp_app::Windowing>();
                auto  name        = controllers->name(connect->player_index);
                cDebug(
                    "Controller {}connected: {} (idx={})",
                    connect->connected ? "" : "dis",
                    name,
                    connect->player_index);
                for(auto player :
                    e.select<PlayerCamera, PlayerInfo, NetworkInfo>())
                {
                    auto [cam, info, net] = player.components();
                    // Don't assign it to remote seat
                    if(info.is_remote())
                        continue;
                    if(connect->connected)
                    {
                        /* Assign controller to the first seat without one; a
                         * keyboard-only seat can take a controller too (usual
                         * local setup, and the dummy plug's synthetic
                         * controller must be able to join seat 0, which
                         * always has the keyboard) */
                        if(cam.controller.index.has_value())
                            continue;
                        cDebug(
                            "Assigning controller {} to player {} (seat {})",
                            connect->player_index,
                            info.player_idx,
                            info.seat_idx);
                        cam.controller.index = connect->player_index;
                        break;
                    } else if(
                        cam.controller.index.value_or(0xFF) ==
                        connect->player_index)
                    {
                        cDebug(
                            "Unassigning controller {} from player {} (seat "
                            "{})",
                            connect->player_index,
                            info.player_idx,
                            info.seat_idx);
                        cam.controller.index = std::nullopt;
                        break;
                    }
                }
                update_camera_aspect(e);
            });
#if defined(FEATURE_ENABLE_ComponentBundleSetup_DummyPlug)
        eventhandler->addEventFunction<
            CIKeyEvent>(1024, [&e](CIEvent&, CIKeyEvent* key) {
            if(key->key != Input::CK_F9)
                return;
            if(!key->pressed())
                return;
            for(auto const cam_ : e.select<PlayerCamera, PlayerInfo>())
            {
                auto const [cam, info] = cam_.components();
                if(info.seat_idx != 0)
                    continue;
                cDebug(
                    R"({{"time": 0, "type": "camera", "position" :[{}, {}, {}], "rotation":[{}, {}, {}, {}]}})",
                    cam.camera.position.x,
                    cam.camera.position.y,
                    cam.camera.position.z,
                    cam.camera.rotation.w,
                    cam.camera.rotation.x,
                    cam.camera.rotation.y,
                    cam.camera.rotation.z);
            }
        });
#endif

        auto eventhandler_w = e.service<comp_app::BasicEventBus<Event>>();

        eventhandler_w->addEventFunction<ResizeEvent>(
            0, [&e](Event&, ResizeEvent* resize) {
                update_camera_aspect(e);
                cDebug("Window resize: {}x{}", resize->w, resize->h);
            });

        eventhandler->addEventFunction<CIDropEvent>(
            0, [&e](CIEvent&, CIDropEvent* drop) {
                if(!drop->file)
                    return;
                GameEvent     ev{.type = GameEvent::MapLoadStart};
                MapLoadEvent  load{.file = *drop->file};
                GameEventBus& gbus = e.subsystem_cast<GameEventBus>();
                gbus.process(ev, &load);
            });
    }

#if defined(FEATURE_ENABLE_ComponentBundleSetup_DummyPlug)
    auto& dummyConfig = e.subsystem_cast<comp_app::AppLoader>()
                            .config<comp_app::dummy_plug::Config>();
    if(dummyConfig.enabled)
    {
        auto& dummy = e.subsystem_cast<comp_app::dummy_plug::DummyEventBus>();
        dummy.addEventData(
            {.prio    = 0,
             .handler = [&e](
                            comp_app::dummy_plug::DummyEvent& ev, const void*) {
                 e.subsystem_cast<Journal>().record(
                     "dummy_event", {{"event", ev.event}, {"data", ev.data}});
                 if(ev.event == "render_param")
                 {
                     /* Generic toggle of the data.h debug/render knobs from a
                      * dummy plug custom event. On desktop these are driven by
                      * the ImGui widgets; headless/web builds have no ImGui
                      * interaction, so the dummy plug drives them. Any key
                      * present in the event overrides the matching field;
                      * absent keys keep their current value. */
                     auto const& d = ev.data;
                     auto        set_val =
                         [&d]<typename T>(std::string_view key, T& value) {
                             value = d.value(key, value);
                         };
                     auto& rp = e.subsystem_cast<RenderingParameters>();
                     set_val("mipmap_bias", rp.mipmap_bias);
                     set_val("color_changing", rp.color_changing);
                     set_val("render_fog", rp.render_fog);
                     set_val("render_lightmaps", rp.render_lightmaps);
                     set_val("render_model_bones", rp.render_model_bones);
                     set_val("render_reflection", rp.render_reflection);
                     set_val("render_scenery", rp.render_scenery);
                     set_val("only_normals", rp.only_normals);
                     set_val("only_normalmaps", rp.only_normalmaps);
                     set_val("only_lightmaps", rp.only_lightmaps);
                     set_val("only_reflections", rp.only_reflections);
                     set_val("only_multipurpose", rp.only_multipurpose);
                     set_val("only_multipurpose2", rp.only_multipurpose2);
                     set_val("only_diffuse", rp.only_diffuse);
                     set_val("render_ui", rp.render_ui);
                     set_val("debug_clear", rp.debug_clear);
                     set_val("occluder_update", rp.occluder_update);
                     set_val("debug_markers", rp.debug_markers);
                     set_val("debug_portals", rp.debug_portals);
                     set_val("debug_clusters", rp.debug_clusters);
                     set_val("debug_triggers", rp.debug_triggers);
                     set_val("tex_res", rp.tex_res);
                     set_val("draw_distance", rp.draw_distance);
                     set_val("current_bsp_cluster", rp.current_bsp_cluster);

                     if(auto it = d.find("clear_color");
                        it != d.end() && it->is_array() && it->size() >= 3)
                     {
                         rp.clear_color = Vecf3(
                             (*it)[0].get<f32>(),
                             (*it)[1].get<f32>(),
                             (*it)[2].get<f32>());
                     }

                     auto& pp = e.subsystem_cast<PostProcessParameters>();
                     set_val("exposure", pp.exposure);
                     set_val("gamma", pp.gamma);
                     set_val("scale", pp.scale);
                     set_val("auto_expose", pp.auto_expose);
                     set_val("doom_mode", pp.doom_mode);
                 }
                 if(ev.event == "camera")
                 {
                     PlayerCamera* target{};
                     for(auto const& en : e.select<PlayerCamera, PlayerInfo>())
                     {
                        auto [cam, info] = en.components();
                         if(info.seat_idx != 0)
                             continue;
                         target = &cam;
                     }
                     if(!target)
                     {
                         cWarning("Could not assign dummy event to camera");
                         return;
                     }
                     if(ev.data.contains("position"))
                     {
                         auto pos                 = ev.data["position"];
                         target->camera.position = Vecf3{
                             pos[0].get<float>(),
                             pos[1].get<float>(),
                             pos[2].get<float>()};
                     }
                     if(ev.data.contains("rotation"))
                     {
                         auto rot        = ev.data["rotation"];
                         f32  deg_to_rad = glm::pi<f32>() / 180.f;
                         if(rot.size() == 2)
                             target->camera.rotation = glm::normalize(
                                 glm::angleAxis(
                                     rot[0].get<float>() * deg_to_rad,
                                     Vecf3{-1.f, 0.f, 0.f}) *
                                 glm::angleAxis(
                                     rot[1].get<float>() * deg_to_rad,
                                     Vecf3{0.f, -1.f, 0.f}));
                         else
                             target->camera.rotation = glm::normalize(Quatf(
                                 rot[0].get<float>(),
                                 rot[1].get<float>(),
                                 rot[2].get<float>(),
                                 rot[3].get<float>()));
                     }
                     /* Headless equivalent of the ImGui "Physics" checkbox */
                     target->mode.physics =
                         ev.data.value("physics", target->mode.physics);
                 }
                 if(ev.event == "dump_state")
                 {
                     /* Generic per-process state dump for integration testing
                      * (currently just the player roster; add more top-level
                      * keys here as other subsystems need the same treatment —
                      * that's why this writes one shared state.json rather
                      * than a roster-only file). One file per process, in its
                      * own TMPDIR (the client/server harness —
                      * .github/tests/net/ — points TMPDIR at a distinct dir
                      * per side precisely so this doesn't collide).
                      * Overwritten every time this event fires, so the final
                      * write holds end-of-test state; the harness diffs the
                      * two files directly once both processes have exited. */
                     nlohmann::json state;

                     nlohmann::json& players = state["players"];
                     players                 = nlohmann::json::array();
                     for(auto const& entity :
                         e.select<PlayerInfo, NetworkInfo, PlayerCamera>())
                     {
                         auto [info, net, cam] = entity.components();
                         players.push_back({
                             {"player_idx", info.player_idx},
                             {"seat_idx", info.seat_idx},
                             {"name", info.name},
                             {"remote", info.is_remote()},
                             {"loading_progress", info.loading_progress},
                             {"connected", net.connected},
                             {"position",
                              nlohmann::json{
                                  cam.camera.position.x,
                                  cam.camera.position.y,
                                  cam.camera.position.z,
                              }},
                         });
                     }

                     /* state_json is a named local, not a temporary passed
                      * straight into ofString(): BytesConst::ofString's
                      * by-value std::string overload copies its argument into
                      * a function-local parameter and returns a span pointing
                      * into it, which is already destroyed by the time the
                      * caller gets the span back. Resource's BytesConst
                      * assignment borrows rather than copies (see cfiles.h),
                      * so the buffer must genuinely outlive it — hence a named
                      * string_view over a local that's still alive at
                      * FileCommit(). */
                     std::string state_json = state.dump(2);
                     auto        file       = Resource("state.json"_tmpfile);
                     file                   = semantic::BytesConst::ofString(
                         std::string_view(state_json));
                     Coffee::FileCommit(
                         file, RSCA::NewFile | RSCA::Discard | RSCA::WriteOnly);
                     cDebug(
                         "State dumped to state.json ({} player(s))",
                         players.size());
                     e.subsystem_cast<Journal>().record("state_dump", state);
                 }
                 if(ev.event == "switch_map")
                 {
                     using namespace ::platform::url::constructors;
                     using platform::url::Url;
                     std::string path = ev.data.value("map", std::string{});
                     if(path.empty())
                     {
                         cWarning("switch_map event missing \"map\" path");
                         return;
                     }
                     GameEventBus& gbus = e.subsystem_cast<GameEventBus>();
                     if(path.find('/') == std::string::npos)
                     {
                         auto name = path;
                         if(auto dot = name.rfind(".map");
                            dot != std::string::npos)
                             name.resize(dot);
                         GameEvent          event{GameEvent::MapLoadByName};
                         MapLoadByNameEvent load{
                             .origin   = MapLoadEvent::Local,
                             .map_name = *blam::bl_string::from(name),
                         };
                         gbus.process(event, &load);
                         return;
                     }
                     Url map_filename = MkUrl(path, RSCA::SystemFile);
                     Url map_dir =
                         map_filename.path().dirname().url(map_filename.flags);

                     GameEvent    event{GameEvent::MapLoadStart};
                     MapLoadEvent load{
                         .directory = map_dir,
                         .file      = map_filename,
                     };
                     gbus.process(event, &load);
                 }
                 if(ev.event == "net_listen" || ev.event == "net_connect")
                 {
                     std::string address = ev.data.value(
                         "address", std::string{"127.0.0.1:27015"});
                     GameEventBus&      gbus = e.subsystem_cast<GameEventBus>();
                     GameEvent          event{GameEvent::ServerConnect};
                     ServerConnectEvent connect{
                         .type   = ev.event == "net_listen"
                                       ? ServerConnectEvent::Listen
                                       : ServerConnectEvent::Server,
                         .remote = address,
                     };
                     gbus.process(event, &connect);
                     if(ev.event == "net_connect")
                     {
                         GameEvent listing{GameEvent::MapRequestListing};
                         MapRequestListingEvent request{};
                         gbus.process(listing, &request);
                     }
                 }
             }});
    }
#endif

    gfx::api&      api       = e.subsystem_cast<gfx::system>();
    BlamResources& resources = e.register_subsystem_inplace<BlamResources>();

    resources.background_worker =
        rq::runtime_queue::CreateNewThreadQueue("Background Worker").value();

    e.register_subsystem_inplace<PostProcessParameters>();

    auto access = RSCA::WriteOnly | RSCA::Persistent | RSCA::Immutable;

    if constexpr(compile_info::platform::is_emscripten)
        access = RSCA::WriteOnly | RSCA::Immutable | RSCA::Discard;

    resources.bsp_buf = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.bsp_buf->alloc();
    resources.bsp_buf->commit(memory_budget::bsp_buffer);
    resources.bsp_index = api.alloc_buffer(gfx::buffers::element, access);
    resources.bsp_index->alloc();
    resources.bsp_index->commit(memory_budget::bsp_elements);
    resources.bsp_light_buf = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.bsp_light_buf->alloc();
    resources.bsp_light_buf->commit(memory_budget::bsp_buffer / 2);

    resources.model_buf = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.model_buf->alloc();
    resources.model_buf->commit(memory_budget::mesh_buffer);
    resources.model_index = api.alloc_buffer(gfx::buffers::element, access);
    resources.model_index->alloc();
    resources.model_index->commit(memory_budget::mesh_elements);

    const u32 per_frame_bufs = compile_info::platform::is_emscripten ? 1 : 3;

    // For access in the vertex shader, UBOs are better
    resources.model_matrix_store = api.alloc_revolving_buffer(
        gfx::buffers::constants, per_frame_bufs, access);
    resources.bone_matrix_buf = api.alloc_revolving_buffer(
        gfx::buffers::constants, per_frame_bufs, access);
    if(api.feature_info().buffer.ssbo && false)
    {
        // From testing, using SSBOs was slower?
        // Batching into UBOs was consistently faster,
        // likely since UBOs are backed by faster memory
        resources.material_store = api.alloc_revolving_buffer(
            gfx::buffers::shader_writable, per_frame_bufs, access);
        resources.transparent_store = api.alloc_revolving_buffer(
            gfx::buffers::shader_writable, per_frame_bufs, access);
    } else if(api.feature_info().buffer.ubo)
    {
        resources.material_store = api.alloc_revolving_buffer(
            gfx::buffers::constants, per_frame_bufs, access);
        resources.transparent_store = api.alloc_revolving_buffer(
            gfx::buffers::constants, per_frame_bufs, access);
    } else
    {
        resources.model_matrix_store = api.alloc_revolving_buffer(
            gfx::buffers::vertex, per_frame_bufs, access);
        resources.material_store = api.alloc_revolving_buffer(
            gfx::buffers::vertex, per_frame_bufs, access);
        resources.transparent_store = api.alloc_revolving_buffer(
            gfx::buffers::vertex, per_frame_bufs, access);
        resources.bone_matrix_buf = api.alloc_revolving_buffer(
            gfx::buffers::vertex, per_frame_bufs, access);
    }
    resources.model_matrix_store->alloc();
    resources.model_matrix_store->commit(memory_budget::matrix_buffer);
    resources.material_store->alloc();
    resources.material_store->commit(memory_budget::material_buffer);
    resources.transparent_store->alloc();
    resources.transparent_store->commit(memory_budget::transparent_buffer);
    resources.bone_matrix_buf->alloc();
    resources.bone_matrix_buf->commit(memory_budget::bone_buffer);

    if(api.feature_info().buffer.ubo)
    {
        resources.world_store =
            api.alloc_buffer(gfx::buffers::constants, access);
    } else
        resources.world_store = api.alloc_buffer(gfx::buffers::vertex, access);
    resources.world_store->alloc();
    resources.world_store->commit(sizeof(materials::world_data));

    resources.bsp_attr             = api.alloc_vertex_array();
    gfx::vertex_array_t& bsp_array = *resources.bsp_attr;
    bsp_array.alloc();

    using vertex_type = std::conditional_t<
        std::is_same_v<halo_version, blam::xbox_version_t>,
        ::xbox_vertex_type,
        ::vertex_type>;
    using light_vertex_type = std::conditional_t<
        std::is_same_v<halo_version, blam::xbox_version_t>,
        ::light_xbox_vertex_type,
        ::light_vertex_type>;

    std::array<gfx::vertex_attribute, 6> common_attributes = {{
        gfx::vertex_attribute::from_member(&vertex_type::position),
        gfx::vertex_attribute::from_member(&vertex_type::texcoord),
        gfx::vertex_attribute::from_member(&vertex_type::normal),
        gfx::vertex_attribute::from_member(&vertex_type::binorm),
        gfx::vertex_attribute::from_member(&vertex_type::tangent),
        gfx::vertex_attribute::from_member(
            &light_vertex_type::texcoord, gfx::vertex_float_type),
    }};

    {
        auto& light     = common_attributes.back();
        light.buffer.id = 1;
    }
    for(auto i : range<u32>(6))
    {
        common_attributes.at(i).index = i;
        bsp_array.add(common_attributes.at(i));
    }
    bsp_array.set_buffer(gfx::buffers::vertex, resources.bsp_buf, 0);
    bsp_array.set_buffer(gfx::buffers::vertex, resources.bsp_light_buf, 1);
    bsp_array.set_buffer(gfx::buffers::element, resources.bsp_index);
    bsp_array.set_attribute_names({
        {"position", 0},
        {"tex", 1},
        {"normal", 2},
        {"binormal", 3},
        {"tangent", 4},
        {"light_tex", 5},
    });

    using model_vertex_type = std::conditional_t<
        std::is_same_v<halo_version, blam::xbox_version_t>,
        blam::vert::mod2_vertex<blam::vert::compressed>,
        blam::vert::mod2_vertex<blam::vert::uncompressed>>;

    resources.model_attr            = api.alloc_vertex_array();
    gfx::vertex_array_t& mod2_array = *resources.model_attr;
    mod2_array.alloc();

    common_attributes = {{
        gfx::vertex_attribute::from_member(&model_vertex_type::position),
        gfx::vertex_attribute::from_member(&model_vertex_type::texcoord),
        gfx::vertex_attribute::from_member(&model_vertex_type::normal),
        gfx::vertex_attribute::from_member(&model_vertex_type::binorm),
        gfx::vertex_attribute::from_member(&model_vertex_type::tangent),
    }};

    if constexpr(std::is_same_v<halo_version, blam::xbox_version_t>)
        common_attributes.at(1) = gfx::vertex_attribute::from_member(
            &model_vertex_type::texcoord, gfx::vertex_float_type);

    for(auto i : range<u32>(5))
    {
        common_attributes.at(i).index = i;
        mod2_array.add(common_attributes.at(i));
    }

    /* Bone index and weight vertex attributes from mod2_vertex::weights */
    if constexpr(!std::is_same_v<halo_version, blam::xbox_version_t>)
    {
        using uvert = blam::vert::mod2_vertex<blam::vert::uncompressed>;
        using uwt   = blam::vert::uncompressed_weights;
        constexpr size_t wt_base = offsetof(uvert, weights);

        gfx::vertex_attribute node_idx_attr;
        node_idx_attr.index = 5;
        node_idx_attr.value = {
            .offset = wt_base + offsetof(uwt, node0),
            .stride = sizeof(uvert),
            .count  = 2,
            .type   = semantic::type_t::u16,
            .flags  = gfx::vertex_attribute::attribute_flags::none,
        };
        gfx::vertex_attribute node_wt_attr;
        node_wt_attr.index = 6;
        node_wt_attr.value = {
            .offset = wt_base + offsetof(uwt, weight0),
            .stride = sizeof(uvert),
            .count  = 2,
            .type   = semantic::type_t::f32,
            .flags  = gfx::vertex_attribute::attribute_flags::none,
        };
        mod2_array.add(node_idx_attr);
        mod2_array.add(node_wt_attr);
    } else
    {
        /* Xbox compressed weights: u8 node0/node1 (game stores them *3) and a
         * single u16 weight (the shader derives weight1 = 1 - weight0). */
        using cvert = blam::vert::mod2_vertex<blam::vert::compressed>;
        using cwt   = blam::vert::compressed_weights;
        constexpr size_t wt_base = offsetof(cvert, weights);

        gfx::vertex_attribute node_idx_attr;
        node_idx_attr.index = 5;
        node_idx_attr.value = {
            .offset = wt_base + offsetof(cwt, node0),
            .stride = sizeof(cvert),
            .count  = 2,
            .type   = semantic::type_t::u8,
            .flags  = gfx::vertex_attribute::attribute_flags::none,
        };
        gfx::vertex_attribute node_wt_attr;
        node_wt_attr.index = 6;
        /* Raw u16 (NOT normalized): Halo's full weight is 32767, not 65535, so
         * the shader divides by 32767 to get weight0 in [0,1]. */
        node_wt_attr.value = {
            .offset = wt_base + offsetof(cwt, weight0),
            .stride = sizeof(cvert),
            .count  = 1,
            .type   = semantic::type_t::u16,
            .flags  = gfx::vertex_attribute::attribute_flags::none,
        };
        mod2_array.add(node_idx_attr);
        mod2_array.add(node_wt_attr);
    }

    mod2_array.set_buffer(gfx::buffers::vertex, resources.model_buf, 0);
    mod2_array.set_buffer(gfx::buffers::element, resources.model_index);
    mod2_array.set_attribute_names({
        {"position", 0},
        {"tex", 1},
        {"i_normal", 2},
        {"i_binormal", 3},
        {"i_tangent", 4},
        {"i_node_indices", 5},
        {"i_node_weights", 6},
    });

    if(api.api_version() != std::make_tuple<u32, u32>(2, 0))
    {
        resources.debug_lines = api.alloc_buffer(gfx::buffers::vertex, access);
        resources.debug_lines->alloc();
        resources.debug_lines->commit(memory_budget::debug_buffer / 2);
        resources.debug_line_colors =
            api.alloc_buffer(gfx::buffers::vertex, access);
        resources.debug_line_colors->alloc();
        resources.debug_line_colors->commit(memory_budget::debug_buffer / 2);
    }

    /* Hand the debug-line buffers to DebugMarkers, the sole gateway for all
     * CPU-side writes/maps to them. */
    DebugMarkers& markers = e.subsystem_cast<DebugMarkers>();
    markers.lines         = resources.debug_lines;
    markers.colors        = resources.debug_line_colors;
    if(resources.debug_lines)
        markers.set_capacity(
            static_cast<u32>((memory_budget::debug_buffer / 2) / sizeof(Vecf3)),
            static_cast<u32>(
                (memory_budget::debug_buffer / 2) / sizeof(Vecf3)));

    if(!compile_info::platform::is_android && resources.debug_lines)
    {
        markers.map(0, 0);
        auto& pos = markers.portal_buffer;
        auto& col = markers.portal_color_buffer;
        pos[0]    = Vecf3(0);
        pos[1]    = Vecf3(10, 0, 0);
        pos[2]    = Vecf3(0);
        pos[3]    = Vecf3(0, 10, 0);
        pos[4]    = Vecf3(0);
        pos[5]    = Vecf3(0, 0, 10);

        col[0] = Vecf3(1, 0, 0);
        col[1] = Vecf3(0, 1, 0);
        col[2] = Vecf3(0, 0, 1);

        markers.unmap();

        compo::EntityRecipe debug_draw;
        debug_draw.components = {
            compo::type_hash_v<DebugDraw>(),
        };
        /* 3 axis lines (X/Y/Z), 2 verts each — matches debug_axes_verts/
         * debug_axes_colors in data.h and the 6 verts/3 colours written
         * just above. Physics bodies and occluder eye markers used to be
         * pre-created here too, capped at a compile-time count (16); they
         * now allocate their own slots lazily via
         * DebugMarkers::acquire_strip(), so any number of them fit, up to
         * the buffer's real capacity. */
        for(auto i : range<u32>(3))
        {
            auto  x_    = e.create_entity(debug_draw);
            auto& x     = x_.get<DebugDraw>();
            x.color_ptr = i;
            x.data      = gfx::draw_command::data_t{
                     .arrays =
                         {
                             .count  = 2,
                             .offset = 2 * i,
                    },
            };
        }
    }

    if(resources.debug_lines)
    {
        struct debug_vertex
        {
            Vecf3 pos;
        };

        auto& debug_attr = resources.debug_attr;
        debug_attr       = api.alloc_vertex_array();
        debug_attr->alloc();
        debug_attr->add(gfx::vertex_attribute::from_member(&debug_vertex::pos));
        auto color_vtx = gfx::vertex_attribute::from_member(&debug_vertex::pos);
        color_vtx.index     = 1;
        color_vtx.buffer.id = 1;
        color_vtx.value.flags =
            gfx::vertex_attribute::attribute_flags::instanced;
        debug_attr->add(color_vtx);
        debug_attr->set_buffer(gfx::buffers::vertex, resources.debug_lines, 0);
        debug_attr->set_buffer(
            gfx::buffers::vertex, resources.debug_line_colors, 1);
        debug_attr->set_attribute_names({{"position", 0}, {"color", 1}});
    }

    //    if constexpr(compile_info::platform::is_android)
    if(api.api_version() != std::make_tuple(2u, 0u))
    {
        auto const& features = api.feature_info().rendertarget;

        auto col_format = features.med_precision_color_format;
        auto dep_format = features.high_precision_depth_format;

        resources.offscreen = api.alloc_rendertarget();
        resources.color =
            api.alloc_texture(gfx::textures::d2, PixDesc(col_format), 1);
        resources.depth =
            api.alloc_texture(gfx::textures::d2, PixDesc(dep_format), 1);
        cDebug(
            "Creating offscreen buffer with: color={} depth={}",
            magic_enum::enum_name(resources.color->m_format.pixfmt),
            magic_enum::enum_name(resources.depth->m_format.pixfmt));
        resources.offscreen->alloc();
        auto const& size = resources.offscreen_size;
        resources.color->alloc(size_3d<i32>{size.x, size.y}.convert<u32>());
        resources.depth->alloc(size_3d<i32>{size.x, size.y}.convert<u32>());

        using gfx::render_targets::attachment;

        resources.offscreen->attach(attachment::color, *resources.color, 0);
        resources.offscreen->attach(attachment::depth, *resources.depth, 0);
        resources.offscreen->resize({0, 0, size.x, size.y});

        resources.color->set_swizzle(
            gfx::textures::swizzle_t::red,
            gfx::textures::swizzle_t::green,
            gfx::textures::swizzle_t::blue,
            gfx::textures::swizzle_t::one);

        using Coffee::Display::Event;
        using Coffee::Display::ResizeEvent;

        e.service<comp_app::BasicEventBus<Event>>()
            ->addEventFunction<ResizeEvent>(
                0, [&resources](Event&, ResizeEvent* resize) {
                    auto const& size = *resize;
                    resources.offscreen->resize(
                        typing::geometry::rect<i32>(0, 0, size.w, size.h));
                    resources.color->alloc(size_3d<u32>{size.w, size.h});
                    resources.depth->alloc(size_3d<u32>{size.w, size.h});
                    resources.offscreen_size = Veci2(size.w, size.h);
                });
    } else
        resources.offscreen = api.default_rendertarget();
}

static void create_shader_program(
    gfx::api&                        api,
    Resource&&                       blob,
    std::string_view                 vertex_entrypoint,
    std::string_view                 fragment_entrypoint,
    std::shared_ptr<gfx::program_t>& result,
    std::string_view                 label)
{
    result = api.alloc_program();
    result->add(
        gleam::program_t::stage_t::Vertex,
        api.alloc_shader(
            gfx::shader_format_t::spv, blob.data(), {}, vertex_entrypoint));
    result->add(
        gleam::program_t::stage_t::Fragment,
        api.alloc_shader(
            gfx::shader_format_t::spv, blob.data(), {}, fragment_entrypoint));
    if(auto res = result->compile(); res.has_error())
    {
        auto [msg] = res.error();
        cFatal("Failed to compile {0} shader: {1}", label, msg);
        result = {};
    }
}

static void create_spv_shaders(gfx::api& api, BlamResources& resources)
{
    using namespace std::string_view_literals;

    auto _ = api.debug().scope();

    create_shader_program(
        api,
        "shaders/debug_lines.spv"_rsc,
        "debug_lines_main"sv,
        "debug_lines_main"sv,
        resources.debug_lines_pipeline,
        "debug_lines");
    create_shader_program(
        api,
        "shaders/scenery_uber.spv"_rsc,
        "scenery_main"sv,
        "scenery_uber_main"sv,
        resources.model_pipeline,
        "scenery");
    create_shader_program(
        api,
        "shaders/map_uber.spv"_rsc,
        "map_main"sv,
        "map_uber_main"sv,
        resources.bsp_pipeline,
        "bsp");
    create_shader_program(
        api,
        "shaders/map_uber.spv"_rsc,
        "map_main"sv,
        "wireframe_main"sv,
        resources.wireframe_pipeline,
        "map_wireframe");
}

static void create_uber_shaders(gfx::api& api, BlamResources& resources)
{
    using namespace std::string_view_literals;
    using platform::url::constructors::MkUrl;

    const auto map_vertex = std::is_same_v<halo_version, blam::xbox_version_t>
                                ? "map_xbox"sv
                                : "map"sv;
    const auto scenery_vertex =
        std::is_same_v<halo_version, blam::xbox_version_t>
            ? (compile_info::platform::is_emscripten ? "scenery_xbox_lite"sv
                                                     : "scenery_xbox"sv)
            : (compile_info::platform::is_emscripten ? "scenery_lite"sv
                                                     : "scenery"sv);
    /* Xbox multipurpose maps are ARGB; use the matching fragment variant. */
    const auto scenery_frag = std::is_same_v<halo_version, blam::xbox_version_t>
                                  ? "scenery_uber_xbox"sv
                                  : "scenery_uber"sv;

    std::array<shader_pair_t, 4> shaders = {{
        {
            .vertex_file   = "debug_lines"sv,
            .fragment_file = "debug_lines"sv,
            .shader        = resources.debug_lines_pipeline,
        },
        {
            .vertex_file   = scenery_vertex,
            .fragment_file = scenery_frag,
            .shader        = resources.model_pipeline,
        },
        {
            .vertex_file   = map_vertex,
            .fragment_file = "map_uber"sv,
            .shader        = resources.bsp_pipeline,
        },
        {
            .vertex_file   = map_vertex,
            .fragment_file = "wireframe"sv,
            .shader        = resources.wireframe_pipeline,
        },
    }};

    create_shaders(api, std::move(shaders));
}

static void create_uber_lite_shaders(gfx::api& api, BlamResources& resources)
{
    using namespace std::string_view_literals;
    using platform::url::constructors::MkUrl;

    const auto map_vertex = std::is_same_v<halo_version, blam::xbox_version_t>
                                ? "map_xbox"sv
                                : "map"sv;
    const auto scenery_vertex =
        std::is_same_v<halo_version, blam::xbox_version_t>
            ? (compile_info::platform::is_emscripten ? "scenery_xbox_lite"sv
                                                     : "scenery_xbox"sv)
            : (compile_info::platform::is_emscripten ? "scenery_lite"sv
                                                     : "scenery"sv);

    std::array<shader_pair_t, 4> shaders = {{
        {
            .vertex_file   = "debug_lines"sv,
            .fragment_file = "debug_lines"sv,
            .shader        = resources.debug_lines_pipeline,
        },
        {
            .vertex_file   = scenery_vertex,
            .fragment_file = "scenery_uber_lite"sv,
            .shader        = resources.model_pipeline,
        },
        {
            .vertex_file   = map_vertex,
            .fragment_file = "map_uber_lite"sv,
            .shader        = resources.bsp_pipeline,
        },
        {
            .vertex_file   = map_vertex,
            .fragment_file = "wireframe"sv,
            .shader        = resources.wireframe_pipeline,
        },
    }};

    create_shaders(api, std::move(shaders));
}

static void create_standard_shaders(gfx::api& api, BlamResources& resources)
{
    using namespace std::string_view_literals;
    using platform::url::constructors::MkUrl;

    const auto map_vertex = std::is_same_v<halo_version, blam::xbox_version_t>
                                ? "map_xbox"sv
                                : "map"sv;
    const auto scenery_vertex =
        std::is_same_v<halo_version, blam::xbox_version_t>
            ? (compile_info::platform::is_emscripten ? "scenery_xbox_lite"sv
                                                     : "scenery_xbox"sv)
            : (compile_info::platform::is_emscripten ? "scenery_lite"sv
                                                     : "scenery"sv);

    std::array<shader_pair_t, 4> shaders = {{
        {
            .vertex_file   = "debug_lines"sv,
            .fragment_file = "debug_lines"sv,
            .shader        = resources.debug_lines_pipeline,
        },
        {
            .vertex_file   = scenery_vertex,
            .fragment_file = "scenery"sv,
            .shader        = resources.model_pipeline,
        },
        {
            .vertex_file   = map_vertex,
            .fragment_file = "map"sv,
            .shader        = resources.bsp_pipeline,
        },
        {
            .vertex_file   = "map"sv,
            .fragment_file = "wireframe"sv,
            .shader        = resources.wireframe_pipeline,
        },
    }};

    create_shaders(api, std::move(shaders));
}

static void create_legacy_shaders(gfx::api& api, BlamResources& resources)
{
    using namespace std::string_view_literals;
    using platform::url::constructors::MkUrl;

    // resources.debug_lines_pipeline = api.alloc_program();
    // resources.model_pipeline       = api.alloc_program();
    // resources.wireframe_pipeline   = api.alloc_program();

    resources.bsp_pipeline = api.alloc_program();
    resources.bsp_pipeline->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader("shaders/map_legacy.vert"_rsc.data()));
    resources.bsp_pipeline->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader("shaders/map_legacy.frag"_rsc.data()));
    auto res = resources.bsp_pipeline->compile();
    if(res.has_error())
    {
        cWarning("Failed to compile BSP shader: {}", std::get<0>(res.error()));
    }

    /* Diffuse-only scenery (mod2) program for the ES2 path; no skinning. */
    resources.model_pipeline = api.alloc_program();
    resources.model_pipeline->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader("shaders/scenery_legacy.vert"_rsc.data()));
    resources.model_pipeline->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader("shaders/scenery_legacy.frag"_rsc.data()));
    if(auto mres = resources.model_pipeline->compile(); mres.has_error())
        cWarning(
            "Failed to compile scenery shader: {}", std::get<0>(mres.error()));

    /* Simplified schi/scex combiner for the ES2 path (sky dome, multi-map
     * scenery). */
    resources.chicago_pipeline = api.alloc_program();
    resources.chicago_pipeline->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader("shaders/scenery_legacy.vert"_rsc.data()));
    resources.chicago_pipeline->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader("shaders/scenery_legacy_chicago.frag"_rsc.data()));
    if(auto cres = resources.chicago_pipeline->compile(); cres.has_error())
        cWarning(
            "Failed to compile chicago shader: {}", std::get<0>(cres.error()));

    /* Simple swat water for the ES2 path (scrolling base + blue tint). */
    resources.water_pipeline = api.alloc_program();
    resources.water_pipeline->add(
        gfx::program_t::stage_t::Vertex,
        api.alloc_shader("shaders/map_legacy.vert"_rsc.data()));
    resources.water_pipeline->add(
        gfx::program_t::stage_t::Fragment,
        api.alloc_shader("shaders/map_legacy_water.frag"_rsc.data()));
    if(auto wres = resources.water_pipeline->compile(); wres.has_error())
        cWarning(
            "Failed to compile water shader: {}", std::get<0>(wres.error()));
}

void create_shaders(compo::EntityContainer& e)
{
    gfx::api&      gfx       = e.subsystem_cast<gfx::system>();
    BlamResources& resources = e.subsystem_cast<BlamResources>();

    auto        _ = gfx.debug().scope();
    ProfContext __;

    auto const& features = gfx.feature_info();
    auto const& bugs     = gfx.workarounds().bugs;

    const bool use_spv = features.program.spirv;
    const bool use_uber =
        features.texture.cube_array /*&& features.buffer.ssbo*/
        && !lowspec_hardware && !bugs.adreno;
    const bool use_uber_lite = features.buffer.ubo /*&& !bugs.adreno_3xx*/;

    if(use_spv && false)
    {
        create_spv_shaders(gfx, resources);
        return;
    }

    if(use_uber)
    {
        create_uber_shaders(gfx, resources);
        return;
    }

    if(use_uber_lite)
    {
        create_uber_lite_shaders(gfx, resources);
        return;
    }

    create_legacy_shaders(gfx, resources);
}

void set_resource_labels(EntityContainer& e)
{
    // if constexpr(!compile_info::debug_mode)
    //     return;
    gfx::api&        api       = e.subsystem_cast<gfx::system>();
    gfx::debug::api& debug     = api.debug();
    BlamResources&   resources = e.subsystem_cast<BlamResources>();

    debug.annotate(*resources.bsp_pipeline, "map_basic");
    if(resources.model_pipeline)
        debug.annotate(*resources.model_pipeline, "scenery");
    if(resources.debug_lines_pipeline)
        debug.annotate(*resources.debug_lines_pipeline, "debug_lines");
    if(resources.wireframe_pipeline)
        debug.annotate(*resources.wireframe_pipeline, "wireframe");

    debug.annotate(*resources.bsp_attr, "bsp_vao");
    debug.annotate(*resources.bsp_buf, "bsp_vertex_buf");
    debug.annotate(*resources.bsp_light_buf, "bsp_light_buf");
    debug.annotate(*resources.bsp_index, "bsp_index_buf");

    debug.annotate(*resources.model_attr, "model_vao");
    debug.annotate(*resources.model_buf, "model_vertex_buf");
    debug.annotate(*resources.model_index, "model_index_buf");

    debug.annotate(*resources.material_store, "material_buffer");
    debug.annotate(*resources.model_matrix_store, "model_matrices");
    debug.annotate(*resources.bone_matrix_buf, "bone_matrices");

    if(resources.debug_lines)
    {
        debug.annotate(*resources.debug_attr, "debug_vao");
        debug.annotate(*resources.debug_lines, "debug_vertices");
    }

    if(api.default_rendertarget() != resources.offscreen)
        debug.annotate(*resources.offscreen, "offscreen");
}

void create_camera(
    compo::EntityContainer&                                          e,
    semantic::Span<const blam::scn::player_starting_location> const& spawns)
{
    u32 count{0};
    for(auto _ : e.select<PlayerCamera>())
        if(auto* info = e.get<PlayerInfo>(_.id()); info && !info->is_remote())
            ++count;
    auto& physics_bus = e.subsystem_cast<PhysicsBus>();
    for(auto entity : e.select<PlayerCamera, PlayerInfo>())
    {
        auto [cam, info]  = entity.components();
        cam.controller.opts.sens.move = {.1f, .1f};
        cam.camera_opts.accel.alt    = 50.f;

        if(spawns.empty())
            continue;
        auto& location =
            info.seat_idx < spawns.size() ? spawns[info.seat_idx] : spawns[0];
        cam.camera.position = location.pos;
        /* R_vertex = R_bsp * bsp_basis^T.
         * Ensures R_vertex * bsp_basis == R_bsp in the view matrix, so
         * rendering is correct while controller direction vectors are in
         * vertex space and can be added directly to the vertex-space position.
         */
        static const glm::mat3 bsp_basis_inv{{0, 1, 0}, {0, 0, 1}, {1, 0, 0}};
        cam.camera_opts.world_basis = bsp_basis_inv;
        cam.camera.rotation =
            glm::angleAxis(glm::pi<f32>() - location.rot, Vecf3{0.f, 1.f, 0.f});
        Physics::Event             event{Physics::Event::BodyCreationShape};
        Physics::BodyCreationShape create{
            .entity_id = entity.id(),
            .scale     = {0.1, 0, 0.5},
            .position  = location.pos + Vecf3{0, 0, 0.6},
            .mass      = 1,
            .shape     = Physics::BodyCreationShape::Capsule,
            .lock =
                {
                    .rotation = true,
                },
        };
        physics_bus.process(event, &create);
    }
    update_camera_aspect(e);
}

void create_program(gfx::api& api, shader_pair_t&& shader_info)
{
    create_shaders<1>(api, {{std::move(shader_info)}});
}
