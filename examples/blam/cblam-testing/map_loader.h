#pragma once

#include "data.h"

#include <coffee/core/files/cfiles.h>
#include <coffee/imgui/imgui_binding.h>

#include <memory>

using BlamMapBrowserManifest = Components::
    SubsystemManifest<empty_list_t, type_list_t<BlamFiles>, empty_list_t>;

struct BlamMapBrowser
    : Components::RestrictedSubsystem<BlamMapBrowser, BlamMapBrowserManifest>
{
    using type  = BlamMapBrowser;
    using Proxy = Components::proxy_of<BlamMapBrowserManifest>;

    BlamMapBrowser(std::function<void(Url const&)>&& map_selected) :
        m_map_selected(std::move(map_selected))
    {
        priority = 2048;
    }

    void start_restricted(Proxy& e, time_point const&)
    {
        if(ImGui::Begin("Maps"))
        {
            if(ImGui::BeginListBox("Maps"))
            {
                auto const& files = e.subsystem<BlamFiles>();

                for(auto const& map : files.maps)
                {
                    auto fname = map.path().fileBasename().removeExt();
                    if(fname.internUrl.empty())
                        continue;
                    if(ImGui::Selectable(fname.internUrl.c_str()))
                    {
                        cDebug("Selected: {0}", fname.internUrl.c_str());
                        try_load_map(map);
                    }
                }
            }
            ImGui::EndListBox();

            if(m_info)
            {
                ImGui::Separator();

                ImGui::Columns(2);
                display_property("Filename", m_file.internUrl);
                display_property(
                    "Game version", magic_enum::enum_name(m_info->version));
                display_property("Name", m_info->name.str());
                display_property("Full name", m_info->full_mapname());
                display_property("Version", m_info->buildDate.str());
                display_property(
                    "Map type", magic_enum::enum_name(m_info->map_type));
                ImGui::Columns();

                ImGui::Separator();

                if(ImGui::Button("Load map"))
                {
                    m_map_selected(m_file);
                }
            }
            if(m_error)
            {
                auto error = magic_enum::enum_name(*m_error);
                ImGui::Text("Failed to parse map %s", m_file.internUrl.c_str());
                ImGui::Text(
                    "Error occurred on map load: %.*s",
                    static_cast<int>(error.size()),
                    error.data());
            }
        }
        ImGui::End();
    }
    void end_restricted(Proxy&, time_point const&)
    {
    }

    void try_load_map(Url const& map)
    {
        m_map  = {};
        m_info = nullptr;
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
};
