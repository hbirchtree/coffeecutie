#pragma once

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
};

using TestList = Test*;
using Error = std::logic_error;

void run_tests(uint32 num, Test const* tests, int argc, char** argv)
{
    ProcessProperty::CoreDumpEnable();

    bool json_formatting = false;

    json_formatting = ArgParse::Check(argc,argv,"json");

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

        bool res = test.test();
        result.push_back(res);
        required.push_back(!test.optional);

        Profiler::PopContext();
    }

    szptr suc = 0;
    for(bool v : result)
        if(v)
            suc++;

    if(!json_formatting)
    {
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

        cBasicPrint("-- Passed: {0}/{1}",suc,result.size());
    }else{
        JSON::WriteBuf buf;
        JSON::Writer root(buf);

        root.StartObject();

        /* List total score */
        root.Key("complete");
        root.Int64(suc);
        root.Key("total");
        root.Int64(result.size());

        /* If all tests passed, don't add this */
        if(suc<result.size())
        {
            /* List required tests */
            root.Key("failed");
            root.StartArray();
            szptr i=0;
            for(bool v : result)
            {
                if(!v)
                {
                    Test const& o = tests[i];

                    if(o.optional)
                        continue;

                    root.StartObject();

                    root.Key("title");
                    if(o.title)
                        root.String(o.title);
                    else
                    {
                        CString fmt = cStringFormat("Test {0}",i);
                        root.String(fmt.c_str());
                    }

                    if(o.description)
                    {
                        root.Key("desc");
                        root.String(o.description);
                    }


                    root.EndObject();
                }
                i++;
            }
            root.EndArray();

            /* List optional tests */
            root.Key("optional");
            root.StartArray();
            i=0;
            for(bool v : result)
            {
                if(!v)
                {
                    Test const& o = tests[i];

                    if(!o.optional)
                        continue;

                    root.StartObject();

                    root.Key("title");
                    root.String(o.title);

                    root.Key("desc");
                    root.String(o.description);

                    root.EndObject();
                }
                i++;
            }
            root.EndArray();
        }

        root.EndObject();

        cOutputPrint("{0}",buf.GetString());
    }

    Profiling::ExitRoutine(0,nullptr,true);
}

}

#define COFFEE_RUN_TESTS(test_list) \
    int main(int argc, char** argv){ \
    size_t num = sizeof(test_list)/sizeof(CoffeeTest::Test); \
    CoffeeTest::run_tests(num,test_list,argc,argv); \
    return 0; \
    }
