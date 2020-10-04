#pragma once

#include "caching.h"
#include "components.h"
#include "data.h"
#include <coffee/imgui/imgui_binding.h>

template<typename Version>
struct BlamBspWidget;

template<typename Version>
using BlamBspWidgetTag = Components::TaggedTypeWrapper<BlamBspWidget<Version>>;

template<typename Version>
struct BlamBspWidget : Components::RestrictedSubsystem<
                           BlamBspWidgetTag<Version>,
                           type_list_t<BspTag, ShaderTag, SubModelTag>,
                           empty_list_t>
{
    using parent_type = Components::RestrictedSubsystem<
        BlamBspWidgetTag<Version>,
        type_list_t<BspTag, ShaderTag, SubModelTag>,
        empty_list_t>;

    using Proxy = typename parent_type::Proxy;

    BlamBspWidget(BlamData<Version>& data) :
        m_data(&data), m_map(&m_data->map_container)
    {
    }

    virtual void start_restricted(Proxy& e, time_point const&) override
    {
        using Components::Entity;
        using Components::EntityRef;

        auto bsps   = e.template select<BspTag>();
        auto models = e.template select<SubModelTag>();

        if(ImGui::Begin("Static models"))
        {
            CString current_bsp;
            bool current_hidden = true;

            for(Entity& bsp_ : bsps)
            {
                auto                bsp_e = e.template ref<Proxy>(bsp_);
                BspReference& bsp   = bsp_e.template get<BspTag>();

                BSPItem const&    bsp_it = m_data->bsp_cache.find(bsp.bsp)->second;
                ShaderItem const& shader =
                    m_data->shader_cache.find(bsp.shader)->second;

                auto bsp_name    = m_map->get_name(bsp_it.tag);
                auto shader_name = m_map->get_name(shader.tag);

                if(current_bsp != bsp_name)
                {
                    if(!current_bsp.empty() && !current_hidden)
                        ImGui::TreePop();
                    current_bsp = bsp_name;
                    current_hidden = !ImGui::TreeNode(current_bsp.c_str());
                }

                if(auto it = m_bsps.find(bsp_name); it == m_bsps.end())
                    m_bsps.insert({bsp_name, false});

                bsp.visible = m_bsps.at(bsp_name);

                if(current_hidden)
                    continue;

                ImGui::Checkbox(shader_name, &bsp.visible);
            }

            if(!current_bsp.empty() && !current_hidden)
                ImGui::TreePop();
        }
        ImGui::End();

        if(ImGui::Begin("Sectors"))
        {
            for(auto& region : m_bsps)
                ImGui::Checkbox(region.first.c_str(), &region.second);
        }
        ImGui::End();
    }
    virtual void end_restricted(Proxy&, time_point const&) override
    {
    }

    BlamData<Version>*         m_data;
    blam::map_container const* m_map;

    Map<CString, bool> m_bsps;
};
