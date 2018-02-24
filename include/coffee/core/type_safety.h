#pragma once

#include "plat/plat_quirks_toggling.h"

#include <type_traits>
#include <limits>

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
    constexpr D max_val = std::numeric_limits<D>::max();
    constexpr D min_val = std::numeric_limits<D>::min();

    return (from > max_val)
            ? max_val
            : (from < min_val)
              ? min_val
              : from;
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
