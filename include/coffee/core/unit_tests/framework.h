#pragma once

#include <coffee/core/CApplication>
#include <coffee/core/CProfiling>
#include <coffee/core/CDebug>
#include <coffee/core/CMD>

namespace CoffeeTest
{

using namespace Coffee;

struct Test
{
    using Fun = bool(*)();

    Fun test;
    cstring title;
    cstring description;
    bool optional;
    bool required_sequence;
};

using TestList = Test*;

extern void run_tests(uint32 num, Test const* tests, int argc, char** argv);

}

#define COFFEE_RUN_TESTS(test_list) \
    int ref_main(int argc, char** argv){ \
    size_t num = sizeof(test_list)/sizeof(CoffeeTest::Test); \
    CoffeeTest::run_tests(num,test_list,argc,argv); \
    return 0; \
    } \
    COFFEE_APPLICATION_MAIN(ref_main)
