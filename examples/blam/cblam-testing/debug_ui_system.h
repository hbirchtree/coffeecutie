#pragma once

#include "caching.h"
#include <coffee/imgui/imgui_binding.h>

struct BlamDebugUi;

using BlamDebugUiTag = Components::TaggedTypeWrapper<BlamDebugUi>;

struct BlamDebugUi : Components::RestrictedSubsystem<
                         BlamDebugUiTag,
                         empty_list_t,
                         empty_list_t,
                         empty_list_t>
{
    BlamDebugUi()
    {
    }

    BlamDebugUi& get() override
    {
        return *this;
    }
    BlamDebugUi const& get() const override
    {
        return *this;
    }

    virtual void start_restricted(Proxy&, time_point const&) override
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

        ImGui::DockSpace(ImGui::GetID("center"));

//        ImGui::ShowDemoWindow();
    }
    virtual void end_restricted(Proxy&, time_point const&) override
    {
    }
};
