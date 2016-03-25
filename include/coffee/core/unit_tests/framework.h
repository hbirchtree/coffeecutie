#pragma once

#include <coffee/core/CDebug>
#include <coffee/core/CMD>
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
};

using TestList = Test*;
using Error = std::logic_error;

void run_tests(uint32 num, Test const* tests)
{
    Profiler::InitProfiler();
    CString tmp;

    Vector<cstring> titles;
    Vector<cstring> descriptions;
    /* Workaround: Vector<bool> gives void components. What? */
    Vector<bool> result;
    Vector<bool> required;

    for(uint32 i=0;i<num;i++)
    {
        Test const& test = tests[i];

        if(test.title)
            tmp = test.title;
        else
            tmp = cStringFormat("Test {0}",i+1);

        if(!test.test)
        {
            cBasicPrint("{0} skipped, nullptr",tmp);
            continue;
        }

        titles.push_back(test.title);
        descriptions.push_back(test.description);

        Profiler::PushContext(tmp.c_str());

        result.push_back(test.test());
        required.push_back(!test.optional);

        Profiler::PopContext();
    }

    Table::Header header;
    header.push_back("Test name");
    header.push_back("Description");
    header.push_back("Passed");
    header.push_back("Required");

    Table::Table table;
    table.push_back(Table::GenColumn(titles));
    table.push_back(Table::GenColumn(descriptions));
    table.push_back(Table::GenColumn(result));
    table.push_back(Table::GenColumn(required));

    cBasicPrint("-- Results: \n"
                "{0}",Table::GenTable(table,header));

    szptr suc = 0;
    for(bool v : result)
        if(v)
            suc++;

    cBasicPrint("-- Passed: {0}/{1}",suc,result.size());

    Profiling::ExitRoutine(0,nullptr,true);
}

}

#define COFFEE_RUN_TESTS(test_list) \
    int main(){ \
    size_t num = sizeof(test_list)/sizeof(CoffeeTest::Test); \
    CoffeeTest::run_tests(num,test_list); \
    return 0; \
    }