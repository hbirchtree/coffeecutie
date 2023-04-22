#include <coffee/imgui/imgui_binding.h>

namespace imgui::widgets {

using compo::duration;
using compo::EntityContainer;
using compo::time_point;
using libc_types::f32;
using libc_types::szptr;
using std::chrono::duration_cast;
using stl_types::Chrono::seconds_float;

ImGuiWidget StatsMenu()
{
    return {
        .name = "framerate stats",
        .func =
            [m_values = std::vector<f32>(), m_index = szptr(0)](
                EntityContainer&,
                time_point const&,
                duration const& delta) mutable {
                m_values.resize(50);

                const auto delta_ms
                    = duration_cast<seconds_float>(delta).count();

                ImGui::BeginMainMenuBar();
                ImGui::Columns(4);

                ImGui::TextColored({1, 1, 1, 1}, "ms=%f", delta_ms);

                ImGui::NextColumn();

                ImGui::NextColumn();

                ImGui::NextColumn();

                m_values.at(m_index) = delta_ms;
                ImGui::PlotHistogram(
                    "",
                    m_values.data(),
                    m_values.size(),
                    0,
                    "",
                    0.f,
                    0.1f,
                    {100, 24});
                ++m_index;
                m_index %= m_values.size();

                ImGui::NextColumn();
                ImGui::EndMainMenuBar();
            },
    };
}

} // namespace imgui::widgets
