#pragma once

#include "caching.h"
#include <coffee/imgui/imgui_binding.h>

struct BlamDebugUi;

using BlamDebugUiManifest
    = Components::SubsystemManifest<empty_list_t, empty_list_t, empty_list_t>;

struct BlamDebugUi
    : Components::RestrictedSubsystem<BlamDebugUi, BlamDebugUiManifest>
{
    using type = BlamDebugUi;

    BlamDebugUi()
    {
        compo::SubsystemBase::priority = 2048;
    }

    void start_restricted(Proxy&, time_point const&)
    {
        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("Scripts"))
            {
                ImGui::MenuItem("Bytecode", nullptr, true);

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

//        ImGui::DockSpace(ImGui::GetID("center"));

        //        ImGui::ShowDemoWindow();
    }
    void end_restricted(Proxy&, time_point const&)
    {
    }
};
