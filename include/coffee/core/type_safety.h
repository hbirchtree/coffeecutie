#pragma once

#include "plat/plat_quirks_toggling.h"

#include <type_traits>
#include <limits>
#include <stdint.h>

#define E_IF std::enable_if
#define IS_CLS std::is_class
#define IS_POLY std::is_polymorphic

namespace Coffee{

template<class T>
struct is_not_virtual
{
    typedef typename
    E_IF<!IS_POLY<T>::value>::type type;

    
};

}

#undef E_IF
#undef IS_CLS
#undef IS_POLY

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
 *  Never underflows or overflows.
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
}

#else

template<typename D, typename T,
		 typename std::enable_if<IS_INT(D) && IS_INT(T)>::type* = nullptr>
static inline D C_FCAST(T from)
{
	return static_cast<D>(from);
}

#endif

template<typename D,
         typename T,

         typename std::enable_if<IS_PTR(D),T>::type* = nullptr,
         typename std::enable_if<IS_PTR(T),T>::type* = nullptr,
         typename std::enable_if<IS_POD(RP(D)) && IS_POD(RP(T)),T>::type* = nullptr,
         typename std::enable_if<(IS_CONST(RP(D)) && IS_CONST(RP(T))) || (!IS_CONST(RP(D)) && !IS_CONST(RP(T))) || (IS_CONST(RP(D)) && !IS_CONST(RP(T))),T>::type* = nullptr

         >
/*!
 * \brief Simple reinterpretation of a pointer, does not allow breaking const rules.
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
/*!
 * \brief For converting pointer to integer type.
 * \param from
 * \return
 */
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
/*!
 * \brief This overload operates on the pointer type. Example:
 * `char* const` -> `char*`
 * \param from
 * \return
 */
static inline D C_FCAST(T from)
{
    using T_not_const = typename std::remove_cv<T>::type;
    return reinterpret_cast<D>(const_cast<T_not_const>(from));
}

template<typename D,
         typename T,

         typename std::enable_if<IS_PTR(D) && IS_PTR(T),
                                 D>::type* = nullptr,

         typename std::enable_if<IS_POD(RP(D)) && !IS_CONST(RP(D)),
                                 bool>::type* = nullptr,
         typename std::enable_if<IS_POD(RP(T)) && IS_CONST(RP(T)),
                                 bool>::type* = nullptr

         >
/*!
 * \brief This overload is for the case:
 * `const char*` -> `char*`, operating on the inner type.
 * \param from
 * \return
 */
static inline D C_FCAST(T from)
{
    using T_not_const =
    typename std::add_pointer<typename std::remove_const<
    typename std::remove_pointer<T>::type>::type>::type;

    return reinterpret_cast<D>(const_cast<T_not_const>(from));
}

template<typename D, typename T>
/*!
 * \brief C_CAST is for the cases where a static cast is acceptable. For example, when you *want* -1 to underflow.
 * Also used for enum casts
 * Few cases.
 * \param from
 * \return
 */
static inline constexpr D C_CAST(T from)
{
    return static_cast<D>(from);
}

template<typename D, typename T,
         typename std::enable_if<std::is_class<T>::value
                                 >::type* = nullptr>
/*!
 * \brief For coercing a class/struct into another type,
 *  such as with `operator T()`
 * \param from
 * \return
 */
static inline constexpr D C_OCAST(T& from)
{
    return static_cast<D>(from);
}
template<typename D, typename T>
/*!
 * \brief For dynamic casts. This function, as specified by the standard,
 *  will tell you if a dynamic_cast is unnecessary.
 * \param from
 * \return
 */
static inline D* C_DCAST(T* from)
{
    return dynamic_cast<D*>(from);
}
template<typename D, typename T>
/*!
 * \brief For discarding const-qualifier. Not recommended.
 * \param from
 * \return
 */
static inline constexpr D C_CCAST(T from)
{
    return const_cast<D>(from);
}
template<typename D, typename T,
         typename Coffee::is_not_virtual<T>::type* = nullptr>
/*!
 * \brief Should not be used with class/struct types,
 *  unless going from void* to class T.
 * Virtual classes are *NOT* allowed.
 * \param from
 * \return
 */
static inline constexpr D C_RCAST(T from)
{
    return reinterpret_cast<D>(from);
}

namespace Coffee {

template<typename Interface, typename Implementation>
struct implements
{
    typedef typename std::enable_if<
    std::is_base_of<Interface, Implementation>::value
    >::type type;
};

template<typename T>
struct is_pod
{
    typedef typename std::enable_if<
    std::is_pod<T>::value
    >::type type;
};

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
