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
    for(const Profiler::DataPoint& p : *Profiler::datapoints)
    {
        cDebug("Type: {0}, name: {1}, time: {2}",p.tp,p.name,p.ts-base_timestamp);
    }
}

void frame_fun(CNect::NectRGB const& c,CNect::NectDepth const& d,
               CNect::NectCloud const& cloud)
{
    CResources::CResource cl_raw("cloudframe.raw");

    CResources::CResource d_raw("dframe.raw");
    CResources::CResource c_raw("cframe.raw");

    cl_raw.data = (void*)&cloud[0];
    cl_raw.size = cloud.size()*sizeof(cloud[0]);

    d_raw.data = (void*)&d.data()[0];
    d_raw.size = d.size.area()*sizeof(scalar);
    c_raw.data = (void*)&c.data()[0];
    c_raw.size = c.size.area()*sizeof(CRGBA);

    CResources::FileCommit(cl_raw);
    CResources::FileCommit(d_raw);
    CResources::FileCommit(c_raw);

    CResources::CResource outfile2("outdata_color.png");

    CStbImageLib::CStbImageConst img;
    img.data = (const byte_t*)c.data();
    img.size = c.size;
    img.bpp = 4;

    CStbImageLib::SavePNG(&outfile2,&img);
    CResources::FileCommit(outfile2,false);
    CResources::FileFree(outfile2);

    CResources::CResource outfile("outdata.png");

    CStbImageLib::CStbImageConst img2;
    CRGBA* fval = (CRGBA*)CCalloc(1,d.size.area()*sizeof(CRGBA));
    img2.data = (byte_t*)fval;
    img2.size.w = d.size.w;
    img2.size.h = d.size.h;
    img2.bpp = 4;

    for(uint32 i=0;i<d.size.area();i++)
    {
        const scalar& vf = d.data()[i];
        uint8 v = (vf/4000.0)*255;
        fval[i].r = v;
        fval[i].a = 255;
    }

    CStbImageLib::SavePNG(&outfile,&img2);
    CResources::FileCommit(outfile,false);
    CResources::FileFree(outfile);

    CFree(fval);
}

int32 coffee_main(int32, cstring_w*)
{
    base_timestamp = Time::CurrentMicroTimestamp();

    SetExitFunction(d_exit_handle);
    Profiler::Profile("Set exit handle");

    ShPtr<FContext> c;
    try{
        c = ShPtr<FContext>(new FContext(0));
    }catch(std::runtime_error ex){
        cDebug("Failed to initialize Freenect: {0}",ex.what());
    }
    Profiler::Profile("Create context");

    if(!c)
        return 1;

    CNect::LaunchAsync(c->context());
    Profiler::Profile("Launch task");

    while(true)
    {
        CNect::ProcessFrame(c->context(),frame_fun,0);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
