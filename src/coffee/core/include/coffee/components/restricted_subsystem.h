#pragma once

#include "types.h"
#include "proxy.h"

namespace Coffee::Components::restricted {

template<class T>
concept is_restricted_subsystem = is_subsystem<T>;

template<class T>
concept is_start_restricted_subsystem = is_restricted_subsystem<T> && requires(T& v)
{
    {v.start_restricted(
        std::declval<typename T::proxy_type&>(), std::declval<time_point>())};
};

template<class T>
concept is_end_restricted_subsystem = is_restricted_subsystem<T> && requires(T& v)
{
    {v.end_restricted(
        std::declval<typename T::proxy_type&>(), std::declval<time_point>())};
};

template<class T>
using proxy_t = ConstrainedProxy<
    typename T::components,
    typename T::subsystems,
    typename T::readable_services>;

template<class T, TEMPLATE_REQUIRES(!is_start_restricted_subsystem<T>, T)>
void start_frame(T&, EntityContainer&, time_point const&)
{
}

template<class T, TEMPLATE_REQUIRES(is_start_restricted_subsystem<T>, T)>
void start_frame(T& subsys, EntityContainer& container, time_point const& t)
{
    auto proxy = proxy_t<T>(container);
    subsys.start_restricted(proxy, t);
}

template<class T, TEMPLATE_REQUIRES(!is_end_restricted_subsystem<T>, T)>
void end_frame(T&, EntityContainer&, time_point const&)
{
}

template<class T, TEMPLATE_REQUIRES(is_end_restricted_subsystem<T>, T)>
void end_frame(T& subsys, EntityContainer& container, time_point const& t)
{
    auto proxy = proxy_t<T>(container);
    subsys.end_restricted(proxy, t);
}

//template<class T, TEMPLATE_REQUIRES(!is_restricted_subsystem<T>, T)>
//void start_frame(T&, EntityContainer&, time_point const&)
//{
//}

//template<class T, TEMPLATE_REQUIRES(!is_restricted_subsystem<T>, T)>
//void end_frame(T&, EntityContainer&, time_point const&)
//{
//}

} // namespace Coffee::Components::restricted
