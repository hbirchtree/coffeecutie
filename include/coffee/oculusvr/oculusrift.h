#pragma once

#include <coffee/graphics_apis/chmd_api.h>

namespace Coffee{
namespace OculusRift{

struct OculusVR : HMD::CHMD_Binding
{
    struct Context;

    struct Device : HMD::CHMD_Binding::Device
    {
        Device(uint32 idx, bool dontcare = false);

        void reset();

        CMat4 view(Eye) const;

        CVec3 angularVelocity() const;
        CVec3 velocity() const;

        bool isConnected() const;
    private:
        uint32 m_idx;
    };

    static bool InitializeBinding();
    static bool PollDevices(int32 *lastValidIndex = nullptr);
    static void Shutdown();

    static SWVersionInfo GetDriverInfo();
    static SWVersionInfo GetRuntimeInfo();

    static Device* GetDevice(int32 id);
    STATICINLINE Device* GetDefaultDevice()
    {
        return GetDevice(0);
    }
};

}
}