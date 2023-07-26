#pragma once

#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/identify/compiler/variable_attributes.h>
#include <peripherals/identify/quirks.h>
#include <peripherals/identify/system.h>
#include <peripherals/stl/constructors.h>
#include <peripherals/stl/type_safety.h>

/*Container types*/
#include <array>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

/*Memory management*/
#include <memory>

/*Shit*/
#include <exception>
#include <system_error>
#include <type_traits>

#include <tinyutf8/tinyutf8.h>

#include <optional>

namespace stl_types {

using namespace type_safety;

using u8string  = tiny_utf8::utf8_string;

template<typename T, class Deleter, typename... Args>
inline std::unique_ptr<T, Deleter> make_unique_with_destructor(Args... a)
{
    return std::unique_ptr<T, Deleter>(new T(std::forward<Args>(a)...));
}

/*
 *
 * Here comes a couple of custom classes and wrappers
 *
 */

template<typename T, typename... Items>
requires (std::is_same_v<T, Items> && ...)
inline constexpr auto make_array(Items&&... items)
{
    return std::array<T, sizeof...(Items)>{{std::move(items)...}};
}

struct non_copy
{
    non_copy() = default;
    C_MOVE_CONSTRUCTOR(non_copy);
    C_DELETE_COPY_CONSTRUCTOR(non_copy);
};

} // namespace stl_types

namespace type_safety {

template<typename D, typename T>
FORCEDINLINE typename std::shared_ptr<D> C_DCAST(
    typename std::shared_ptr<T> const& from)
{
    return std::dynamic_pointer_cast<D>(from);
}

template<typename D, typename T>
FORCEDINLINE typename std::shared_ptr<D> C_PCAST(
    typename std::shared_ptr<T> const& from)
{
    return std::static_pointer_cast<D>(from);
}

} // namespace type_safety

using stl_types::non_copy;
