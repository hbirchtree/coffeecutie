#include <coffee/COpenVR>
#include <coffee/COculusRift>
#include <coffee/core/CMD>

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
            {
                /* We do this to detect whether the default constructor
                 *  will poison the well, change pointers, close devices etc. */
                VR::Device* dev2 = VR::GetDefaultDevice();
                Profiler::Profile("Device acquisition");

                VR::PollDevices();
                Profiler::Profile("Device poll");

                cDebug("Your device: {0}",(const HWDeviceInfo&)*dev);

                while(true)
                {
                    CVec3 angvel = dev->angularVelocity();
                    Profiler::Profile("Pose acquisition");
                    CVec3 linvel = dev->velocity();
                    Profiler::Profile("Pose acquisition");
                    CMat4 t = dev->head();
                    Profiler::Profile("Pose acquisition");

                    cDebug("Values: angvel={0}, linvel={1}",angvel,linvel);
                    cDebug("Head transform: {0}",t);

                    Cmd::Wait();
                }
            }
        }
        cDebug("Shutting down VR systems");
        VR::Shutdown();
        Profiler::Profile("VR shutdown");
    }
    Profiler::PopContext();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
