#pragma once

#if defined(COFFEE_COMPILE_APPLICATION)
#include <coffee/core/CApplication>
#endif

#if MODE_LOWFAT
#include <stdio.h>
#endif

#include <coffee/core/CProfiling>

#include <coffee/core/CDebug>

struct test_failure : std::runtime_error
{
    using runtime_error::runtime_error;
};

namespace CoffeeTest {

struct Test
{
    using Fun = bool (*)();

    constexpr Test(
        Fun             f,
        Coffee::cstring t,
        Coffee::cstring d = nullptr,
        bool            o = false,
        bool            r = true)
        : test(f)
        , title(t)
        , description(d)
        , optional(o)
        , required_sequence(r)
    {
    }

    Fun             test;        /*!< The function containing the test */
    Coffee::cstring title;       /*!< The title of the test, short */
    Coffee::cstring description; /*!< Description of test, long*/
    bool            optional;    /*!< Whether test is necessary for success */
    bool required_sequence; /*!< Whether this test is necessary for running next
                               test */
};

struct StartupMessage
{
    StartupMessage(const char* msg)
    {
        if constexpr(!compile_info::lowfat_mode)
        {
            Coffee::Logging::log(
                stderr, "Test", msg, semantic::debug::Severity::Information);
        } else
            fprintf(stderr, "TESTING: %s\n", msg);
    }
};

using TestList = Test*;

extern int run_tests(Coffee::u32 num, Test const* tests, int argc, char** argv);

} // namespace CoffeeTest

namespace assertion {

FORCEDINLINE void assertTrue_impl(bool result, const char* context)
{
    if(!result)
        throw test_failure(context);
}

template<typename T1, typename T2 = T1>
FORCEDINLINE void assertEquals_impl(T1 v1, T2 v2, const char* context)
{
    if(v1 != v2)
        throw test_failure(context);
}

template<typename T1, typename T2 = T1>
FORCEDINLINE void assertNotEquals_impl(T1 v1, T2 v2, const char* context)
{
    if(v1 == v2)
        throw test_failure(context);
}

} // namespace assertion

namespace assert {

FORCEDINLINE void True(bool result)
{
    assertion::assertTrue_impl(result, "boolean check failed");
}

FORCEDINLINE void False(bool result)
{
    assertion::assertTrue_impl(!result, "boolean check failed");
}

template<typename T1, typename T2 = T1>
FORCEDINLINE void Equals(T1 v1, T2 v2)
{
    assertion::assertEquals_impl<T1, T2>(v1, v2, "equivalence failed");
}

template<typename T1, typename T2 = T1>
FORCEDINLINE void NotEquals(T1 v1, T2 v2)
{
    assertion::assertNotEquals_impl<T1, T2>(v1, v2, "equivalence failed");
}

} // namespace assert

template<size_t NumTests>
using TestArray = Coffee::Array<CoffeeTest::Test, NumTests>;

#define ASSERT_CONTEXT __FILE__ ":" C_STR(__LINE__) ": "

#define assertEquals(v1, v2)      \
    assertion::assertEquals_impl( \
        v1,                       \
        v2,                       \
        ASSERT_CONTEXT "equivalence failed: " C_STR(v1) " == " C_STR(v2))

#define assertNotEquals(v1, v2)      \
    assertion::assertNotEquals_impl( \
        v1,                          \
        v2,                          \
        ASSERT_CONTEXT "equivalence failed: " C_STR(v1) " != " C_STR(v2))

#define assertTrue(val)         \
    assertion::assertTrue_impl( \
        val, ASSERT_CONTEXT "boolean check failed: " C_STR(val))

#define assertFalse(val)        \
    assertion::assertTrue_impl( \
        !val, ASSERT_CONTEXT "boolean check failed: !" C_STR(val))

#define COFFEE_RUN_TESTS(test_list)                                  \
    int ref_main(int argc, char** argv)                              \
    {                                                                \
        size_t num = test_list.size();                               \
                                                                     \
        return CoffeeTest::run_tests(                                \
            C_CAST<Coffee::u32>(num), test_list.data(), argc, argv); \
    }                                                                \
    COFFEE_APPLICATION_MAIN(ref_main)

#define COFFEE_TEST_SUITE(num_tests) const constexpr TestArray<num_tests> _tests

#define COFFEE_EXEC_TESTS() COFFEE_RUN_TESTS(_tests)

#define COFFEE_TESTS_BEGIN(num) COFFEE_TEST_SUITE(num) = {{
#define COFFEE_TESTS_END() \
    }                      \
    }                      \
    ;                      \
    COFFEE_EXEC_TESTS()

#define COFFEE_TESTS_DISCLAIMER(id, msg)        \
    extern CoffeeTest::StartupMessage msg_##id; \
    CoffeeTest::StartupMessage        msg_##id(msg);
