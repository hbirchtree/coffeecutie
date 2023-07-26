#pragma once

#include <coffee/components/components.h>
#include <coffee/core/libc_types.h>
#include <peripherals/stl/types.h>
#include <coffee/core/types/display/event.h>

#include "services.h"

namespace comp_app {

using compo::convenience::type_hash_v;

using AppLoaderTag = detail::TagType<libc_types::u32, libc_types::u32>;

struct AppLoader : AppService<AppLoader>
{
    using readable_services = detail::subsystem_list<
        Windowing,
        GraphicsContext,
        GraphicsBinding,
        EventBus<Coffee::Display::Event>,
        EventBus<Coffee::Input::CIEvent>,
        KeyboardInput,
        MouseInput,
        ControllerInput,
        TouchInput>;
    using type = AppLoader;

    using proxy_type = detail::restricted::proxy_t<AppLoader>;

    struct service_register
    {
        service_register(detail::EntityContainer& e, app_error& ec) :
            e(e), ec(ec)
        {
        }

        template<typename T>
        void operator()()
        {
            using namespace type_safety;

            if(e.template service<T>())
                return;

            T::template register_service<T>(e);

            C_ERROR_CHECK(ec);
        }

        detail::EntityContainer& e;
        app_error&               ec;
    };

    struct service_loader
    {
        service_loader(detail::EntityContainer& e, app_error& ec) : e(e), ec(ec)
        {
        }
        template<detail::is_subsystem T>
        void operator()()
        {
            auto ptr
                = C_DCAST<AppLoadableService>(e.service<typename T::type>());

            if(ptr)
                ptr->do_load(e, ec);

            C_ERROR_CHECK(ec);
        }

        detail::EntityContainer& e;
        app_error&               ec;
    };

    struct service_unloader
    {
        service_unloader(detail::EntityContainer& e, app_error& ec) :
            e(e), ec(ec)
        {
        }
        template<typename T>
        void operator()()
        {
            auto ptr = C_DCAST<AppLoadableService>(e.service<T>());

            if(ptr)
                ptr->do_unload(e, ec);

            C_ERROR_CHECK(ec);
        }

        detail::EntityContainer& e;
        app_error&               ec;
    };

    template<typename Services>
    void registerAll(detail::EntityContainer& e, app_error& ec)
    {
        using namespace type_safety::type_list;

        ec = AppError::None;

        for_each<Services>(service_register(e, ec));
    }

    template<typename Services>
    void loadAll(detail::EntityContainer& e, app_error& ec)
    {
        using namespace type_safety::type_list;

        ec = AppError::None;

        for_each<Services>(service_register(e, ec));
        for_each<Services>(service_loader(e, ec));
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

        for_each_rev<Services>(service_unloader(e, ec));
    }

    template<class Config>
    Config& addConfig(std::unique_ptr<Config>&& ptr)
    {
        auto const type_id = type_hash_v<Config>();

        m_configStore.emplace_back(std::move(ptr));
        m_configs.insert({type_id, m_configStore.back().get()});

        return *C_DCAST<Config>(m_configStore.back().get());
    }

    struct config_adder
    {
        config_adder(AppLoader& ldr) : ldr(ldr)
        {
        }
        template<typename T>
        void operator()()
        {
            ldr.addConfig(std::make_unique<T>());
        }

        AppLoader& ldr;
    };

    template<typename Configs>
    void addConfigs()
    {
        using namespace type_safety::type_list;

        for_each<Configs>(config_adder(*this));
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

    std::vector<std::unique_ptr<detail::SubsystemBase>> m_configStore;
    std::map<detail::type_hash, detail::SubsystemBase*>  m_configs;

    template<class Config>
    static Config& config(detail::EntityContainer& container)
    {
        return container.service<AppLoader>()->config<Config>();
    }

    static AppLoader& register_service(detail::EntityContainer& container)
    {
        return AppService<AppLoader>::register_service<AppLoader>(container);
    }
};

} // namespace comp_app
