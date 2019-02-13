#define JOB_SYSTEM_PROFILE 1

#include <coffee/core/CProfiling>

#include <coffee/core/CThreading>
#include <coffee/core/types/chunk.h>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/time_types.h>

#include <coffee/core/CUnitTesting>
#include <coffee/core/terminal/table.h>

#include <coffee/core/types/vector_types.h>

using namespace Coffee;

using Clock = Chrono::high_resolution_clock;

bool workload_test()
{
    PrintingVerbosityLevel() = 10;

    size_2d<u64> size;
    size.w = 128;
    size.h = 128;

    mem_chunk<Matf4> workspace = mem_chunk<Matf4>::Alloc(size.area());

    Profiler::Profile("Memory allocation");

    Function<void(Matf4&)> kern = [](Matf4& v) {
        ProfContext _("Running kernel");
        v = typing::vectors::inverse(v);
    };

    Vector<Chrono::microseconds> timing;
    Vector<CString>              titles;
    titles.push_back("Parallel");
    titles.push_back("Serial");

    Profiler::Profile("Task setup");

    Profiler::PushContext("Parallel");
    auto start_time = Chrono::high_resolution_clock::now();

    threads::Parallel::ForEach<
        decltype(workspace),
        threads::parametric_parallel<8, decltype(workspace)>>(
        workspace, std::move(kern));
    timing.push_back(
        Chrono::duration_cast<Chrono::microseconds>(Clock::now() - start_time));

    Profiler::PopContext();

    Profiler::PushContext("Serial");
    start_time = Chrono::high_resolution_clock::now();
    for(auto& v : workspace)
    {
        kern(v);
    }
    timing.push_back(
        Chrono::duration_cast<Chrono::microseconds>(Clock::now() - start_time));
    Profiler::PopContext();

    Table::Header head;
    head.push_back("Workload type");
    head.push_back("Time score");

    Table::Table table(head);
    table.push_back(Table::GenColumn(titles.data(), titles.size()));
    table.push_back(Table::GenColumn(timing.data(), timing.size()));

    cBasicPrint("{0}", table);

    return true;
}

const constexpr CoffeeTest::Test _tests[1] = {
    {workload_test, "Threaded workload test", nullptr, true, false}};

COFFEE_RUN_TESTS(_tests);
