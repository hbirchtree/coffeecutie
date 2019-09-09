#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/comp_app/gl_config.h>

namespace glad {

struct Binding : comp_app::GraphicsBinding, comp_app::AppLoadableService
{
    virtual void load(entity_container&e, comp_app::app_error& ec) final;
};

using Services = comp_app::detail::TypeList<Binding>;

}
