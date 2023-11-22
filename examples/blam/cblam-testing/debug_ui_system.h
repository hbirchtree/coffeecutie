#pragma once

#include "caching.h"
#include <coffee/imgui/imgui_binding.h>

struct BlamDebugUi;

using BlamDebugUiManifest = compo::
    SubsystemManifest<empty_list_t, type_list_t<LoadingStatus>, empty_list_t>;

struct BlamDebugUi
    : compo::RestrictedSubsystem<BlamDebugUi, BlamDebugUiManifest>
{
    using type = BlamDebugUi;

    BlamDebugUi()
    {
        compo::SubsystemBase::priority = 2048;
    }

    void start_restricted(Proxy& e, time_point const& current_time)
    {
        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("Scripts"))
            {
                ImGui::MenuItem("Bytecode", nullptr, true);

                ImGui::EndMenu();
            }

            ImGui::Columns(5);

            framerate_text(current_time);
            ImGui::NextColumn();

            status_text(e);
            ImGui::NextColumn();

            ImGui::Columns();

            ImGui::EndMainMenuBar();
        }

        //        ImGui::DockSpace(ImGui::GetID("center"));

        //        ImGui::ShowDemoWindow();
    }
    void end_restricted(Proxy&, time_point const&)
    {
    }

    void framerate_text(time_point const& time)
    {
        using namespace std::chrono_literals;

        const auto delta = time - last_time;
        const auto delta_f
            = std::chrono::duration_cast<stl_types::Chrono::seconds_float>(
                  delta)
                  .count();
        frametimes.push_back(delta_f);

        if(frametimes.size() > 300)
            frametimes.erase(frametimes.begin());

        ImGui::PlotHistogram(
            "Frametime graph",
            frametimes.data(),
            frametimes.size(),
            0,
            nullptr,
            0,
            1,
            {100, 24});

        ImVec4 perf_color;

// clang-format off
        if(delta > 66ms)
            perf_color = {1,   0,   0, 1};
        else if(delta > 33ms)
            perf_color = {1,   0.5, 0, 1};
        else if(delta > 25ms)
            perf_color = {1,   1,   0, 1};
        else if(delta > 20ms)
            perf_color = {0.5, 1,   0, 1};
        else
            perf_color = {0,   1,   0, 1};
// clang-format on

        ImGui::TextColored(perf_color, "ms=%f", delta_f);
        last_time = time;
    }

    void status_text(Proxy& e)
    {
        LoadingStatus* loading;
        e.subsystem(loading);

        if(loading->progress < 0)
            return;

        ImGui::ProgressBar(loading->progress / 100.f, ImVec2{200, 24});
        ImGui::Text("Loading: %s", loading->status.c_str());
    }

    time_point       last_time{};
    std::vector<f32> frametimes;
};
