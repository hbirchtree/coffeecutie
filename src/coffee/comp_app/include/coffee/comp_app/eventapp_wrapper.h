#pragma once

#include <platforms/pimpl_state.h>

#include "app_main.h"
#include "gl_config.h"
#include "subsystems.h"

namespace comp_app {

template<typename R, typename D>
struct EventapplicationWrapper
    : AppService<EventapplicationWrapper<R, D>>
    , AppMain
{
    using type = EventapplicationWrapper<R, D>;

    using loop_fun     = stl_types::Function<void(R&, D*)>;
    using service_type = AppService<type>;

    using readable_services = type_safety::empty_list_t;
    using proxy_type        = detail::restricted::proxy_t<type>;

    struct EventConfig : comp_app::Config<EventConfig>
    {
        EventConfig()
        {
        }

        EventConfig(
            std::shared_ptr<R>& r,
            std::shared_ptr<D>& d,
            loop_fun&&          s,
            loop_fun&&          l,
            loop_fun&&          c)
            : m_renderer(r)
            , m_data(d)
            , m_setup(std::move(s))
            , m_loop(std::move(l))
            , m_cleanup(std::move(c))
        {
        }

        std::shared_ptr<R> m_renderer;
        std::shared_ptr<D> m_data;
        loop_fun           m_setup, m_loop, m_cleanup;
    };

    EventapplicationWrapper()
        : m_loaded(false)
    {
    }

    virtual void load(entity_container& e, app_error& /*ec*/) final
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

    virtual void unload(entity_container& /*e*/, app_error& /*ec*/) final
    {
        m_config.m_cleanup(*m_config.m_renderer, m_config.m_data.get());
        m_loaded = false;
    }

    virtual void start_restricted(
        proxy_type&, typename service_type::time_point const&) final;

    EventConfig m_config;
    bool        m_loaded;
};

template<typename R, typename D>
void EventapplicationWrapper<R, D>::start_restricted(
    proxy_type&, const typename service_type::time_point&)
{
    if(!m_loaded)
        return;

    m_config.m_loop(*m_config.m_renderer, m_config.m_data.get());
}

namespace detail {

struct DummyData
{
    static constexpr void (*EmscriptenLoop)() = nullptr;
};

template<
    typename R,
    typename std::enable_if<std::is_class<R>::value>::type* = nullptr>
static void addContainer(std::shared_ptr<R>& ptr, detail::EntityContainer& cnt)
{
    ptr->m_container = &cnt;
}

template<
    typename R,
    typename std::enable_if<!std::is_class<R>::value>::type* = nullptr>
static void addContainer(std::shared_ptr<R>&, detail::EntityContainer&)
{
}

} // namespace detail
} // namespace comp_app
