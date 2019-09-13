#pragma once

#include <coffee/core/task_queue/task.h>

#include "subsystems.h"

namespace comp_app {

struct EventMain : AppLoadableService
{
    using type = EventMain;
};

template<typename R, typename D>
struct EventapplicationWrapper : AppService<EventapplicationWrapper<R, D>>,
                                 EventMain
{
    using type = EventapplicationWrapper<R, D>;

    using loop_fun     = stl_types::Function<void(R&, D*)>;
    using service_type = AppService<EventapplicationWrapper<R, D>>;

    struct EventConfig : comp_app::Config<EventConfig>
    {
        EventConfig()
        {
        }
        EventConfig(
            stl_types::UqPtr<R>&& r,
            stl_types::UqPtr<D>&& d,
            loop_fun&&            s,
            loop_fun&&            l,
            loop_fun&&            c) :
            m_renderer(std::move(r)),
            m_data(std::move(d)), m_setup(std::move(s)), m_loop(std::move(l)),
            m_cleanup(std::move(c))
        {
        }

        stl_types::UqPtr<R> m_renderer;
        stl_types::UqPtr<D> m_data;
        loop_fun            m_setup, m_loop, m_cleanup;
    };

    virtual void load(entity_container& e, app_error& ec) final
    {
        auto& config = AppLoader::config<EventConfig>(e);

        m_config.m_data     = std::move(config.m_data);
        m_config.m_renderer = std::move(config.m_renderer);
        m_config.m_setup    = std::move(config.m_setup);
        m_config.m_loop     = std::move(config.m_loop);
        m_config.m_cleanup  = std::move(config.m_cleanup);

        m_config.m_setup(*m_config.m_renderer, m_config.m_data.get());
    }
    virtual void unload(entity_container& e, app_error& ec) final
    {
        m_config.m_cleanup(*m_config.m_renderer, m_config.m_data.get());
    }
    virtual void start_restricted(
        typename service_type::Proxy&,
        typename service_type::time_point const&) final
    {
        m_config.m_loop(*m_config.m_renderer, m_config.m_data.get());
    }

    EventConfig m_config;
};

template<typename R, typename D>
struct AutoExec
{
    using presetup_fun = stl_types::Function<void(R&, D*)>;
    using loop_fun     = stl_types::Function<void(R&, D*)>;

    static void addTo(
        detail::EntityContainer& container,
        presetup_fun&&           presetup,
        loop_fun&&               setup,
        loop_fun&&               loop,
        loop_fun&&               cleanup)
    {
        using EventAppType = EventapplicationWrapper<R, D>;

        auto app      = container.service<AppLoader>();
        auto renderer = stl_types::MkUq<R>();
        auto data     = stl_types::MkUq<D>();

        presetup(*renderer, data.get());

        app->addConfig(stl_types::MkUq<typename EventAppType::EventConfig>(
            std::move(renderer),
            std::move(data),
            std::move(setup),
            std::move(loop),
            std::move(cleanup)));

        app_error ec;
        app->loadAll<detail::TypeList<EventAppType>>(container, ec);
        auto service = container.service<EventAppType>();
        container.register_subsystem_services<AppServiceTraits<EventMain>>(
            service);
    }

    static int exec(detail::EntityContainer& container)
    {
#if !defined(COFFEE_CUSTOM_EXIT_HANDLING)
        Coffee::runtime_queue_error ec;
        auto queue = Coffee::RuntimeQueue::GetCurrentQueue(ec);
        C_ERROR_CHECK(ec);

        while(!container.service<Windowing>()->notifiedClose())
        {
            container.exec();

            if(queue)
                queue->executeTasks();
        }

            /* TODO: Unload all services */
#endif
        return 0;
    }
};

} // namespace comp_app