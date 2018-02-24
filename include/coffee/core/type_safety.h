#pragma once

#include "plat/plat_quirks_toggling.h"

#include <type_traits>
#include <limits>
#include <stdint.h>

#if defined(COFFEE_MACRO_CASTS)
#define C_CAST(type, var) static_cast<type>(var)
#define C_DCAST(type, var) dynamic_cast<type>(var)
#else

#define IS_SIGNED(Type) std::is_signed<D>::value
#define IS_USIGNED(Type) std::is_unsigned<D>::value

#define IS_INT(Type) std::is_integral<Type>::value
#define IS_PTR(Type) std::is_pointer<Type>::value
#define RP(Type) typename std::remove_pointer<Type>::type
#define IS_POD(Type) (!std::is_pointer<Type>::value)
#define IS_CONST(Type) std::is_const<Type>::value

/*! \brief If you want to know why this is done, it's because Visual C++ is broken.
 *  Too broken for me to care.
 */
#if !defined(COFFEE_WINDOWS)

template<typename D,
         typename T,
         typename std::enable_if<IS_INT(D) && IS_INT(T)>::type* = nullptr,
         typename std::enable_if<
    (std::numeric_limits<T>::max() <= std::numeric_limits<D>::max()) &&
    (std::numeric_limits<T>::min() >= std::numeric_limits<D>::min())
            >::type* = nullptr,
         typename std::enable_if<
             IS_SIGNED(D) == IS_SIGNED(T)>::type* = nullptr>
/*!
 * \brief The case where all casting is safe, no downcasting
 *  or signed-unsigned conversion
 * \param from
 * \return
 */
static inline D C_FCAST(T from)
{
    return static_cast<D>(from);
}

template<typename D,
         typename T,
         typename std::enable_if<IS_INT(D) && IS_INT(T)>::type* = nullptr,
         typename std::enable_if<
    (std::numeric_limits<T>::max() > std::numeric_limits<D>::max()) ||
    (std::numeric_limits<T>::min() < std::numeric_limits<D>::min())
             >::type* = nullptr>
/*!
 * \brief If there is a risk of going out of the
 *  range of type D, return the closest value.
 * \param from
 * \return
 */
static inline D C_FCAST(T from)
{
    static constexpr D max_val = std::numeric_limits<D>::max();
    static constexpr D min_val = std::numeric_limits<D>::min();

    static constexpr T T_max_val = std::numeric_limits<T>::max();
//    static constexpr T T_min_val = std::numeric_limits<T>::min();

    static constexpr bool T_is_signed = std::is_signed<T>::value;
    static constexpr bool D_is_signed = std::is_signed<D>::value;
    static constexpr bool same_sign = T_is_signed == D_is_signed;

    const ::uint64_t T_max_val_u64 = static_cast<::uint64_t>(T_max_val);
    const ::uint64_t D_max_val_u64 = static_cast<::uint64_t>(max_val);

    if(same_sign)
    {
        /* This part is quite simple, because comparisons
         *  with same sign is well-defined */
        if(from < min_val)
            return min_val;
        else if(from > max_val)
            return max_val;
        else
            return from;
    }else{
        /* Comparisons with different sign is literally iffy */
        if(from < 0)
            /* If `from' is signed and below 0,
             *  we know that `D' must be unsigned.
             *  We can only return the smallest value for `D'. */
            return min_val;
        else if(T_max_val_u64 > D_max_val_u64 &&
                static_cast<uint64_t>(from) > D_max_val_u64)
            /* Check that, if T has a larger range than D,
             *  that from is still within range.
             * If not, return max_val.
             *  At this point, we know that from is larger
             *  than or equal to 0, and it is safe to cast
             *  to the largest unsigned type. */
            return max_val;
        else
            return from;
    }

//    return (from_signed > max_signed)
//            ? max_val
//            : (from_signed < min_signed)
//              ? min_val
//              : from;
}

#else

template<typename D, typename T,
		 typename std::enable_if<IS_INT(D) && IS_INT(T)>::type* = nullptr>
static inline D C_FCAST(T from)
{
	return static_cast<D>(from);
}

#endif

//template<typename D,
//         typename T,

//         typename std::enable_if<IS_INT(D), D>::type* = nullptr,
//         typename std::enable_if<std::is_enum<T>::value, T>::type* = nullptr

//         >
//static inline D C_FCAST(T from)
//{
//    return static_cast<D>(from);
//}

template<typename D,
         typename T,

         typename std::enable_if<IS_PTR(D),T>::type* = nullptr,
         typename std::enable_if<IS_PTR(T),T>::type* = nullptr,
         typename std::enable_if<IS_POD(RP(D)) && IS_POD(RP(T)),T>::type* = nullptr,
         typename std::enable_if<(IS_CONST(RP(D)) && IS_CONST(RP(T))) || (!IS_CONST(RP(D)) && !IS_CONST(RP(T))) || (IS_CONST(RP(D)) && !IS_CONST(RP(T))),T>::type* = nullptr

         >
/*!
 * \brief Simple reinterpretation of a pointer
 * \param from
 * \return
 */
static inline D C_FCAST(T from)
{
    return reinterpret_cast<D>(from);
}

template<typename D,
         typename T,

         typename std::enable_if<IS_INT(D), D>::type* = nullptr,
         typename std::enable_if<IS_PTR(T), T>::type* = nullptr

         >
static inline D C_FCAST(T from)
{
    return reinterpret_cast<D>(from);
}

template<typename D,
         typename T,

         typename std::enable_if<IS_PTR(D), D>::type* = nullptr,
         typename std::enable_if<!IS_CONST(D), D>::type* = nullptr,

         typename std::enable_if<IS_CONST(T), D>::type* = nullptr,
         typename std::enable_if<IS_PTR(T), T>::type* = nullptr
         >
static inline D C_FCAST(T from)
{
    using T_not_const = typename std::remove_cv<T>::type;
    return reinterpret_cast<D>(const_cast<T_not_const>(from));
}

template<typename D,
         typename T,

         typename std::enable_if<IS_PTR(D) && IS_PTR(T), D>::type* = nullptr,

         typename std::enable_if<IS_POD(RP(D)) && !IS_CONST(RP(D)), bool>::type* = nullptr,
         typename std::enable_if<IS_POD(RP(T)) && IS_CONST(RP(T)), bool>::type* = nullptr

         >
static inline D C_FCAST(T from)
{
    using T_not_const =
    typename std::add_pointer<typename std::remove_const<typename std::remove_pointer<T>::type>::type>::type;

    return reinterpret_cast<D>(const_cast<T_not_const>(from));
}

//template<typename D, typename T>
//static inline D C_FFCAST(T from)
//{
//    return (D)from;
//}

template<typename D, typename T>
static inline constexpr D C_CAST(T from)
{
    return static_cast<D>(from);
}
template<typename D, typename T>
static inline D* C_DCAST(T* from)
{
    return dynamic_cast<D*>(from);
}
template<typename D, typename T>
static inline constexpr D C_CCAST(T from)
{
    return const_cast<D>(from);
}
template<typename D, typename T>
static inline constexpr D C_RCAST(T from)
{
    return reinterpret_cast<D>(from);
}

#undef IS_INT
#undef IS_PTR
#undef RP
#undef IS_POD
#undef IS_CONST
#undef IS_SIGNED
#undef IS_USIGNED

#define C_CAST(type, var) C_CAST<type>(var)
#define C_DCAST(type, var) C_DCAST<type>(var)
#endif
