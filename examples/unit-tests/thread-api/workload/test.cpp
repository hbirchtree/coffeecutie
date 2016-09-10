#include <coffee/core/CUnitTesting>
#include <coffee/core/CThreading>
#include <coffee/core/CDebug>

using namespace Coffee;

bool workload_test()
{
    struct DataSet
    {
        CSize64 size;
        uint8* value;
    } data;
    data.size.w = 1024*1024;
    data.size.h = 1024;

    data.value = (uint8*)Alloc(data.size.area());

    if(!data.value)
    {
        cDebug("Failed to allocate {0} B of memory",data.size.area());
        /* If we fail to allocate the memory, try again with less memory, reducing it */
        /* This may be a problem with low-memory systems (eg. RPi) */
        uint64 sz = 1024;
        while(!data.value)
        {
            sz /= 2;

            data.size.w = sz * sz;
            data.size.h = sz;

            cDebug("Reducing to {0} B of memory",data.size.area());

            data.value = (uint8*)Alloc(data.size.area());

            if(sz <= 2)
                break;
        }

        if(!data.value)
        {
            cWarning("Failed to allocate memory :(");
            return false;
        }else
            cDebug("Settling with {0} B of memory",data.size.area());
    }

	Profiler::Profile("Memory allocation");

    Threads::Function<void(uint64,DataSet*)> kern = [](uint64 i, DataSet* d)
    {
        for(int32 j=0;j<64;j++)
            d->value[i*64+j] /= 2;
    };

	Vector<uint64> timing;
	Vector<CString> titles;
	titles.push_back("Parallel");
	titles.push_back("Serial");

    CElapsedTimer t;

	Profiler::Profile("Task setup");

    t.start();
    Threads::ParallelFor(kern,data.size.area()/64,&data).get();
	timing.push_back(t.elapsed());
    t.start();
    for(uint64 i=0;i<data.size.area();i++)
    {
        data.value[i] /= 2;
    }
	timing.push_back(t.elapsed());

	Table::Header head;
	head.push_back("Workload type");
	head.push_back("Time score");

	Table::Table table;
	table.push_back(Table::GenColumn(titles.data(),titles.size()));
	table.push_back(Table::GenColumn(timing.data(),timing.size()));

	cBasicPrint("{0}",Table::GenTable(table,head));

    return true;
}

const constexpr CoffeeTest::Test _tests[1] = {
	{workload_test,"Threaded workload test"}
};

COFFEE_RUN_TESTS(_tests);
