#pragma once

#if __has_include(<boost/outcome/std_result.hpp>)
#include <boost/outcome/std_result.hpp>
#define USE_BOOST_OUTCOME 1
#else
#include <outcome/result.hpp>
#define USE_BOOST_OUTCOME 0
#endif

namespace stl_types {

#if USE_BOOST_OUTCOME == 1
template<typename T, typename E>
using result = boost::outcome_v2::std_checked<T, E>;
using boost::outcome_v2::failure;
using boost::outcome_v2::success;
#else
template<typename T, typename E>
using result = outcome_v2::checked<T, E>;
using outcome_v2::failure;
using outcome_v2::success;
#endif

template<typename T, typename E>
inline T value_or(result<T, E>&& res, T&& v)
{
    if(res.has_value())
        return res.value();
    return v;
}

template<typename T, typename E>
inline T value_or(result<T, E>&& res, T const& v)
{
    if(res.has_value())
        return res.value();
    return v;
}

} // namespace stl_types
