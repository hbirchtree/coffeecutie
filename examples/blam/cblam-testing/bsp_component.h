#pragma once

#include "caching.h"
#include "components.h"
#include "data.h"
#include <coffee/imgui/imgui_binding.h>

template<typename Version>
struct BlamBspWidget;

template<typename V>
using BlamBspWidgetManifest = Components::SubsystemManifest<
    type_list_t<BspReference, ShaderData, SubModel>,
    type_list_t<ShaderCache<V>, BSPCache<V>>,
    empty_list_t>;

template<typename V>
struct BlamBspWidget : Components::RestrictedSubsystem<
                           BlamBspWidget<V>,
                           BlamBspWidgetManifest<V>>
{
    using type  = BlamBspWidget<V>;
    using Proxy = Components::proxy_of<BlamBspWidgetManifest<V>>;

    BlamBspWidget(BlamData<V>* data) :
        m_data(data), m_map(&m_data->map_container)
    {
    }

    void start_restricted(Proxy& e, time_point const&)
    {
        using Components::Entity;
        using Components::EntityRef;

        auto bsps   = e.template select<BspReference>();
        auto models = e.template select<SubModel>();
        auto& shader_cache = e.template subsystem<ShaderCache<V>>();
        auto& bsp_cache = e.template subsystem<BSPCache<V>>();

        if(ImGui::Begin("Static models"))
        {
            CString current_bsp;
            bool    current_hidden = true;

            for(Entity& bsp_ : bsps)
            {
                auto          bsp_e = e.template ref<Proxy>(bsp_);
                BspReference& bsp   = bsp_e.template get<BspReference>();

                BSPItem const& bsp_it = bsp_cache.find(bsp.bsp)->second;
                ShaderItem const& shader
                    = shader_cache.find(bsp.shader)->second;

                auto bsp_name    = m_map->get_name(bsp_it.tag);
                auto shader_name = m_map->get_name(shader.tag);

                if(current_bsp != bsp_name)
                {
                    if(!current_bsp.empty() && !current_hidden)
                        ImGui::TreePop();
                    current_bsp    = bsp_name;
                    current_hidden = !ImGui::TreeNode(current_bsp.c_str());
                }

                if(auto it = m_bsps.find(bsp_name); it == m_bsps.end())
                    m_bsps.insert({bsp_name, false});

                bsp.visible = m_bsps.at(bsp_name);

                if(current_hidden)
                    continue;

                ImGui::Checkbox(shader_name.data(), &bsp.visible);
                if(bsp.visible)
                    cDebug("{0}", bsp.visible);
            }

            if(!current_bsp.empty() && !current_hidden)
                ImGui::TreePop();
        }
        ImGui::End();

        if(ImGui::Begin("Sectors"))
        {
            String name;
            for(auto& region : m_bsps)
            {
                name.clear();
                name.insert(name.begin(), region.first.begin(), region.first.end());
                ImGui::Checkbox(name.c_str(), &region.second);
            }
        }
        ImGui::End();
    }
    void end_restricted(Proxy&, time_point const&)
    {
    }

    BlamData<V>*                  m_data;
    blam::map_container<V> const* m_map;

    Map<std::string_view, bool> m_bsps;
};
