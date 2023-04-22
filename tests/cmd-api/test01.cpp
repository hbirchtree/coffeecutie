#include <coffee/core/CUnitTesting>
#include <coffee/core/CMD>

using namespace Coffee;

bool altscreen_test()
{
    Cmd::AltScreen();

    cDebug("This should appear in an alternate buffer");
    cDebug("Press any key to proceed");

    Cmd::Wait();

    Cmd::ResetScreen();
    return true;
}

bool input_test()
{
    /* Testing interactive command line */
    std::string ts;
    ts.resize(100);

    while(!StrCmp(&ts[0],"quit\n"))
    {
        ts.clear();
        cBasicPrintNoNL("Type something: ");
        Cmd::ReadString(&ts[0],99,stdin);
        cDebug("You wrote: {0}",ts.c_str());
    }
    return true;
}

const constexpr CoffeeTest::Test _tests[2] = {
    {altscreen_test},
    {input_test}
};

COFFEE_RUN_TESTS(_tests);
