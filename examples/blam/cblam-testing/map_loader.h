#pragma once

#include "coffee/comp_app/services.h"
#include "components.h"
#include "data.h"
#if defined(FEATURE_ENABLE_DiscordLatte)
#include "discord/discord_system.h"
#endif
#include "networking.h"
#include "selected_version.h"

#include <coffee/core/debug/formatting.h>
#include <coffee/core/files/cfiles.h>
#include <coffee/imgui/imgui_binding.h>
#include <imgui.h>
#include <peripherals/stl/type_list.h>
#include <url/url.h>

#include <memory>

using platform::url::Url;
using type_safety::empty_list_t;
using type_safety::type_list_t;

using Coffee::cDebug;

using BlamMapBrowserManifest = compo::SubsystemManifest<
    type_list_t<PlayerInfo, NetworkInfo, PlayerCamera>,
    type_list_t<
        GameEventBus,
        NetworkState,
        PlayerRoster
#if defined(FEATURE_ENABLE_DiscordLatte)
        ,
        discord::Subsystem
#endif
        >,
    type_list_t<comp_app::ControllerInput>>;

struct BlamMapBrowser
    : compo::RestrictedSubsystem<BlamMapBrowser, BlamMapBrowserManifest>
{
    using type  = BlamMapBrowser;
    using Proxy = compo::proxy_of<BlamMapBrowserManifest>;

    BlamMapBrowser(std::function<void(Url const&)>&& map_selected)
        : m_map_selected(std::move(map_selected))
    {
        priority       = 2048;
        remote_address = "0.0.0.0:16420";
        remote_address.resize(64);
    }

    void start_restricted(Proxy& e, time_point const&)
    {
        if(ImGui::Begin("Game"))
        {
            if(ImGui::BeginTabBar("Game Options"))
            {
                if(ImGui::BeginTabItem("Local"))
                {
                    if(ImGui::BeginListBox("Maps"))
                    {
                        for(auto const& map : m_maps)
                        {
                            auto fname = map.path().fileBasename().removeExt();
                            if(fname.internUrl.empty())
                                continue;
                            if(ImGui::Selectable(fname.internUrl.c_str()))
                            {
                                cDebug(
                                    "Selected: {0}", fname.internUrl.c_str());
                                try_load_map(map);
                            }
                        }
                        ImGui::EndListBox();
                    }

                    if(m_info)
                    {
                        ImGui::Separator();

                        ImGui::Columns(2);
                        display_property("Filename", m_file.internUrl);
                        display_property(
                            "Game version",
                            magic_enum::enum_name(m_info->version));
                        display_property("Name", m_info->name.str());
                        display_property("Full name", m_info->full_mapname());
                        display_property("Version", m_info->buildDate.str());
                        display_property(
                            "Map type",
                            magic_enum::enum_name(m_info->map_type));
                        ImGui::Columns();

                        ImGui::Separator();

                        if(ImGui::Button("Load map"))
                        {
                            auto&        gbus = e.subsystem<GameEventBus>();
                            GameEvent    ev{GameEvent::MapLoadStart};
                            MapLoadEvent load{.file = m_file};
                            gbus.inject(ev, &load);
                        }
                    }
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Player inputs"))
                {
                    auto controllers = e.service<comp_app::ControllerInput>();
                    ImGui::Columns(3);
                    for(auto const& player : e.select<PlayerCamera, PlayerInfo>())
                    {
                        if(!controllers)
                            continue;
                        auto [camera, info] = player.components();
                        ImGui::PushID(info.player_idx);
                        ImGui::Text(
                            "[i=%02i, seat=%02i] %s",
                            info.player_idx,
                            info.seat_idx,
                            info.name.c_str());
                        ImGui::NextColumn();
                        ImGui::Checkbox("Physics", &camera.mode.physics);
                        ImGui::Checkbox("Keyboard", &camera.keyboard.enabled);
                        ImGui::NextColumn();
                        int controller = camera.controller.index.value_or(-1);
                        ImGui::SliderInt(
                            "Controller: #%02i",
                            &controller,
                            -1,
                            controllers->count() - 1);
                        if(controller == -1)
                            camera.controller.index.reset();
                        else
                            camera.controller.index = controller;
                        ImGui::NextColumn();
                        ImGui::PopID();
                    }
                    ImGui::Columns();
                    ImGui::EndTabItem();
                }
                NetworkState* net_state;
                e.subsystem(net_state);
                // auto server_active = net_state->server_state
                //                  != NetworkState::ServerState::None;
                // auto client_active = net_state->client_state
                //                  != NetworkState::ClientState::None;
                if(ImGui::BeginTabItem("Client"))
                {
                    ImGui::Columns(2);
#if defined(FEATURE_ENABLE_DiscordLatte)
                    if(auto& discord = e.subsystem<discord::Subsystem>();
                       discord.connected())
                    {
                        ImGui::Text(
                            "Candidate name: %s (from Discord)",
                            e.subsystem<discord::Subsystem>()
                                .playerInfo()
                                .username.c_str());
                        ImGui::NextColumn();
                        ImGui::NextColumn();
                    }
#endif
                    ImGui::InputText(
                        "Server", remote_address.data(), remote_address.size());
                    ImGui::NextColumn();
                    if(ImGui::Button("Connect"))
                    {
                        auto&              gbus = e.subsystem<GameEventBus>();
                        GameEvent          ev{GameEvent::ServerConnect};
                        ServerConnectEvent connect{
                            .type   = ServerConnectEvent::Server,
                            .remote = std::string(remote_address.c_str()),
                        };
                        cDebug("Initiating connection to: {}", connect.remote);
                        gbus.inject(ev, &connect);
                    }
                    ImGui::Columns();
                    if(auto state =
                           magic_enum::enum_name(net_state->client_state);
                       state.size())
                        ImGui::Text(
                            "State: %.*s",
                            static_cast<int>(state.size()),
                            state.data());
                    if(auto remote = net_state->remote_address)
                        ImGui::Text("Connected to %s", remote->c_str());
                    {
                        auto& roster_sys = e.subsystem<PlayerRoster>();
                        auto  roster =
                            roster_sys.roster(net_state->remote_player_idx);
                        if(!roster.empty())
                        {
                            ImGui::Separator();
                            ImGui::Text("Players:");
                            for(auto const& entry : roster)
                            {
                                if(entry.is_self)
                                    ImGui::Text(
                                        " > [%u] %s (you)",
                                        entry.remote_idx,
                                        entry.name.c_str());
                                else
                                    ImGui::Text(
                                        " - [%u] %s",
                                        entry.remote_idx,
                                        entry.name.c_str());
                            }
                        }
                    }
                    if(ImGui::Button("Look at me!"))
                    {
                        auto& gbus = e.subsystem<GameEventBus>();
                        auto& net  = e.subsystem<NetworkState>();

                        GameEvent           ev{GameEvent::ServerCameraControl};
                        ServerCameraControl control{
                            .request = ServerCameraControl::RequestCameraFocus,
                            .target_player =
                                net.remote_player_idx.value_or(0xFFFF),
                        };
                        cDebug("Grabbing the server camera's attention");
                        gbus.inject(ev, &control);
                    }
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Server"))
                {
                    ImGui::Columns(2);
                    ImGui::InputText(
                        "Address",
                        remote_address.data(),
                        remote_address.size());
                    ImGui::NextColumn();
                    if(ImGui::Button("Listen"))
                    {
                        auto&              gbus = e.subsystem<GameEventBus>();
                        GameEvent          ev{GameEvent::ServerConnect};
                        ServerConnectEvent connect{
                            .type   = ServerConnectEvent::Listen,
                            .remote = std::string(remote_address.c_str()),
                        };
                        cDebug("Initiating connection to: {}", connect.remote);
                        gbus.inject(ev, &connect);
                    }
                    ImGui::Columns();
                    if(auto state =
                           magic_enum::enum_name(net_state->server_state);
                       state.size())
                        ImGui::Text(
                            "State: %.*s",
                            static_cast<int>(state.size()),
                            state.data());
                    ImGui::Columns(2);
                    if(auto local_name = net_state->local_address)
                    {
                        ImGui::Text("Server (%s)", local_name->c_str());
                        ImGui::NextColumn();
                        if(ImGui::Button("Return"))
                        {
                            /* Swap seat 0 back to player_idx 0 */
                            PlayerInfo* old_seat0 = nullptr;
                            PlayerInfo* original  = nullptr;
                            for(auto pe : e.select<PlayerInfo>())
                            {
                                auto* pi = e.get<PlayerInfo>(pe.id());
                                if(pi && pi->seat_idx == 0)
                                    old_seat0 = pi;
                                if(pi && pi->player_idx == 0)
                                    original = pi;
                            }
                            if(old_seat0 && original && old_seat0 != original)
                            {
                                std::swap(
                                    old_seat0->seat_idx, original->seat_idx);
                                /* Move keyboard.enabled */
                                for(auto pe : e.select<PlayerCamera>())
                                {
                                    auto* cam = e.get<PlayerCamera>(pe.id());
                                    auto* pi  = e.get<PlayerInfo>(pe.id());
                                    if(cam && pi)
                                        cam->keyboard.enabled =
                                            (pi->seat_idx == 0);
                                }
                            }
                        }
                        ImGui::NextColumn();
                    }
                    for(auto const& player : e.select<PlayerInfo>())
                    {
                        auto const& pinfo = *e.get<PlayerInfo>(player.id());
                        auto const& net_i = *e.get<NetworkInfo>(player.id());
                        if(!net_i.connected && !pinfo.remote.empty())
                            ImGui::TextColored(
                                ImVec4(0.7, 0.7, 0.7, 1.0),
                                " - %s (%s) [unstable]",
                                pinfo.name.c_str(),
                                pinfo.remote.c_str());
                        else if(pinfo.loading_progress < 100)
                            ImGui::Text(
                                " - %s (%s) [%u%%]",
                                pinfo.name.c_str(),
                                pinfo.remote.c_str(),
                                pinfo.loading_progress);
                        else
                            ImGui::Text(
                                " - %s (%s)",
                                pinfo.name.c_str(),
                                pinfo.remote.c_str());
                        ImGui::NextColumn();
                        if(ImGui::Button(
                               Coffee::Strings::fmt(
                                   "Focus {}", pinfo.player_idx)
                                   .c_str()))
                        {
                            /* Swap seat_idx: target gets seat 0, old seat 0
                             * gets target's seat */
                            PlayerInfo* old_seat0   = nullptr;
                            u32         target_pidx = pinfo.player_idx;
                            for(auto pe : e.select<PlayerInfo>())
                            {
                                auto* pi = e.get<PlayerInfo>(pe.id());
                                if(pi && pi->seat_idx == 0)
                                {
                                    old_seat0 = pi;
                                    break;
                                }
                            }
                            /* pinfo is const here, find mutable */
                            PlayerInfo* target = nullptr;
                            for(auto pe : e.select<PlayerInfo>())
                            {
                                auto* pi = e.get<PlayerInfo>(pe.id());
                                if(pi && pi->player_idx == target_pidx)
                                {
                                    target = pi;
                                    break;
                                }
                            }
                            if(old_seat0 && target && old_seat0 != target)
                            {
                                std::swap(
                                    old_seat0->seat_idx, target->seat_idx);
                                for(auto pe : e.select<PlayerCamera, PlayerInfo>())
                                {
                                    auto [cam, pi] = pe.components();
                                    cam.keyboard.enabled = pi.seat_idx == 0;
                                }
                            }
                        }
                        ImGui::NextColumn();
                    }
                    ImGui::Columns();
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Entities"))
                {
                    auto& ec = e.unconstrained_container();
                    if(ImGui::BeginListBox("##entities"))
                    {
                        u32 entity_idx = 0;
                        for(auto entity : e.select(0))
                        {
                            char label[64];
                            snprintf(
                                label,
                                sizeof(label),
                                "E %llu [0x%llX]##%u",
                                static_cast<unsigned long long>(entity.id()),
                                static_cast<unsigned long long>(entity.tags()),
                                entity_idx);
                            if(ImGui::Selectable(
                                   label, m_selected_entity == entity.id()))
                                m_selected_entity = entity.id();
                            entity_idx++;
                        }
                        ImGui::EndListBox();
                    }
                    if(m_selected_entity && ec.exists(m_selected_entity))
                    {
                        ImGui::Text(
                            "Entity %llu",
                            static_cast<unsigned long long>(m_selected_entity));
                        auto tags = ec.tags_of(m_selected_entity);
                        ImGui::Text(
                            "Tags: 0x%llX",
                            static_cast<unsigned long long>(tags));
                        ImGui::Text("Components:");
                        auto check = [&](const char* name, auto* ptr) {
                            if(ptr)
                                ImGui::BulletText("%s", name);
                        };
                        check("Model", ec.get<Model>(m_selected_entity));
                        check("SubModel", ec.get<SubModel>(m_selected_entity));
                        check(
                            "BspReference",
                            ec.get<BspReference>(m_selected_entity));
                        check(
                            "ObjectSpawn",
                            ec.get<ObjectSpawn>(m_selected_entity));
                        check(
                            "NetworkInfo",
                            ec.get<NetworkInfo>(m_selected_entity));
                        check(
                            "PlayerInfo",
                            ec.get<PlayerInfo>(m_selected_entity));
                        check(
                            "PlayerCamera",
                            ec.get<PlayerCamera>(m_selected_entity));
                        check(
                            "SoundEffects",
                            ec.get<SoundEffects>(m_selected_entity));
                        check(
                            "MultiplayerSpawn",
                            ec.get<MultiplayerSpawn>(m_selected_entity));
                        check(
                            "ShaderData",
                            ec.get<ShaderData>(m_selected_entity));
                        check(
                            "MeshTrackingData",
                            ec.get<MeshTrackingData>(m_selected_entity));
                        check(
                            "DebugDraw", ec.get<DebugDraw>(m_selected_entity));
                        check(
                            "TriggerVolume",
                            ec.get<TriggerVolume>(m_selected_entity));
                        check("Light", ec.get<Light>(m_selected_entity));
                        check(
                            "DepthInfo", ec.get<DepthInfo>(m_selected_entity));

                        if(auto* pinfo = ec.get<PlayerInfo>(m_selected_entity))
                        {
                            ImGui::Separator();
                            ImGui::Text("PlayerInfo:");
                            ImGui::Text("  name: %s", pinfo->name.c_str());
                            ImGui::Text(
                                "  loading: %u%%", pinfo->loading_progress);
                            ImGui::Text(
                                "  remote: %s",
                                pinfo->remote.empty() ? "<local>"
                                                      : pinfo->remote.c_str());
                            ImGui::Text("  seat_idx: %u", pinfo->seat_idx);
                            ImGui::Text("  player_idx: %u", pinfo->player_idx);
                            ImGui::Text("  permissions:");
                            ImGui::Text(
                                "    move: %u", pinfo->permissions.move);
                            ImGui::Text(
                                "    camera: %u", pinfo->permissions.camera);
                        }
                        if(auto* pcam = ec.get<PlayerCamera>(m_selected_entity))
                        {
                            ImGui::Separator();
                            ImGui::Text("PlayerCamera");
                            ImGui::Text(
                                "  keyboard and mouse: %s",
                                pcam->keyboard.enabled ? "enabled"
                                                       : "disabled");
                            ImGui::Text(
                                "  controller: %u",
                                pcam->controller.index.value_or(0xFF));
                            ImGui::DragFloat3(
                                "  position",
                                &pcam->camera.position.x,
                                2.f,
                                -1000.f,
                                1000.f);
                        }
                        if(auto* neti = ec.get<NetworkInfo>(m_selected_entity))
                        {
                            ImGui::Separator();
                            ImGui::Text("NetworkInfo");
                            ImGui::Text(
                                "  connected: %s",
                                neti->connected ? "true" : "false");
                            ImGui::Text(
                                "  tag: class=%.*s id=%u instance=%u",
                                4,
                                neti->object.tag_class_name().data(),
                                neti->object.tag_id,
                                neti->instance_id);
                        }
                    }
                    ImGui::EndTabItem();
                }
                if(m_error)
                {
                    auto error = magic_enum::enum_name(*m_error);
                    ImGui::Text(
                        "Failed to parse map %s", m_file.internUrl.c_str());
                    ImGui::Text(
                        "Error occurred on map load: %.*s",
                        static_cast<int>(error.size()),
                        error.data());
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void end_restricted(Proxy&, time_point const&)
    {
    }

    void try_load_map(Url const& map)
    {
        m_map   = {};
        m_info  = nullptr;
        m_error = std::nullopt;

        m_file = map;
        m_map  = std::make_unique<Coffee::Resource>(m_file);

        if constexpr(std::is_same_v<halo_version, blam::trial_version_t>)
        {
            /* Trial/Demo headers are scrambled; translate to the retail layout
             * for display (kept in m_info_storage, the browser shows one map at
             * a time). */
            semantic::BytesConst data = *m_map;
            if(data.size < sizeof(blam::file_header_trial_t))
                m_error = blam::map_load_error::map_file_too_small;
            else if(auto const* trial =
                        reinterpret_cast<blam::file_header_trial_t const*>(
                            data.data);
                    !trial->valid())
                m_error = blam::map_load_error::
                    incompatible_map_version_expected_trial;
            else
            {
                reinterpret_cast<blam::file_header_trial_t const*>(data.data)
                    ->to_retail()
                    .copy_to(m_info_storage);
                m_info = &m_info_storage;
            }
        } else if(auto info =
                      blam::file_header_t::from_data(*m_map, halo_version_v);
                  info.has_error())
            m_error = info.error();
        else
            m_info = info.value();
    }

    void display_property(std::string name, std::string value)
    {
        ImGui::Text("%s", name.c_str());
        ImGui::NextColumn();
        ImGui::Text("%s", value.c_str());
        ImGui::NextColumn();
    }

    void display_property(std::string name, std::string_view value)
    {
        ImGui::Text("%s", name.c_str());
        ImGui::NextColumn();
        ImGui::Text("%.*s", static_cast<int>(value.size()), value.data());
        ImGui::NextColumn();
    }

    std::function<void(platform::url::Url const&)> m_map_selected;
    platform::url::Url                             m_file;
    std::unique_ptr<Coffee::Resource>              m_map;
    blam::file_header_t const*                     m_info{nullptr};
    blam::file_header_t m_info_storage{}; /* normalized header for Trial maps */
    std::optional<blam::map_load_error> m_error;
    std::vector<Url>                    m_maps;
    std::string                         remote_address;
    u64                                 m_selected_entity{0};
};
