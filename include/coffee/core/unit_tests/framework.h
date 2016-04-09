#pragma once

#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CMD>
#include <coffee/core/CJSONParser>
#include <coffee/core/CPlatform>
#include <coffee/core/plat/environment/argument_parse.h>
#include <coffee/core/profiler/profiling-export.h>
#include "../types/cdef/memtypes.h"
#include "../types/tdef/integertypes.h"
#include "../types/tdef/stltypes.h"

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
using Error = std::logic_error;

extern void run_tests(uint32 num, Test const* tests, int argc, char** argv);

}

#define COFFEE_RUN_TESTS(test_list) \
    int ref_main(int argc, char** argv){ \
    size_t num = sizeof(test_list)/sizeof(CoffeeTest::Test); \
    CoffeeTest::run_tests(num,test_list,argc,argv); \
    return 0; \
    } \
    COFFEE_APPLICATION_MAIN(ref_main)
