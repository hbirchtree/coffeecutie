#pragma once

#include <coffee/comp_app/app_main.h>

#include "bundle.h"

namespace comp_app {

template<typename DataType>
struct AppContainer : AppService<AppContainer<DataType>>, AppMain
{
    using type = AppContainer<DataType>;

    using service_type = AppService<AppContainer<DataType>>;
    using Proxy        = typename service_type::Proxy;
    using time_point   = typename service_type::time_point;

    using setup_func = stl_types::Function<void(entity_container&, DataType&)>;
    using loop_func  = stl_types::Function<void(
        entity_container&, DataType&, time_point const&)>;

    template<typename... Args>
    static void addTo(entity_container& e, Args... args)
    {
        service_type::template register_service<AppContainer<DataType>>(
            e, args...);

        e.register_subsystem_services<AppServiceTraits<AppMain>>(
            e.service<AppContainer<DataType>>());
    }

    static void exec(entity_container& e)
    {
    }

    AppContainer(setup_func&& setup, loop_func&& loop, setup_func&& cleanup) :
        m_setup(setup), m_loop(loop), m_cleanup(cleanup),
        m_data(stl_types::MkShared<DataType>())
    {
    }

    virtual void load(entity_container& e, app_error& ec) final
    {
        m_setup(e, *m_data);
    }
    virtual void unload(entity_container& e, app_error& ec) final
    {
        m_cleanup(e, *m_data);
    }
    virtual void start_restricted(Proxy& p, time_point const& t) final
    {
        m_loop(service_type::get_container(p), *m_data, t);
    }

  private:
    setup_func m_setup, m_cleanup;
    loop_func  m_loop;

    stl_types::ShPtr<DataType> m_data;
};

} // namespace comp_app
