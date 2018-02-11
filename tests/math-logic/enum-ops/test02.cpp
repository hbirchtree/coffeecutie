#include <coffee/core/CUnitTesting>
#include <coffee/core/type_safety.h>

using namespace Coffee;

static constexpr u32 big_number = 0x1234;

enum class TestingEnum
{
    EvaluatesToZero = 0,

    EvaluatesToBigNumber = big_number
};

bool enum_cast()
{
    if(C_FCAST<u32>(TestingEnum::EvaluatesToZero) != 0)
        return false;

    if(C_FCAST<u32>(TestingEnum::EvaluatesToBigNumber) != big_number)
        return false;

    return true;
}

template<typename T>
using num_limits = std::numeric_limits<T>;

bool downcasting_u32_u16()
{
    u32 value = 0;

    /* Valid cast, in range and all */
    if((value = C_FCAST<u16, u32>(0x1234)) != 0x1234)
        return false;

    /* Weird cast, number is too big for type */
    if((value = C_FCAST<u8>(0x1234)) != num_limits<u8>::max())
        return false;

    return true;
}

bool casting_signed_unsigned()
{
    if(C_FCAST<i8>(num_limits<u8>::max()) != num_limits<i8>::max())
        return false;

    if(C_FCAST<u8>(num_limits<i8>::min()) != num_limits<u8>::min())
        return false;

    if(C_FCAST<u8>(num_limits<i16>::min()) != num_limits<u8>::min())
        return false;

    if(C_FCAST<i8>(num_limits<i16>::min()) != num_limits<i8>::min())
        return false;

    return true;
}

#if defined(COFFEE_WINDOWS)
static constexpr bool supports_safe_cast = false;
#else
static constexpr bool supports_safe_cast = true;
#endif

COFFEE_TEST_SUITE(3) = {
{enum_cast, "Casting enums to int"},
{downcasting_u32_u16, "Downcasting", nullptr, true, false},
{casting_signed_unsigned, "Unsigned casting", nullptr, true, false}
};

COFFEE_EXEC_TESTS();
