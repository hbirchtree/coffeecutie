#include <coffee/CCore>
#include <coffee/CGraphics>
#include <coffee/CImage>

#include <coffee_ext/kinect_shim/cnect.h>

using namespace Coffee;
using namespace CoffeeExt;

using CNect = Freenect::FreenectImplementation;
using FContext = CNect::FContext;

void frame_fun(CNect::NectRGB const&,CNect::NectDepth const&)
{
}

int32 coffee_main(int32, cstring_w*)
{
    ShPtr<FContext> c;
    try{
        c = ShPtr<FContext>(0);
    }catch(std::runtime_error ex){
        cDebug("Failed to initialize Freenect: {0}",ex.what());
    }

    if(!c->context())
        return 1;

    CNect::LaunchAsync(c->context());

    while(true)
    {
        CNect::ProcessFrame(c->context(),frame_fun,0);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
