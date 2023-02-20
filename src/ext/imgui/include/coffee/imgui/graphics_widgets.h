#pragma once

#include <coffee/core/types/hardware_info.h>
#include <coffee/core/types/software_info.h>
#include <coffee/imgui/imgui_binding.h>
#include <coffee/interfaces/cgraphics_api.h>
#include <type_traits>

#include <coffee/strings/libc_types.h>

#include <coffee/strings/format.h>

namespace Coffee {
namespace CImGui {

namespace Widgets {

using RendererViewer = Function<void(
    RHI::GraphicsAPI::GraphicsContext const&,
    RHI::GraphicsAPI::GraphicsDevice const&)>;

template<
    typename GFX,
    typename std::enable_if<
        std::is_base_of<RHI::GraphicsAPI, GFX>::value,
        bool>::type* = nullptr>
inline RendererViewer GetRendererViewer()
{
    return [](typename GFX::G_CTXT const& c, typename GFX::G_DEV const& d) {
        using namespace Coffee::Strings;

        auto& io = ImGui::GetIO();

        ImGui::SetNextWindowPos({4, 24});

        ImGui::Begin("Renderer info");

        CString       api_name;
        SWVersionInfo api_version;
        HWDeviceInfo  renderer_info;
        SWVersionInfo driver_info;
        CString       sl_lang_name;
        SWVersionInfo sl_lang_version;

        api_name = GFX::GetAPIName(d);
        GFX::GetAPIVersion(d, &api_version);
        GFX::GetRendererInfo(d, &renderer_info);
        GFX::GetRendererDriverInfo(d, &driver_info);
        sl_lang_name = GFX::GetShaderLanguageName(c);
        GFX::GetShaderLanguageVersion(c, &sl_lang_version);

        CString api_ver_str = cStringFormat(
            "{0}.{1}/{2}",
            api_version.major,
            api_version.minor,
            api_version.build);
        CString renderer_str = cStringFormat(
            "{0} {1}", renderer_info.manufacturer, renderer_info.model);
        CString driver_str      = cStringFormat("{0}", driver_info.name);
        CString sl_lang_ver_str = cStringFormat(
            "{0}.{1}", sl_lang_version.major, sl_lang_version.minor);

        ImGui::Text("API: %s", api_name.c_str());
        ImGui::Text("API version/level: %s", api_ver_str.c_str());

        ImGui::Text("Renderer: %s", renderer_str.c_str());
        ImGui::Text("Renderer driver: %s", driver_str.c_str());

        ImGui::Text("SL name: %s", sl_lang_name.c_str());
        ImGui::Text("SL version: %s", sl_lang_ver_str.c_str());

        ImGui::End();
    };
}

} // namespace Widgets

} // namespace CImGui
} // namespace Coffee
