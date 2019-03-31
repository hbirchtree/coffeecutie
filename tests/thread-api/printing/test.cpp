#include <coffee/core/CThreading>
#include <coffee/strings/libc_types.h>
#include <peripherals/libc/output_ops.h>

#include <coffee/core/CUnitTesting>

using namespace Coffee;

bool mutex_locking()
{
    Function<void()> message_test = []() {
        i64      msgs = 0;
        ThreadId thread;

        while(msgs < 10)
        {
            cDebug("{0}: Spam! Messages! Yes!", C_RCAST<c_cptr>(thread.hash()));
            msgs++;
        }
    };

    libc::io::terminal::buffers::swap();

    auto t1 = threads::RunAsync(message_test);
    auto t2 = threads::RunAsync(message_test);
    t1.get();
    t2.get();

    libc::io::terminal::buffers::reset();

    return true;
}

COFFEE_TESTS_BEGIN(1)

    {mutex_locking, "Locked printing"}

COFFEE_TESTS_END()
