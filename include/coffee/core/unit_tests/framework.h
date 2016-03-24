#pragma once

#include <coffee/core/CDebug>
#include <coffee/core/profiler/profiling-export.h>
#include "../types/cdef/memtypes.h"
#include "../types/tdef/integertypes.h"
#include "../types/tdef/stltypes.h"

namespace CoffeeTest
{

using namespace Coffee;

using Test = bool(*)();
using TestList = Test*;
using Error = std::logic_error;

void run_tests(uint32 num, Test const* tests)
{
    Profiler::InitProfiler();
    CString tmp;
    try{
        for(uint32 i=0;i<num;i++)
        {
            tmp = cStringFormat("Test {0}",i+1);
            Test test = tests[i];
            if(!test)
            {
                cDebug("{0} skipped, nullptr",tmp);
                continue;
            }
            Profiler::PushContext(tmp.c_str());
            if(!test())
            {
                CString fmt = cStringFormat("Error on: {0}",(const void* const&)test);
                throw Error(fmt.c_str());
            }
            Profiler::PopContext();
            cDebug("{0} completed successfully",tmp);
        }
    }
    catch(Error e)
    {
        cWarning("{0}\n"
                 "Failed with:\n"
                 "{1}",
                 Env::ExecutableName(),
                 e.what());
        Profiler::PopContext();
    }
    Profiling::ExitRoutine(0,nullptr,true);
}

}

#define COFFEE_RUN_TESTS(test_list) \
    int main(){ \
    size_t num = sizeof(test_list)/sizeof(CoffeeTest::Test); \
    CoffeeTest::run_tests(num,test_list); \
    return 0; \
    }