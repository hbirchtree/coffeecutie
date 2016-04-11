#pragma once

#include <coffee/interfaces/chmd_api.h>

namespace Coffee{
namespace HMD{

struct DummyPlugHMD : CHMD_Binding
{
    static const constexpr cstring SystemName = "DummyPlug";

    struct Device : CHMD_Binding::Device
    {
        Device():
            CHMD_Binding::Device("Dummyplug","1.0")
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

        CSize resolution(Eye, uint32 density) const
        {
            CSize m(960,1080);
            m *= density;
            return m;
        }
        CRect windowPos() const
        {
            return CRect(0,0,1920,1080);
        }

        ZField zfield() const
        {
            return ZField();
        }

        FovDetail fov() const
        {
            return FovDetail(90,90);
        }

        BoundBox viewerSpace() const
        {
            ZField z;
            FovDetail f;
            return BoundBox(f.h,f.v,z.far-z.near,true);
        }

        CMat4 head() const
        {
            return CMat4();
        }

        CMat4 view(Eye) const
        {
            return CMat4();
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
