#pragma once

#include <coffee/comp_app/services.h>

namespace glad {

struct Binding : comp_app::AppService<Binding, comp_app::GraphicsBinding>,
                 comp_app::interfaces::GraphicsBinding,
                 comp_app::AppLoadableService
{
    Binding()
    {
        priority = 600;
    }

    virtual void load(entity_container& e, comp_app::app_error& ec) final;
};

using Services = comp_app::detail::TypeList<Binding>;

} // namespace glad
