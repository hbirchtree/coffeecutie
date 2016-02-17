#include <coffee/CCore>
#include <coffee/graphics_apis/CGLeam>
#include <coffee/COpenVR>
#include <coffee/CGraphics>

#include <coffee_ext/qt_shim/dialogs/dialogs.h>

using namespace Coffee;

int32 coffee_main(int32, cstring_w*)
{
    Profiler::PushContext("Root");

    if(!OpenVRDev::InitializeBinding())
    {
        cDebug("Move on, there is nothing to see here.");
    }else{
        cDebug("By the gods... So it was true! You must be the hero of Kvatch!");
        cDebug("[HERE BE DRAGONS]");
        uint32 devs;
        if(!OpenVRDev::PollDevices(&devs))
        {
            cDebug("I used to be a reality tripper like you,"
                   " until I took a cyberkick to the knee.");
        }
        OpenVRDev::OVRDevice* dev = OpenVRDev::GetDevice(0);
        cDebug("What you got: {0}",(const HWDeviceInfo&)*dev);
    }

    Profiler::PopContext();

    for(Profiler::DataPoint const& p : Profiler::datapoints)
    {
        if(p.tp==Profiler::DataPoint::Profile)
            cDebug("Time: {0}, label: {1}",p.ts,p.name);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
