#pragma once

#include <coffee/components/components.h>
#include <coffee/components/subsystem.h>
#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <coffee/core/types/display/event.h>

#include "services.h"

namespace comp_app {

using Coffee::Components::Convenience::type_hash_v;

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
        void operator()(detail::EntityContainer& e, app_error& ec)
        {
            using namespace type_safety;

            auto& service = T::template register_service<T>(e);

            C_ERROR_CHECK(ec);
        }
    };

    template<typename T>
    struct service_loader
    {
        void operator()(detail::EntityContainer& p, app_error& ec)
        {
            auto ptr = C_DCAST<AppLoadableService>(p.service<T>());

            if(ptr)
                ptr->load(p, ec);

            C_ERROR_CHECK(ec);
        }
    };

    template<typename T>
    struct service_unloader
    {
        void operator()(detail::EntityContainer& p, app_error& ec)
        {
            auto ptr = C_DCAST<AppLoadableService>(p.service<T>());

            if(ptr)
                ptr->unload(p, ec);

            C_ERROR_CHECK(ec);
        }
    };

    template<typename Services>
    void loadAll(detail::EntityContainer& e, app_error& ec)
    {
        using namespace type_safety::type_list;

        for_each<Services, service_register>(std::ref(e), std::ref(ec));
        for_each<Services, service_loader>(std::ref(e), std::ref(ec));
    }

    void clearConfigs()
    {
        m_configs.clear();
        m_configStore.clear();
    }

    template<typename Services>
    void unloadAll(detail::EntityContainer& e, app_error& ec)
    {
        using namespace type_safety::type_list;

        for_each_rev<Services, service_unloader>(std::ref(e), std::ref(ec));
    }

    template<class Config>
    void addConfig(stl_types::UqPtr<Config>&& ptr)
    {
        auto const type_id = type_hash_v<Config>();

        m_configStore.emplace_back(std::move(ptr));
        m_configs.insert({type_id, m_configStore.back().get()});
    }

    template<typename T>
    struct config_adder
    {
        void operator()(AppLoader& ldr)
        {
            ldr.addConfig(stl_types::MkUq<T>());
        }
    };

    template<typename Configs>
    void addConfigs()
    {
        using namespace type_safety::type_list;

        for_each<Configs, config_adder>(std::ref(*this));
    }

    template<class Config>
    Config& config()
    {
        auto const type_id = type_hash_v<Config>();

        auto it = m_configs.find(type_id);

        if(it == m_configs.end())
            Throw(undefined_behavior("failed to find config"));

        auto ptr = C_DCAST<Config>(it->second);

        if(!ptr)
            Throw(undefined_behavior("config cast failed"));

        return *ptr;
    }

    stl_types::Vector<stl_types::UqPtr<detail::SubsystemBase>> m_configStore;
    stl_types::Map<detail::type_hash, detail::SubsystemBase*> m_configs;

    template<class Config>
    static Config& config(detail::EntityContainer& container)
    {
        return container.service<AppLoader>()->config<Config>();
    }
};

} // namespace comp_app
