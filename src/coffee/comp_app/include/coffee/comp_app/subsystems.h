#pragma once

#include <coffee/components/components.h>
#include <coffee/components/subsystem.h>
#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <coffee/core/types/display/event.h>

#include "services.h"

namespace comp_app {

using Coffee::Components::Convenience::type_hash_v;

template<typename T>
using AppValue = detail::Globals::ValueSubsystem<T>;

using AppLoaderTag = detail::TagType<libc_types::u32, libc_types::u32>;

struct AppLoader : AppService<
                       AppLoader,
                       detail::TypeList<
                           Windowing,
                           GraphicsContext,
                           GraphicsBinding,
                           EventBus<Coffee::Display::Event>,
                           EventBus<Coffee::Input::CIEvent>,
                           KeyboardInput,
                           MouseInput,
                           ControllerInput,
                           TouchInput>>
{
    using services = Proxy::service_list;

    template<typename T>
    struct service_register
    {
        struct noop
        {
            void operator()(T&, detail::EntityContainer&, app_error&)
            {
            }
        };
        struct load
        {
            void operator()(
                T& service, detail::EntityContainer& e, app_error& ec)
            {
                service.load(e, ec);
            }
        };

        void operator()(detail::EntityContainer& e, app_error& ec)
        {
            using namespace type_safety;

            auto& service = T::template register_service<T>(e);

            using operation = typename std::conditional<
                implementation<T>::template implements<
                    AppLoadableService>::value,
                load,
                noop>::type;

            operation()(service, e, ec);

            C_ERROR_CHECK(ec);
        }
    };

    template<typename Services>
    bool load_all(detail::EntityContainer& e, app_error& ec)
    {
        using namespace type_safety::type_list;

        for_each<Services, service_register>(std::ref(e), std::ref(ec));

        return true;
    }

    template<typename T>
    struct service_loader
    {
        void operator()(Proxy& p, app_error& ec)
        {
            auto ptr = C_DCAST<AppLoadableService>(p.service<T>());

            if(ptr)
                ptr->load(get_container(p), ec);
        }
    };

    bool init(Proxy& p, app_error& ec)
    {
        using namespace type_safety::type_list;

        p.service<Windowing>();

        for_each<services, service_loader>(std::ref(p), std::ref(ec));

        return true;
    }
};

} // namespace comp_app
