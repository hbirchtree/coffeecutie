#include <coffee/CCore>
#include <coffee/CGraphics>
#include <coffee/CImage>

#include <coffee_ext/kinect_shim/cnect.h>

using namespace Coffee;
using namespace CoffeeExt;

using CNect = Freenect::FreenectImplementation;
using FContext = CNect::FContext;

uint64 base_timestamp = 0;

void d_exit_handle()
{
    for(const Profiler::DataPoint& p : Profiler::datapoints)
    {
        cDebug("Type: {0}, name: {1}, time: {2}",p.tp,p.name,p.ts-base_timestamp);
    }
}

void frame_fun(CNect::NectRGB const& c,CNect::NectDepth const& d)
{
    return;

    CStbImageLib::CStbImageConst img;
    img.data = (const byte_t*)c.data();
    img.size = c.size;
    img.bpp = 4;

    cDebug("Frame received");

    CResources::CResource outfile("outdata.png");

    CStbImageLib::SavePNG(&outfile,&img);
//    CResources::FileCommit(outfile,false);
    CResources::FileFree(outfile);

    CStbImageLib::CStbImage img2;
    uint8* fval = (uint8*)Alloc(d.size.area()*sizeof(uint8));
    img2.data = (byte_t*)fval;
    img2.size.w = d.size.w;
    img2.size.h = d.size.h;
    img.bpp = 1;

    for(uint32 i=0;i<d.size.area();i++)
    {
        fval[i] = (uint8)(d.data()[i]/4000);
    }

    CStbImageLib::SavePNG(&outfile,&img2);
//    CResources::FileCommit(outfile,false);
    CResources::FileFree(outfile);

    CFree(fval);
}

int32 coffee_main(int32, cstring_w*)
{
    base_timestamp = Time::CurrentMicroTimestamp();
    SetExitFunction(d_exit_handle);

    ShPtr<FContext> c;
    try{
        c = ShPtr<FContext>(new FContext(0));
    }catch(std::runtime_error ex){
        cDebug("Failed to initialize Freenect: {0}",ex.what());
    }

    if(!c)
        return 1;

    CNect::LaunchAsync(c->context());

    uint64 tmp;
    while(true)
    {
        tmp = Time::CurrentMicroTimestamp();
        CNect::ProcessFrame(c->context(),frame_fun,0);
        cDebug("Processtime : {0}",Time::CurrentMicroTimestamp()-tmp);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
