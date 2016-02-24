#include <coffee/CCore>
#include <coffee/graphics_apis/CGLeam>
#include <coffee/COpenVR>
#include <coffee/COculusRift>
#include <coffee/CGraphics>

#include <coffee_ext/qt_shim/dialogs/dialogs.h>

using namespace Coffee;

using VR = OculusRift::OculusVR;
//using VR = OpenVR;

int32 coffee_main(int32, cstring_w*)
{
    cDebug("Testing with driver: {0}",VR::GetDriverInfo());

    Profiler::PushContext(VR::SystemName);
    if(!VR::InitializeBinding())
    {
        cDebug("Failed to initialize {0} device",VR::SystemName);
    }else{
        Profiler::Profile("VR init");


        cDebug("By the gods... So it was true! You must be the hero of Kvatch!");
        cDebug("[HERE BE DRAGONS]");
        int32 devs;
        if(!VR::PollDevices(&devs))
        {
            cDebug("I used to be a reality tripper like you,"
                   " until I took a cyberkick to the knee.");
        }else{
            Profiler::Profile("Device poll");

            VR::Device* dev = VR::GetDefaultDevice();
            Profiler::Profile("Device acquisition");

            if(dev)
                cDebug("Your device: {0}",(const HWDeviceInfo&)*dev);
        }
        VR::Shutdown();
    }
    Profiler::PopContext();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
