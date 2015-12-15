#include <coffee/CCore>
#include <coffee/CFFMedia>

using namespace Coffee;

int32 coffee_main(int32, byte_t**)
{
    CFFMedia::coffee_ffmedia_init();
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
