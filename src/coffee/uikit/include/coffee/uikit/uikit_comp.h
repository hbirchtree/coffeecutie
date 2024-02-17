#pragma once

#include <coffee/comp_app/services.h>

namespace uikit {

struct GestureInput
    : comp_app::interfaces::GestureInput
    , comp_app::AppService<GestureInput>
    , comp_app::AppLoadableService
{
    virtual void load(entity_container& e, comp_app::app_error& ec) final;
    virtual void start_frame(
        ContainerProxy&, Coffee::Components::time_point const&) final;

  private:
    entity_container* m_container;
};

using Services = comp_app::detail::TypeList<GestureInput>;

} // namespace uikit
