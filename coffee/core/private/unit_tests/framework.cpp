#include <coffee/core/unit_tests/framework.h>

#include <coffee/core/CDebug>
#include <coffee/core/CMD>
#include <coffee/core/CJSONParser>
#include <coffee/core/CPlatform>
#include <coffee/core/plat/environment/argument_parse.h>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/types/cdef/memtypes.h>
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/tdef/stltypes.h>

#include <coffee/core/string_casting.h>

namespace CoffeeTest{

using namespace Coffee;

static Vector<cstring> titles;
static Vector<cstring> descriptions;
static Vector<uint64> test_times;
static Vector<bool> result;
static Vector<bool> required;

void WriteJsonData(JSON::WriteBuf& buf, szptr const& suc,
                   uint64 const& total_time,
                   Test const* tests)
{
    JSON::Writer root(buf);

    root.StartObject();

    /* List total score */
    root.Key("complete");
    root.Uint64(suc);
    root.Key("total");
    root.Uint64(result.size());
    root.Key("time");
    root.Uint64(total_time);

    /* If all tests passed, don't add this */
    if(suc<result.size())
    {
        root.Key("passed");
        root.StartArray();
        for(uint32 i=0;i<result.size();i++)
        {
            root.StartObject();

            root.Key("title");
            if(tests[i].title)
                root.String(tests[i].title);
            else
            {
                CString fmt = cStringFormat("Test {0}",i);
                root.String(fmt.c_str());
            }

            root.Key("time");
            root.Uint64(test_times[i]);

            root.EndObject();
        }
        root.EndArray();

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
}

void PrintAsciiTable(uint64 const& time_accum, szptr suc)
{
    Table::Header header;
    header.push_back("Test name");
    header.push_back("Description");
    header.push_back("Passed");
    header.push_back("Required");
    header.push_back("Time");

    Table::Table table(header);
    table.push_back(Table::GenColumn(titles));
    table.push_back(Table::GenColumn(descriptions));
    table.push_back(Table::GenColumn(result));
    table.push_back(Table::GenColumn(required));
    table.push_back(Table::GenColumn(test_times));

    cBasicPrint("-- Results: \n"
                "{0}",Table::GenTable(table,header));

    cBasicPrint("Total time: {0} s", C_CAST<bigscalar>(time_accum)/1000000);

    cBasicPrint("-- Passed: {0}/{1}",suc,result.size());
}

void RunTest(uint32 i, CString& tmp, Test const* tests, bool& fail)
{
    Test const& test = tests[i];

    if(test.title)
        tmp = test.title;
    else
        tmp = cStringFormat("Test {0}",i+1);

    if(!test.test)
    {
        cBasicPrint("{0} skipped, nullptr",tmp);
        return;
    }

    titles[i] = test.title;
    descriptions[i] = test.description;

    if(fail)
    {
        result.push_back(false);
        required.push_back(!test.optional);
        return;
    }

    Profiler::PushContext(tmp.c_str());

    auto start_time = Chrono::steady_clock::now();
    bool res = test.test();
    test_times[i] = Chrono::duration_cast<Chrono::microseconds>(Chrono::steady_clock::now() - start_time).count();
    result.push_back(res);
    required.push_back(!test.optional);

    Profiler::PopContext();

    if(!res && (test.required_sequence || !test.optional))
        fail = true;
}

int run_tests(uint32 num, Test const* tests, int argc, char** argv)
{
    auto args = AppArg::Clone(argc, argv);

    ArgumentParser parser;

    parser.addSwitch("json", "json", nullptr, "Output JSON document");

    auto parseArgs = parser.parseArguments(args);

    bool json_formatting = parseArgs.switches.count("json");


    CString tmp;

    titles.resize(num);
    descriptions.resize(num);
    result.reserve(num);
    required.reserve(num);
    test_times.resize(num);

    bool fail = false;


    for(uint32 i=0;i<num;i++)
    {
        RunTest(i,tmp,tests,fail);
    }

    result.resize(num,false);
    required.resize(num,true);

    szptr suc = 0;
    for(bool v : result)
        if(v)
            suc++;

    uint64 time_accum = 0;

    for(uint64 v : test_times)
        time_accum += v;

    Profiler::AddExtraData(
                "testing:title",
                GetCurrentApp().application_name);

    Profiler::AddExtraData("testing:bmark", cast_pod(time_accum));

    Profiler::AddExtraData("testing:result", cStringFormat("{0},{1}",suc,num));

    Profiler::AddExtraData("testing:mem", cast_pod(ProcessProperty::Mem(0)));

    if(!json_formatting)
    {
        PrintAsciiTable(time_accum,suc);
    }else{
        JSON::WriteBuf buf;
        WriteJsonData(buf, suc, time_accum, tests);

        cOutputPrint("{0}",buf.GetString());

        Profiler::AddExtraData("testing:jsonresult",buf.GetString());
    }

    cDebug("Memory consumption: {0} kB", ProcessProperty::Mem(0));

	/* For verbosity, we write it as this */
	return (fail) ? 1 : 0;
}

}
