#pragma once

#define C_FC(t, v) static_cast<t>(v)

#define C_FLAGS(e, t)                                      \
                                                           \
    constexpr inline e operator~(e val)                    \
    {                                                      \
        return C_FC(e, ~(C_FC(t, val)));                   \
    }                                                      \
                                                           \
    constexpr inline e operator^(e lhs, e rhs)             \
    {                                                      \
        return C_FC(e, C_FC(t, lhs) ^ C_FC(t, rhs));       \
    }                                                      \
    constexpr inline e operator&(e lhs, e rhs)             \
    {                                                      \
        return C_FC(e, C_FC(t, lhs) & C_FC(t, rhs));       \
    }                                                      \
    constexpr inline e operator|(e lhs, e rhs)             \
    {                                                      \
        return C_FC(e, C_FC(t, lhs) | C_FC(t, rhs));       \
    }                                                      \
                                                           \
    inline e operator^=(e& lhs, e rhs)                     \
    {                                                      \
        return lhs = C_FC(e, C_FC(t, lhs) ^ C_FC(t, rhs)); \
    }                                                      \
    inline e operator&=(e& lhs, e rhs)                     \
    {                                                      \
        return lhs = C_FC(e, C_FC(t, lhs) & C_FC(t, rhs)); \
    }                                                      \
    inline e operator|=(e& lhs, e rhs)                     \
    {                                                      \
        return lhs = C_FC(e, C_FC(t, lhs) | C_FC(t, rhs)); \
    }

namespace enum_helpers {

template<typename T>
/*!
 * \brief Cast enum type to boolean
 * \param flagcombo
 * \return
 */
inline bool feval(T flagcombo)
{
    return static_cast<int>(flagcombo);
}

template<typename T>
constexpr inline bool feval(T flags, T target)
{
    return (flags & target) == target;
}

template<typename T, T v1, T v2>
struct or_equals
{
    static constexpr bool value = ((v1 & v2) == v2);
};

template<typename T, T v1, T v2, T v3>
struct or_equals_except
{
    static constexpr bool value = ((v1 & v2) == v2) && (v1 & v3) == 0;
};

} // namespace enum_helpers

namespace Coffee {

using namespace ::enum_helpers;
}
