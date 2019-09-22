#pragma once

#include <coffee/core/types/display/properties.h>
#include <platforms/pimpl_state.h>

#include "app_main.h"
#include "gl_config.h"
#include "subsystems.h"

namespace comp_app {

template<typename R, typename D>
struct EventapplicationWrapper : AppService<EventapplicationWrapper<R, D>>,
                                 AppMain
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
            stl_types::ShPtr<R>& r,
            stl_types::ShPtr<D>& d,
            loop_fun&&           s,
            loop_fun&&           l,
            loop_fun&&           c) :
            m_renderer(r),
            m_data(d), m_setup(std::move(s)), m_loop(std::move(l)),
            m_cleanup(std::move(c))
        {
        }

        stl_types::ShPtr<R> m_renderer;
        stl_types::ShPtr<D> m_data;
        loop_fun            m_setup, m_loop, m_cleanup;
    };

    EventapplicationWrapper() : m_loaded(false)
    {
    }

    virtual void load(entity_container& e, app_error& ec) final
    {
        auto& config = AppLoader::config<EventConfig>(e);

        m_config.m_data     = config.m_data;
        m_config.m_renderer = config.m_renderer;
        m_config.m_setup    = std::move(config.m_setup);
        m_config.m_loop     = std::move(config.m_loop);
        m_config.m_cleanup  = std::move(config.m_cleanup);

        m_config.m_setup(*m_config.m_renderer, m_config.m_data.get());
        m_loaded = true;
    }
    virtual void unload(entity_container& e, app_error& ec) final
    {
        m_config.m_cleanup(*m_config.m_renderer, m_config.m_data.get());
        m_loaded = false;
    }
    virtual void start_restricted(
        typename service_type::Proxy&,
        typename service_type::time_point const&) final
    {
        if(!m_loaded)
            return;

        m_config.m_loop(*m_config.m_renderer, m_config.m_data.get());
    }

    EventConfig m_config;
    bool        m_loaded;
};

namespace detail {

struct DummyData
{
    static constexpr void (*EmscriptenLoop)() = nullptr;
};

template<
    typename R,
    typename std::enable_if<std::is_class<R>::value>::type* = nullptr>
static void addContainer(stl_types::ShPtr<R>& ptr, detail::EntityContainer& cnt)
{
    ptr->m_container = &cnt;
}
template<
    typename R,
    typename std::enable_if<!std::is_class<R>::value>::type* = nullptr>
static void addContainer(stl_types::ShPtr<R>&, detail::EntityContainer&)
{
}

} // namespace detail

template<typename D = int, typename R = int>
struct AutoExecEx
{
    using presetup_fun = stl_types::Function<void(
        stl_types::ShPtr<R>,
        stl_types::ShPtr<D>,
        Coffee::Display::Properties&)>;
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
        auto renderer = stl_types::MkShared<R>();
        auto data     = stl_types::MkShared<D>();

        auto& config =
            app->addConfig(stl_types::MkUq<typename EventAppType::EventConfig>(
                std::ref(renderer),
                std::ref(data),
                std::move(setup),
                std::move(loop),
                std::move(cleanup)));

        EventAppType::template register_service<EventAppType>(container);

        auto& windowConfig = app->config<WindowConfig>();
        auto& glConfig = app->config<GLConfig>();

        Coffee::Display::Properties props;

        props.title = windowConfig.title.c_str();
        props.size = windowConfig.size;
        props.gl.version.major = glConfig.version.major;
        props.gl.version.minor = glConfig.version.minor;

        detail::addContainer<R>(renderer, container);
        presetup(config.m_renderer, config.m_data, props);

        windowConfig.title = props.title;
        windowConfig.size = props.size;
        glConfig.version.major = props.gl.version.major;
        glConfig.version.minor = props.gl.version.minor;

        auto service = container.service<EventAppType>();
        container.register_subsystem_services<AppServiceTraits<AppMain>>(
            service);
    }

    static void addTo(
        detail::EntityContainer& container,
        loop_fun&&               setup,
        loop_fun&&               loop,
        loop_fun&&               cleanup)
    {
        addTo(
            std::ref(container),
            [](stl_types::ShPtr<R>,
               stl_types::ShPtr<D>,
               Coffee::Display::Properties&) {},
            std::move(setup),
            std::move(loop),
            std::move(cleanup));
    }
};

template<typename R, typename D>
using AutoExec = AutoExecEx<D, R>;

} // namespace comp_app
