#include <coffee/CCore>
#include <coffee/CGraphics>
#include <coffee/CImage>

#include <coffee_ext/kinect_shim/cnect.h>

using namespace Coffee;
using namespace CoffeeExt;

using CNect = Freenect::FreenectImplementation;
using FContext = CNect::FContext;

int32 coffee_main(int32, cstring_w*)
{
    ShPtr<FContext> c;
    CNect::FreenectContext* context = nullptr;
    try{
        context = CNect::Alloc(0);
        c = ShPtr<FContext>(new FContext(context));
    }catch(std::runtime_error ex){
        cDebug("Failed to initialize Freenect: {0}",ex.what());
    }

    CNect::LaunchAsync(context);

    while(true)
    {
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
