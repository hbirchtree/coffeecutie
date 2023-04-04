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
    type_list_t<
        ShaderCache<V>,
        BSPCache<V>,
        BlamCamera,
        PostProcessParameters,
        RenderingParameters>,
    empty_list_t>;

template<typename V>
struct BlamBspWidget
    : Components::
          RestrictedSubsystem<BlamBspWidget<V>, BlamBspWidgetManifest<V>>
{
    using type  = BlamBspWidget<V>;
    using Proxy = Components::proxy_of<BlamBspWidgetManifest<V>>;

    BlamBspWidget(BlamData<V>* data) : m_map(&data->map_container)
    {
        compo::SubsystemBase::priority = 2048;
    }

    void start_restricted(Proxy& e, time_point const&)
    {
        using Components::Entity;
        using Components::EntityRef;

        auto  bsps         = e.template select<BspReference>();
        auto  models       = e.template select<SubModel>();
        auto& shader_cache = e.template subsystem<ShaderCache<V>>();
        auto& bsp_cache    = e.template subsystem<BSPCache<V>>();

        if(ImGui::Begin("Static models"))
        {
            CString current_bsp;
            bool    current_hidden = true;

            for(Entity& bsp_ : bsps)
            {
                auto          bsp_e = e.template ref<Proxy>(bsp_);
                BspReference& bsp   = bsp_e.template get<BspReference>();

                BSPItem const&    bsp_it = bsp_cache.find(bsp.bsp)->second;
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
                    m_bsps.insert({bsp_name, true});

                bsp.visible = m_bsps.at(bsp_name);

                if(current_hidden)
                    continue;

                ImGui::Checkbox(shader_name.data(), &m_bsps.at(bsp_name));
            }

            if(!current_bsp.empty() && !current_hidden)
                ImGui::TreePop();
        }
        ImGui::End();

        if(ImGui::Begin("Sectors"))
        {
            RenderingParameters* rendering;
            e.subsystem(rendering);

            ImGui::Checkbox("Render scenery", &rendering->render_scenery);

            String name;
            for(auto& region : m_bsps)
            {
                name.clear();
                name.insert(
                    name.begin(), region.first.begin(), region.first.end());
                ImGui::Checkbox(name.c_str(), &region.second);
            }
        }
        ImGui::End();

        if(ImGui::Begin("Graphics"))
        {
            BlamCamera const*      camera;
            PostProcessParameters* postprocess;
            e.subsystem(postprocess);
            e.subsystem(camera);

            ImGui::Text("Camera properties");
            ImGui::Columns(2);
            ImGui::Text("Position");
            ImGui::NextColumn();
            ImGui::Text(
                "vec3(%f, %f, %f)",
                camera->camera.position.x(),
                camera->camera.position.y(),
                camera->camera.position.z());
            ImGui::SliderFloat("Gamma", &postprocess->gamma, 0.1, 5.0);
            ImGui::SliderFloat("Exposure", &postprocess->exposure, -10.f, 10.f);
            ImGui::Columns();
        }
        ImGui::End();
    }
    void end_restricted(Proxy&, time_point const&)
    {
    }

    blam::map_container<V> const* m_map;

    Map<std::string_view, bool> m_bsps;
};
