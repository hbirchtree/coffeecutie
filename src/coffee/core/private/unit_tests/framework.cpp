#include <coffee/core/CJSONParser>
#include <coffee/core/CPlatform>
#include <coffee/core/internal_state.h>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/core/types/chunk.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/types.h>
#include <platforms/argument_parse.h>
#include <platforms/process.h>
#include <platforms/stacktrace.h>

#include <coffee/strings/libc_types.h>

#define MODE_CRASHTEST 0

/* This is placed here because it includes CDebug */
#include <coffee/core/unit_tests/framework.h>

#include <coffee/core/terminal/table.h>

namespace CoffeeTest {

using namespace ::platform::args;
using namespace ::platform::env;
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
static Vector<u64>     test_times;
static Vector<bool>    result;
static Vector<bool>    required;

void WriteJsonData(
    JSON::WriteBuf& buf,
    szptr const&    suc,
    u64 const&      total_time,
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
                CString fmt = Strings::cStringFormat("Test {0}", i);
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
                    CString fmt = Strings::cStringFormat("Test {0}", i);
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
    TemporaryState() :
        m_state(State::GetInternalState()),
        m_tstate(State::GetInternalThreadState())
    {
        auto profiler    = State::PeekState("jsonProfiler");
        auto threadNames = State::PeekState("threadNames");

        State::SetInternalState(State::CreateNewState());
        State::SetInternalThreadState(State::CreateNewThreadState());

        State::SwapState("jsonProfiler", profiler);
        State::SwapState("threadNames", threadNames);

        auto profilerState = State::GetProfilerStore();

        if(profilerState)
            profilerState->flags.enabled = true;
    }

    ~TemporaryState()
    {
        State::SetInternalState(m_state);
        State::SetInternalThreadState(m_tstate);
    }

  private:
    State::P<State::InternalState>       m_state;
    State::P<State::InternalThreadState> m_tstate;
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

    cDebug("Starting test {0}", test_info.title);

    Profiler::PushContext(test_info.title);
#if !MODE_CRASHTEST
    try
#endif
    {
        C_UNUSED(TemporaryState _);

        res = test.test();
    }
#if !MODE_CRASHTEST
    catch(std::exception const&)
    {
        if constexpr(!compile_info::lowfat_mode)
            platform::env::Stacktracer::ExceptionStacktrace(
                std::current_exception(),
                DebugFun::OutputPrinter::fprintf_platform);
        else
            platform::env::Stacktracer::ExceptionStacktrace(
                std::current_exception(), typing::logging::fprintf_logger);
        Profiler::Profile("exception");
    }
#endif
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
    ExtraData::Add("testing:result", Strings::fmt("{0},{1}", suc, num));
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
