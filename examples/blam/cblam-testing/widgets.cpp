#if defined(FEATURE_ENABLE_ImGui)
#include "bsp_component.h"
#include "debug_ui_system.h"
#include "input_widget.h"
#include "map_loader.h"
#include "script_component.h"
#include "texture_browser_component.h"
#include <coffee/imgui/imgui_binding.h>
#endif

#include "data.h"
#include "selected_version.h"

void install_imgui_widgets(
    compo::EntityContainer&           e,
    BlamData<halo_version>&           data,
    std::function<void(Url const&)>&& map_select)
{
    auto& imgui = e.register_subsystem_inplace<imgui::ImGuiSystem>(1024);
    e.register_subsystem_inplace<BlamBspWidget<halo_version>>(&data);
    e.register_subsystem_inplace<BlamDebugUi>();
    e.register_subsystem_inplace<BlamTextureBrowser<halo_version>>(&data);
    e.register_subsystem_inplace<BlamMapBrowser>(std::move(map_select));
    e.register_subsystem_inplace<InputDebugWidget>();

    {
        comp_app::app_error ec;
        imgui.do_load(e, ec);
        auto frame_ui = e.create_entity({
            .components = {
                    typeid(imgui::ImGuiWidget).hash_code(),
                },
            });
        frame_ui.get<imgui::ImGuiWidget>() = imgui::widgets::StatsMenu();
    }
}
