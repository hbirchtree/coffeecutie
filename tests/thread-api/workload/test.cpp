#include <coffee/core/CThreading>
#include <coffee/core/types/cdef/memsafe.h>
#include <coffee/core/terminal/table-print.h>

#include <coffee/core/CDebug>
#include <coffee/core/CUnitTesting>

using namespace Coffee;

using Clock = Chrono::high_resolution_clock;

bool workload_test()
{
    struct DataSet
    {
        CSize64 size;
        u8*  value;
    } data;
    data.size.w = 1024 * 1024;
    data.size.h = 1024;

    Bytes workspace = Bytes::Alloc(data.size.area());

    data.value = C_CAST<u8*>(workspace.data);

    Profiler::Profile("Memory allocation");

    Function<void(u64, DataSet*)> kern = [](u64 i, DataSet* d) {
        for(i32 j = 0; j < 64; j++)
            d->value[i * 64 + j] /= 2;
    };

    Vector<Chrono::microseconds> timing;
    Vector<CString>              titles;
    titles.push_back("Parallel");
    titles.push_back("Serial");

    Profiler::Profile("Task setup");

    auto start_time = Chrono::high_resolution_clock::now();

    Threads::ParallelFor(kern, data.size.area() / 64, &data).get();
    timing.push_back(
        Chrono::duration_cast<Chrono::microseconds>(Clock::now() - start_time));

    start_time = Chrono::high_resolution_clock::now();
    for(u64 i = 0; i < data.size.area(); i++)
    {
        data.value[i] /= 2;
    }
    timing.push_back(
        Chrono::duration_cast<Chrono::microseconds>(Clock::now() - start_time));

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
