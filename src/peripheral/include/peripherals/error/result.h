#pragma once

//#include <boost/outcome/std_result.hpp>
#include <outcome/result.hpp>

namespace stl_types {

//template<typename T, typename E>
//using result = boost::outcome_v2::std_checked<T, E>;
//using boost::outcome_v2::success;
//using boost::outcome_v2::failure;

template<typename T, typename E>
using result = outcome_v2::checked<T, E>;
using outcome_v2::success;
using outcome_v2::failure;

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

}
