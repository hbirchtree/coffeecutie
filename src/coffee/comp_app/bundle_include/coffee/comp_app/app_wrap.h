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
    static void addTo(entity_container& e, Args&&... args)
    {
        type::template register_service<AppContainer<DataType>>(
            e, std::forward<Args>(args)...);

        e.register_subsystem_services<AppServiceTraits<AppMain>>(
            e.service<AppContainer<DataType>>());
    }
    static void exec(entity_container&)
    {
    }

    template<typename... Args>
    AppContainer(
        setup_func&& setup,
        loop_func&&  loop    = dummy_loop,
        setup_func&& cleanup = dummy_setup,
        Args&&... args) :
        m_setup(setup),
        m_loop(loop), m_cleanup(cleanup),
        m_data(std::make_shared<DataType>(std::forward<Args>(args)...))
    {
        detail::SubsystemBase::priority = 8192 * 1024;
    }

    virtual void load(entity_container& e, app_error& ec) final
    {
        m_setup(e, *m_data, detail::clock::now());
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
            std::chrono::duration_cast<duration>(t - m_previousTime));
        m_previousTime = t;
    }

  private:
    setup_func m_setup, m_cleanup;
    loop_func  m_loop;
    time_point m_previousTime;

    std::shared_ptr<DataType> m_data;
};

} // namespace comp_app
