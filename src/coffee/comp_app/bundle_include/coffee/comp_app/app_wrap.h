#pragma once

#include <coffee/comp_app/app_main.h>

#include "bundle.h"

namespace comp_app {

template<typename DataType>
struct AppContainer : AppService<AppContainer<DataType>>, AppMain
{
    using type       = AppContainer<DataType>;
    using duration   = typename type::duration;
    using time_point = typename type::time_point;

    using setup_func = stl_types::Function<void(
        entity_container&, DataType&, time_point const&)>;
    using loop_func  = stl_types::Function<void(
        entity_container&, DataType&, time_point const&, duration const&)>;

    static void dummy_setup(entity_container&, DataType&, time_point const&)
    {
    }
    static void dummy_loop(
        entity_container&, DataType&, time_point const&, duration const&)
    {
    }

    template<typename... Args>
    static void addTo(entity_container& e, Args... args)
    {
        type::template register_service<AppContainer<DataType>>(e, args...);

        e.register_subsystem_services<AppServiceTraits<AppMain>>(
            e.service<AppContainer<DataType>>());
    }
    static void exec(entity_container& e)
    {
    }

    AppContainer(
        setup_func&& setup,
        loop_func&&  loop    = dummy_loop,
        setup_func&& cleanup = dummy_setup) :
        m_setup(setup),
        m_loop(loop), m_cleanup(cleanup),
        m_data(stl_types::MkShared<DataType>())
    {
    }

    virtual void load(entity_container& e, app_error& ec) final
    {
        m_setup(e, *m_data, detail::clock::now());
        detail::SubsystemBase::priority = 8192 * 1024;
    }
    virtual void unload(entity_container& e, app_error& ec) final
    {
        m_cleanup(e, *m_data, detail::clock::now());
    }
    virtual void start_frame(
        detail::ContainerProxy& p, time_point const& t) final
    {
        m_loop(
            type::get_container(p),
            *m_data,
            t,
            stl_types::Chrono::duration_cast<duration>(t - m_previousTime));
        m_previousTime = t;
    }

  private:
    setup_func m_setup, m_cleanup;
    loop_func  m_loop;
    time_point m_previousTime;

    stl_types::ShPtr<DataType> m_data;
};

} // namespace comp_app
