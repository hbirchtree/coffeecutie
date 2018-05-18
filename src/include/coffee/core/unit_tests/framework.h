#pragma once

#include <coffee/core/CApplication>
#include <coffee/core/CProfiling>
#include <coffee/core/CMD>
#include <coffee/core/terminal/table-print.h>

namespace CoffeeTest
{

struct Test
{
    using Fun = bool(*)();

    constexpr Test(Fun f, Coffee::cstring t,
                   Coffee::cstring d = nullptr,
                   bool o = false, bool r = true):
        test(f),
        title(t),
        description(d),
        optional(o),
        required_sequence(r)
    {
    }


    Fun test; /*!< The function containing the test */
    Coffee::cstring title; /*!< The title of the test, short */
    Coffee::cstring description; /*!< Description of test, long*/
    bool optional; /*!< Whether test is necessary for success */
    bool required_sequence; /*!< Whether this test is necessary for running next test */
};

using TestList = Test*;

extern int run_tests(Coffee::uint32 num, Test const* tests, int argc, char** argv);

}

#define COFFEE_RUN_TESTS(test_list) \
    int ref_main(int argc, char** argv){ \
        size_t num = sizeof(test_list)/sizeof(CoffeeTest::Test); \
        \
        return CoffeeTest::run_tests(C_CAST<Coffee::u32>(num),\
                                     test_list,\
                                     argc, argv); \
    } \
    COFFEE_APPLICATION_MAIN(ref_main)

#define COFFEE_TEST_SUITE(num_tests) \
    const constexpr CoffeeTest::Test _tests[num_tests]

#define COFFEE_EXEC_TESTS() COFFEE_RUN_TESTS(_tests)
