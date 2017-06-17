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

template <typename D,
          typename T,

          typename std::enable_if<IS_SIGNED(D), T>::type* = nullptr,
          typename std::enable_if<IS_USIGNED(T), T>::type* = nullptr

          >
/*!
 * \brief "Safe" casting between unsigned and signed types
 * \param from
 * \return The correct integer is all is good, 0 if it cannot be represented
 */
static inline D C_FCAST(T from)
{
    return ((from > std::numeric_limits<D>::max())
            ? 0
            : from);
}
template <typename D,
          typename T,

          typename std::enable_if<IS_USIGNED(D), D>::type* = nullptr,
          typename std::enable_if<IS_SIGNED(T), T>::type* = nullptr

          >
/*!
 * \brief "Safe" casting between unsigned and signed types
 * \param from
 * \return The correct integer if all is good, 0 if it cannot be represented
 */
static inline D C_FCAST(T from)
{
    return ((from < 0)
            ? 0
            : from);
}

#define IS_INT(Type) std::is_integral<Type>::value
#define IS_PTR(Type) std::is_pointer<Type>::value
#define RP(Type) typename std::remove_pointer<Type>::type
//#define IBASE(Type) (std::is_void<Type>::value || std::is_integral<Type>::value || std::is_class<Type>::value || std::is_floating_point<Type>::value)
#define IS_POD(Type) (!std::is_pointer<Type>::value)
#define IS_CONST(Type) std::is_const<Type>::value

template<typename D,
         typename T,

         typename std::enable_if<std::is_convertible<T,D>::value,T>::type* = nullptr,
         typename std::enable_if<IS_INT(D),T>::type* = nullptr,
         typename std::enable_if<IS_INT(T),T>::type* = nullptr,

         typename std::enable_if<!((IS_SIGNED(T) && IS_USIGNED(D)) || (IS_USIGNED(T) && IS_SIGNED(D))), T>::type* = nullptr

         >
/*!
 * \brief Integral casting, ex. u32 to u16
 * \param from
 * \return
 */
static inline D C_FCAST(T from)
{
    return static_cast<D>(from);
}

template<typename D,
         typename T,

         typename std::enable_if<IS_INT(D), D>::type* = nullptr,
         typename std::enable_if<std::is_enum<T>::value, T>::type* = nullptr

         >
static inline D C_FCAST(T from)
{
    return static_cast<D>(from);
}

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

//template<typename D,
//         typename T,

//         typename std::enable_if<std::is_same<T, void*>::value, bool>::type* = nullptr,
//         typename std::enable_if<IBASE(RP(D)), bool>::type* = nullptr,
//         typename std::enable_if<NPTR(D), bool>::type* = nullptr,
//         typename std::enable_if<(CONST(RP(D)) && CONST(RP(T))) || (!CONST(RP(D)) && !CONST(RP(T))) || (CONST(RP(D)) && !CONST(RP(T))),T>::type* = nullptr

//         >
///*!
// * \brief Simple reinterpretation of a pointer
// * \param from
// * \return
// */
//static inline D C_FCAST(T from)
//{
//    return reinterpret_cast<D>(from);
//}

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
static inline D C_CAST(T from)
{
    return static_cast<D>(from);
}
template<typename D, typename T>
static inline D* C_DCAST(T* from)
{
    return dynamic_cast<D*>(from);
}
template<typename D, typename T>
static inline D C_CCAST(T from)
{
    return const_cast<D>(from);
}
template<typename D, typename T>
static inline D C_RCAST(T from)
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
