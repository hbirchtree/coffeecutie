#include <coffee/core/CUnitTesting>
#include <coffee/core/types/edef/enumfun.h>

using namespace Coffee;

enum IntegerEnum
{
    IE1 = 0x1,
    IE2 = 0x2,
    IE3 = 0x3,
    IE4 = 0x4,
};

enum class ClassEnum
{
    E1 = 0x1,
    E2 = 0x2,
    E3 = 0x3,
    E4 = 0x4,
};

C_FLAGS(ClassEnum,uint32);

namespace IntegerTests{
bool _and()
{
    return (IE1&IE3) == IE1;
}

bool _or()
{
    return (IE1|IE2) == IE3;
}

bool _xor()
{
    return (IE3^IE1) == IE2;
}

bool assign_and()
{
    uint32 v = IE1|IE4;
    v &= IE3;
    return v == IE1;
}

bool assign_or()
{
    uint32 v = IE1;
    v |= IE2;
    return v == IE3;
}

bool assign_xor()
{
    uint32 v = IE3;
    v ^= IE1;
    return v == IE2;
}
}

namespace ClassTests{
bool _and()
{
    return (ClassEnum::E1&ClassEnum::E3) == ClassEnum::E1;
}

bool _or()
{
    return (ClassEnum::E1|ClassEnum::E2) == ClassEnum::E3;
}

bool _xor()
{
    return (ClassEnum::E3^ClassEnum::E1) == ClassEnum::E2;
}

bool assign_and()
{
    ClassEnum v = ClassEnum::E1|ClassEnum::E4;
    v &= ClassEnum::E3;
    return v == ClassEnum::E1;
}

bool assign_or()
{
    ClassEnum v = ClassEnum::E1;
    v |= ClassEnum::E2;
    return v == ClassEnum::E3;
}

bool assign_xor()
{
    ClassEnum v = ClassEnum::E3;
    v ^= ClassEnum::E1;
    return v == ClassEnum::E2;
}
}

const constexpr CoffeeTest::Test _tests[12] = {
    {IntegerTests::_and,"Bitwise AND on integer enum"},
    {IntegerTests::_or,"Bitwise OR on integer enum"},
    {IntegerTests::_xor,"Bitwise XOR on integer enum"},
    {IntegerTests::assign_and,"Bitwise AND on integer enum with &="},
    {IntegerTests::assign_or,"Bitwise OR on integer enum with |="},
    {IntegerTests::assign_xor,"Bitwise XOR on integer enum with ^="},

    {ClassTests::_and,"Bitwise AND on class enum"},
    {ClassTests::_or,"Bitwise OR on class enum"},
    {ClassTests::_xor,"Bitwise XOR on class enum"},
    {ClassTests::assign_and,"Bitwise AND on class enum with &="},
    {ClassTests::assign_or,"Bitwise OR on class enum with |="},
    {ClassTests::assign_xor,"Bitwise XOR on class enum with ^="},
};

COFFEE_RUN_TESTS(_tests);