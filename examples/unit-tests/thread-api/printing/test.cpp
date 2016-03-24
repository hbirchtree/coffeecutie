#include <coffee/core/CUnitTesting>
#include <coffee/core/CDebug>
#include <coffee/core/CThreading>

using namespace Coffee;

bool mutex_locking()
{
    cDebug("Testing multithread + printing, should be mutex-locked");
    cDebug("User must verify output");
    Threads::Function<void()> message_test = [](){
        int64 msgs = 0;
        ThreadId thread;

        while(msgs<10)
        {
            cDebug("{0}: Spam! Messages! Yes!",(const void*)thread.hash());
            msgs++;
        }
    };

    auto t1 = Threads::RunAsync(message_test);
    auto t2 = Threads::RunAsync(message_test);

    t1.get();
    t2.get();

    return true;
}

static const constexpr CoffeeTest::Test _tests[2] = {
    mutex_locking
};

COFFEE_RUN_TESTS(_tests);