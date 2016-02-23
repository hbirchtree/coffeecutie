#include <coffee/CCore>
#include <coffee/graphics_apis/CGLeam>
#include <coffee/COpenVR>
#include <coffee/COculusRift>
#include <coffee/CGraphics>

#include <coffee_ext/qt_shim/dialogs/dialogs.h>

using namespace Coffee;

using VR = OpenVRDev;
using OVR = OculusRift::OculusVR;

int32 coffee_main(int32, cstring_w*)
{
    Profiler::PushContext("OpenVR");
    if(!VR::InitializeBinding())
    {
        cDebug("Failed to initialize OpenVR device");
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
                cDebug("What you got: {0}",(const HWDeviceInfo&)*dev);
        }
        VR::Shutdown();
    }
    Profiler::PopContext();

    Profiler::PushContext("OculusVR");
    if(!OVR::InitializeBinding())
    {
        cDebug("Failed to initialize OculusVR device");
        return 1;
    }else{
        Profiler::Profile("VR init");

        int32 devs;
        if(!OVR::PollDevices(&devs))
        {
            cDebug("Failed to poll Oculus devices");
        }else{
            Profiler::Profile("Device poll");

            OVR::Device* dev = OVR::GetDefaultDevice();
            Profiler::Profile("Device acquisition");

            if(dev)
                cDebug("Your device: {0}",(const HWDeviceInfo&)*dev);
        }
        OVR::Shutdown();
    }
    Profiler::PopContext();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
