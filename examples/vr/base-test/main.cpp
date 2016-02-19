#include <coffee/CCore>
#include <coffee/graphics_apis/CGLeam>
#include <coffee/COpenVR>
#include <coffee/CGraphics>

#include <coffee_ext/qt_shim/dialogs/dialogs.h>

using namespace Coffee;

int32 coffee_main(int32, cstring_w*)
{
    if(!OpenVRDev::InitializeBinding())
    {
        cDebug("Move on, there is nothing to see here.");
    }else{
        Profiler::Profile("VR init");

        cDebug("By the gods... So it was true! You must be the hero of Kvatch!");
        cDebug("[HERE BE DRAGONS]");
        uint32 devs;
        if(!OpenVRDev::PollDevices(&devs))
        {
            cDebug("I used to be a reality tripper like you,"
                   " until I took a cyberkick to the knee.");
        }
        Profiler::Profile("Device poll");

        OpenVRDev::OVRDevice* dev = OpenVRDev::GetDevice(0);
        Profiler::Profile("Device acquisition");

        cDebug("What you got: {0}",(const HWDeviceInfo&)*dev);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
