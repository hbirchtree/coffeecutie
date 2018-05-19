#pragma once

#include <coffee/interfaces/chmd_api.h>

namespace Coffee{
namespace HMD{

struct DummyPlugHMD : HeadDisplayDriver_def
{
    static const constexpr cstring SystemName = "DummyPlug";

    struct Device : HeadDisplayDriver_def::Device
    {
        Device():
            HeadDisplayDriver_def::Device("Dummyplug","1.0")
        {
        }

        SWVersionInfo firmwareInfo() const
        {
            return SWVersionInfo("DummyFW",1,0);
        }
        HWDeviceInfo deviceInfo() const
        {
            return HWDeviceInfo("DummyPlug","0x0");
        }

        void reset()
        {
        }

        void startFrame()
        {
        }
        void endFrame()
        {
        }

        CVec3 velocity() const
        {
            return CVec3();
        }
        CVec3 angularVelocity() const
        {
            return CVec3();
        }
        bool isConnected() const
        {
            return true;
        }
    };

    static bool InitializeBinding()
    {
        return true;
    }
    static bool PollDevices(int32* count = nullptr)
    {
        if(count)
            *count = 1;
        return true;
    }
    static void Shutdown()
    {
    }
    static SWVersionInfo GetDriverInfo()
    {
        return SWVersionInfo("DummyDriver",1,0);
    }
    static SWVersionInfo GetRuntimeInfo()
    {
        return SWVersionInfo("DummyRuntime",1,0);
    }
    static Device* GetDevice(int32 id)
    {
        if(id==0)
            return new Device();
        else
            return nullptr;
    }
    static Device* GetDefaultDevice()
    {
        return GetDevice(0);
    }
    static const Context* GetConstContext()
    {
        return nullptr;
    }
    static Context* GetContext()
    {
        return nullptr;
    }
    static bool SetContext(Context* context)
    {
        return true;
    }
    static bool IsDummy()
    {
        return true;
    }
};

}
}
