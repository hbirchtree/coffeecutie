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
    data.size.w = 10240;
    data.size.h = 10240;
    data.value = (uint8*)Alloc(data.size.area());

    Threads::Function<void(uint64,DataSet*)> kern = [](uint64 i, DataSet* d)
    {
        for(int32 j=0;j<64;j++)
            d->value[i*64+j] /= 2;
    };

    CElapsedTimer t;
    t.start();
    Threads::ParallelFor(kern,data.size.area()/64,&data).get();
    cDebug("Parallel time: {0}",t.elapsed());
    t.start();
    for(uint64 i=0;i<data.size.area();i++)
    {
        data.value[i] /= 2;
    }
    cDebug("Serial time: {0}",t.elapsed());
    return true;
}

static const constexpr CoffeeTest::Test _tests[1] = {
    workload_test
};

COFFEE_RUN_TESTS(_tests);