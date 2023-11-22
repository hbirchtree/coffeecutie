#if defined(FEATURE_ENABLE_ImGui)
#include "bsp_component.h"
#include "debug_ui_system.h"
#include "input_widget.h"
#include "map_loader.h"
#include "texture_browser_component.h"
#include <coffee/imgui/imgui_binding.h>
#endif

#include "data.h"
#include "selected_version.h"

void install_imgui_widgets(
    compo::EntityContainer& e, std::function<void(Url const&)>&& map_select)
{
    auto& files = e.subsystem_cast<BlamFiles<halo_version>>();

    auto& imgui = e.register_subsystem_inplace<imgui::ImGuiSystem>(1024);
    auto& bsp   = e.register_subsystem_inplace<BlamBspWidget<halo_version>>();
    e.register_subsystem_inplace<BlamDebugUi>();
    auto& textures
        = e.register_subsystem_inplace<BlamTextureBrowser<halo_version>>();
    auto& map_browser
        = e.register_subsystem_inplace<BlamMapBrowser>(std::move(map_select));
    e.register_subsystem_inplace<InputDebugWidget>();

    comp_app::app_error ec;
    imgui.do_load(e, ec);

    auto& gbus = e.subsystem_cast<GameEventBus>();
    gbus.addEventFunction<MapListingEvent>(
        0, [map_browser = &map_browser](GameEvent&, MapListingEvent* listing) {
            map_browser->m_maps = listing->maps;
        });
    gbus.addEventFunction<MapLoadEvent>(0, [&](auto&, auto*) {
        textures.m_map = nullptr;
    });
    gbus.addEventFunction<MapChangedEvent<halo_version>>(
        0, [&](auto&, MapChangedEvent<halo_version>* changed) {
            textures.m_map = &changed->container;
        });

    if(false)
    {
        auto frame_ui = e.create_entity({
            .components = {
                    typeid(imgui::ImGuiWidget).hash_code(),
                },
            });
        frame_ui.get<imgui::ImGuiWidget>() = imgui::widgets::StatsMenu();
    }
}
