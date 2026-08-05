#pragma once

#include "proxy.h"
#include "types.h"

namespace compo::restricted {

template<class T>
concept is_restricted_subsystem = is_subsystem<T>;

template<class T>
concept is_start_restricted_subsystem =
    is_restricted_subsystem<T> && requires(T& v) {
        {
            v.start_restricted(
                std::declval<typename T::proxy_type&>(),
                std::declval<time_point>())
        };
    };

template<class T>
concept is_end_restricted_subsystem = is_restricted_subsystem<T> && requires(
                                                                        T& v) {
    {
        v.end_restricted(
            std::declval<typename T::proxy_type&>(), std::declval<time_point>())
    };
};

template<class T>
using proxy_t = ConstrainedProxy<
    typename T::components,
    typename T::subsystems,
    typename T::readable_services>;

template<class T>
struct readable_services_of_s
{
    using type = type_safety::empty_list_t;
};

template<class T>
requires requires { typename T::readable_services; }
struct readable_services_of_s<T>
{
    using type = typename T::readable_services;
};

template<class T>
using readable_services_of = typename readable_services_of_s<T>::type;

template<class T>
requires(!is_start_restricted_subsystem<T>)
void start_frame(T& subsys, EntityContainer&, time_point const&)
{
    subsys.no_start_hook = true;
}

template<class T>
requires is_start_restricted_subsystem<T>
void start_frame(T& subsys, EntityContainer& container, time_point const& t)
{
    access::scope_guard _(subsys.runtime_access);
    auto                proxy = proxy_t<T>(container);
    subsys.start_restricted(proxy, t);
}

template<class T>
requires(!is_end_restricted_subsystem<T>)
void end_frame(T& subsys, EntityContainer&, time_point const&)
{
    subsys.no_end_hook = true;
}

template<class T>
requires is_end_restricted_subsystem<T>
void end_frame(T& subsys, EntityContainer& container, time_point const& t)
{
    access::scope_guard _(subsys.runtime_access);
    auto                proxy = proxy_t<T>(container);
    subsys.end_restricted(proxy, t);
}

} // namespace compo::restricted
