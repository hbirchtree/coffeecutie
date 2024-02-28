#pragma once

#include "components.h"
#include "data.h"
#include "networking.h"
#include "selected_version.h"

#include <coffee/core/debug/formatting.h>
#include <coffee/core/files/cfiles.h>
#include <coffee/imgui/imgui_binding.h>
#include <peripherals/stl/type_list.h>
#include <url/url.h>

#include <memory>

using platform::url::Url;
using type_safety::empty_list_t;
using type_safety::type_list_t;

using Coffee::cDebug;

using BlamMapBrowserManifest = compo::SubsystemManifest<
    type_list_t<PlayerInfo>,
    type_list_t<GameEventBus, NetworkState, BlamCamera>,
    empty_list_t>;

struct BlamMapBrowser
    : compo::RestrictedSubsystem<BlamMapBrowser, BlamMapBrowserManifest>
{
    using type  = BlamMapBrowser;
    using Proxy = compo::proxy_of<BlamMapBrowserManifest>;

    BlamMapBrowser(std::function<void(Url const&)>&& map_selected)
        : m_map_selected(std::move(map_selected))
    {
        priority       = 2048;
        remote_address = "127.0.0.1:16420";
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
                NetworkState* net_state;
                e.subsystem(net_state);
                // auto server_active = net_state->server_state
                //                  != NetworkState::ServerState::None;
                // auto client_active = net_state->client_state
                //                  != NetworkState::ClientState::None;
                if(ImGui::BeginTabItem("Client"))
                {
                    ImGui::Columns(2);
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
                    BlamCamera& camera = e.subsystem<BlamCamera>();
                    if(auto local_name = net_state->local_address)
                    {
                        ImGui::Text(" - Server (%s)", local_name->c_str());
                        ImGui::NextColumn();
                        if(ImGui::Button("Focus"))
                            camera.focused_player = 0;
                        ImGui::NextColumn();
                    }
                    for(auto const& player : e.select<PlayerInfo>())
                    {
                        auto const& pinfo =
                            e.ref<Proxy>(player.id).get<PlayerInfo>();
                        ImGui::Text(
                            " - %s (%s)",
                            pinfo.name.c_str(),
                            pinfo.remote.c_str());
                        ImGui::NextColumn();
                        if(ImGui::Button("Focus"))
                            camera.focused_player = pinfo.player_idx;
                        ImGui::NextColumn();
                    }
                    ImGui::Columns();
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
        if(auto info = blam::file_header_t::from_data(*m_map, halo_version_v);
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
    std::optional<blam::map_load_error>            m_error;
    std::vector<Url>                               m_maps;
    std::string                                    remote_address;
};
