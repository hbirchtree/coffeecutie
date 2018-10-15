#include <coffee/core/unit_tests/framework.h>

#include <coffee/core/CJSONParser>
#include <coffee/core/CMD>
#include <coffee/core/CPlatform>
#include <coffee/core/plat/environment/argument_parse.h>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/string_casting.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/core/terminal/table-print.h>
#include <coffee/core/types/cdef/memtypes.h>
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/tdef/stltypes.h>

#include <coffee/core/CDebug>

namespace CoffeeTest {

using namespace Coffee;

struct TestInstance
{
    void from_test(Test const& test_info)
    {
        title       = test_info.title;
        description = test_info.description;
        required    = !test_info.optional;
    }

    void submit(bool result, u64 time)
    {
        this->result = result;
        this->time   = time;
    }

    u64 time;

    cstring title;
    cstring description;

    bool result;
    bool required;
};

static Vector<cstring> titles;
static Vector<cstring> descriptions;
static Vector<u64>  test_times;
static Vector<bool>    result;
static Vector<bool>    required;

void WriteJsonData(
    JSON::WriteBuf& buf,
    szptr const&    suc,
    u64 const&   total_time,
    Test const*     tests)
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
    if(suc < result.size())
    {
        root.Key("passed");
        root.StartArray();
        for(u32 i = 0; i < result.size(); i++)
        {
            root.StartObject();

            root.Key("title");
            if(tests[i].title)
                root.String(tests[i].title);
            else
            {
                CString fmt = cStringFormat("Test {0}", i);
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
        szptr i = 0;
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
                    CString fmt = cStringFormat("Test {0}", i);
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
        i = 0;
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

void PrintAsciiTable(u64 const& time_accum, szptr suc)
{
    Table::Header header;
    header.push_back("Test name");
    header.push_back("Description");
    header.push_back("Passed");
    header.push_back("Required");
    header.push_back("Time (us)");

    Table::Table table(header);
    table.push_back(Table::GenColumn(titles));
    table.push_back(Table::GenColumn(descriptions));
    table.push_back(Table::GenColumn(result));
    table.push_back(Table::GenColumn(required));
    table.push_back(Table::GenColumn(test_times));

    cBasicPrint(
        "-- Results: \n"
        "{0}",
        Table::GenTable(table, header));

    cBasicPrint("Total time: {0} s", C_CAST<bigscalar>(time_accum) / 1000000);

    cBasicPrint("-- Passed: {0}/{1}", suc, result.size());
}

struct TemporaryState
{
    TemporaryState():
        m_state(State::GetInternalState()),
        m_tstate(State::GetInternalThreadState())
    {
        State::SetInternalState(State::CreateNewState());
        State::SetInternalThreadState(State::CreateNewThreadState());
    }

    ~TemporaryState()
    {
        State::SetInternalState(m_state);
        State::SetInternalThreadState(m_tstate);
    }

private:
    State::P<InternalState> m_state;
    State::P<InternalThreadState> m_tstate;
};

void RunTest(Test const& test, TestInstance& test_info)
{
    test_info.from_test(test);

    if(!test.test)
    {
        cBasicPrint("{0} skipped, nullptr", test_info.title);
        return;
    }

    auto start_time = Chrono::steady_clock::now();
    bool res        = false;

    Profiler::PushContext(test_info.title);
    try
    {
        TemporaryState _;

        res = test.test();
    } catch(std::exception const& e)
    {
        cWarning(
            "exception encountered: {0}: {1}",
            Stacktracer::DemangleSymbol(typeid(e).name()),
            e.what());
        Profiler::Profile("exception");
    }
    Profiler::PopContext();

    test_info.submit(
        res,
        C_FCAST<u64>(Chrono::duration_cast<Chrono::microseconds>(
                         Chrono::steady_clock::now() - start_time)
                         .count()));
}

int run_tests(u32 num, Test const* tests, int argc, char** argv)
{
    State::SetInternalThreadState(State::CreateNewThreadState());

    SetPrintingVerbosity(10);
    auto args = AppArg::Clone(argc, argv);

    RuntimeQueue::CreateNewQueue("Testing");

    ArgumentParser parser;
    parser.addSwitch("json", "json", nullptr, "Output JSON document");
    auto parseArgs = parser.parseArguments(args);

    bool json_formatting = parseArgs.switches.count("json");

    Vector<TestInstance> test_results;
    test_results.resize(num);

    titles.resize(num);
    descriptions.resize(num);
    test_times.resize(num);
    result.reserve(num);
    required.reserve(num);

    bool fail = false;

    for(u32 i = 0; i < num; i++)
    {
        TestInstance& test_info = test_results.at(i);
        RunTest(tests[i], test_info);

        if(test_info.required && !test_info.result)
            fail = true;
    }

    result.resize(num, false);
    required.resize(num, true);

    szptr suc = 0;
    for(bool v : result)
        if(v)
            suc++;

    u64 time_accum = 0;

    for(u64 v : test_times)
        time_accum += v;

    ExtraData::Add("testing:title", GetCurrentApp().application_name);
    ExtraData::Add("testing:bmark", cast_pod(time_accum));
    ExtraData::Add("testing:result", fmt("{0},{1}", suc, num));
    ExtraData::Add("testing:mem", cast_pod(ProcessProperty::Mem(0)));

    for(auto i : Range<>(test_results.size()))
    {
        auto const& test = test_results.at(i);

        titles.at(i)       = test.title;
        descriptions.at(i) = test.description;
        test_times.at(i)   = test.time;
        result.at(i)       = test.result;
        required.at(i)     = test.required;
    }

    if(!json_formatting)
    {
        PrintAsciiTable(time_accum, suc);
    } else
    {
        JSON::WriteBuf buf;
        WriteJsonData(buf, suc, time_accum, tests);

        cOutputPrint("{0}", buf.GetString());

        ExtraData::Add("testing:jsonresult", buf.GetString());
    }

    cDebug("Memory consumption: {0} kB", ProcessProperty::Mem(0));

    /* For verbosity, we write it as this */
    return (fail) ? 1 : 0;
}

} // namespace CoffeeTest
