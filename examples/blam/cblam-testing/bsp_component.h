#pragma once

#include "caching.h"
#include "components.h"
#include "data.h"
#include "resource_creation.h"

#include <coffee/imgui/imgui_binding.h>

template<typename Version>
struct BlamBspWidget;

template<typename V>
using BlamBspWidgetManifest = compo::SubsystemManifest<
    type_list_t<BspReference, ShaderData, SubModel, TriggerVolume, DebugDraw,
                PlayerCamera, PlayerInfo>,
    type_list_t<
        ShaderCache<V>,
        BSPCache<V>,
        BlamResources,
        PostProcessParameters,
        RenderingParameters,
        SoundPreferences>,
    empty_list_t>;

template<typename V>
struct BlamBspWidget
    : compo::RestrictedSubsystem<BlamBspWidget<V>, BlamBspWidgetManifest<V>>
{
    using type  = BlamBspWidget<V>;
    using Proxy = compo::proxy_of<BlamBspWidgetManifest<V>>;

    BlamBspWidget()
    {
        compo::SubsystemBase::priority = 2048;
    }

    void start_restricted(Proxy& e, time_point const&)
    {
        using compo::Entity;
        using compo::EntityRef;

        auto bsps     = e.template select<BspReference>();
        auto models   = e.template select<SubModel>();
        auto triggers = e.template select<TriggerVolume>();
        //        auto& shader_cache = e.template subsystem<ShaderCache<V>>();
        //        auto& bsp_cache    = e.template subsystem<BSPCache<V>>();

        //        if(ImGui::Begin("Static models"))
        //        {
        //            std::string current_bsp;
        //            bool        current_hidden = true;

        //            for(Entity& bsp_ : bsps)
        //            {
        //                auto          bsp_e = e.template ref<Proxy>(bsp_);
        //                BspReference& bsp   = bsp_e.template
        //                get<BspReference>();

        //                BSPItem const&    bsp_it =
        //                bsp_cache.find(bsp.bsp)->second; ShaderItem const&
        //                shader
        //                    = shader_cache.find(bsp.shader)->second;

        //                auto bsp_name    = m_map->get_name(bsp_it.tag);
        //                auto shader_name = m_map->get_name(shader.tag);

        //                if(current_bsp != bsp_name)
        //                {
        //                    if(!current_bsp.empty() && !current_hidden)
        //                        ImGui::TreePop();
        //                    current_bsp    = bsp_name;
        //                    current_hidden =
        //                    !ImGui::TreeNode(current_bsp.c_str());
        //                }

        //                if(auto it = m_bsps.find(bsp_name); it ==
        //                m_bsps.end())
        //                    m_bsps.insert({bsp_name, true});

        //                bsp.visible = m_bsps.at(bsp_name);

        //                if(current_hidden)
        //                    continue;

        //                ImGui::Checkbox(shader_name.data(),
        //                &m_bsps.at(bsp_name));
        //            }

        //            if(!current_bsp.empty() && !current_hidden)
        //                ImGui::TreePop();
        //        }
        //        ImGui::End();

        if(ImGui::Begin("Rendering"))
        {
            RenderingParameters* rendering;
            e.subsystem(rendering);

            ImGui::Checkbox("Render UI", &rendering->render_ui);

            if(ImGui::BeginTabBar(""))
            {
                if(ImGui::BeginTabItem("BSP"))
                {
                    ImGui::Checkbox(
                        "Render scenery", &rendering->render_scenery);
                    ImGui::Checkbox(
                        "Clear before draw", &rendering->debug_clear);
                    ImGui::Checkbox(
                        "Show clusters", &rendering->debug_clusters);
                    ImGui::Checkbox(
                        "Show debug markers", &rendering->debug_markers);
                    ImGui::Checkbox("Show portals", &rendering->debug_portals);
                    for(auto& region : m_bsps)
                    {
                        std::string name(
                            region.first.begin(), region.first.end());
                        ImGui::Checkbox(name.c_str(), &region.second);
                    }
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Models"))
                {
                    ImGui::Checkbox(
                        "Color changing", &rendering->color_changing);
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Trigger volumes"))
                {
                    ImGui::Checkbox(
                        "Show trigger volumes", &rendering->debug_triggers);
                    for(Entity& trigger : triggers)
                    {
                        auto t = e.template ref<Proxy>(trigger);

                        DebugDraw&     draw = t.template get<DebugDraw>();
                        TriggerVolume& trig = t.template get<TriggerVolume>();

                        ImGui::Checkbox(
                            trig.trigger_volume->name.str().data(),
                            &draw.selected);
                    }
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Camera"))
                {
                    BlamResources*         resources;
                    PostProcessParameters* postprocess;
                    e.subsystem(postprocess);
                    e.subsystem(resources);

                    /* Find primary (seat_idx==0) PlayerCamera */
                    PlayerCamera* primary_cam = nullptr;
                    u32 num_cameras = 0;
                    for(auto& ent : e.template select<PlayerCamera>())
                    {
                        auto* info = e.template get<PlayerInfo>(ent.id);
                        auto* cam  = e.template get<PlayerCamera>(ent.id);
                        if(!info || !cam)
                            continue;
                        ++num_cameras;
                        if(info->seat_idx == m_selected_camera)
                            primary_cam = cam;
                    }

                    ImGui::Text("Camera properties");
                    {
                        auto label = fmt::format("Player {}", m_selected_camera);
                        if(ImGui::BeginCombo("Selected camera", label.c_str()))
                        {
                            for(auto i : range<>(num_cameras))
                            {
                                auto opt = fmt::format("Player {}", i);
                                if(ImGui::Selectable(opt.c_str()))
                                    m_selected_camera = i;
                            }
                            ImGui::EndCombo();
                        }
                    }
                    ImGui::Columns(2);
                    ImGui::Text("Position");
                    ImGui::NextColumn();
                    if(primary_cam)
                    {
                        ImGui::Text(
                            "vec3(%f, %f, %f)",
                            primary_cam->camera->position[0],
                            primary_cam->camera->position[1],
                            primary_cam->camera->position[2]);
                        ImGui::Text("Aspect: %f", primary_cam->camera->aspect);
                        ImGui::SliderFloat(
                            "Draw distance",
                            &rendering->draw_distance,
                            1.f,
                            5000.f);
                        ImGui::SliderFloat(
                            "FOV", &primary_cam->camera->fieldOfView, 10.f, 120.f);
                    }
                    ImGui::SliderFloat("Gamma", &postprocess->gamma, 0.1, 5.0);
                    ImGui::SliderFloat(
                        "Exposure", &postprocess->exposure, -10.f, 10.f);
                    ImGui::SliderFloat(
                        "Texture resolution", &rendering->tex_res, -20.f, 20.f);
                    ImGui::SliderInt(
                        "Texture resolution (load)",
                        reinterpret_cast<i32*>(&rendering->mipmap_bias),
                        0,
                        4);
                    if(ImGui::Checkbox("Doom mode", &postprocess->doom_mode))
                        postprocess->scale =
                            postprocess->doom_mode ? 0.25f : 1.f;
                    ImGui::Text(
                        "Offscreen size: %ix%i",
                        resources->offscreen_size.x,
                        resources->offscreen_size.y);
                    if(ImGui::Button("Reload shaders from disk"))
                    {
                        create_shaders(e.underlying());
                    }
                    ImGui::Columns();
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Sound"))
                {
                    SoundPreferences* pref;
                    e.subsystem(pref);
                    ImGui::SliderFloat(
                        "Master volume", &pref->master_volume, 0.f, 1.f);
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void end_restricted(Proxy&, time_point const&)
    {
    }

    // blam::map_container<V> const* m_map{nullptr};

    std::map<std::string_view, bool> m_bsps;
    u32 m_selected_camera{0};
};
