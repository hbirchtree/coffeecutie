#include <coffee/core/CUnitTesting>
#include <coffee/core/CDebug>
#include <coffee/core/CMD>
#include <coffee/core/CThreading>

using namespace Coffee;

bool mutex_locking()
{
    Function<void()> message_test = [](){
        i64 msgs = 0;
        ThreadId thread;

        while(msgs<10)
        {
            cDebug("{0}: Spam! Messages! Yes!",(const void*)thread.hash());
            msgs++;
        }
    };


    Cmd::AltScreen();

    auto t1 = Threads::RunAsync(message_test);
    auto t2 = Threads::RunAsync(message_test);
    t1.get();
    t2.get();

    Cmd::ResetScreen();

    return true;
}

const constexpr CoffeeTest::Test _tests[1] = {
    {mutex_locking,"Locked printing"}
};

COFFEE_RUN_TESTS(_tests);
