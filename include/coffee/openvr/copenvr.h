#ifndef COFFEE_OPENVR_OPENVR_H
#define COFFEE_OPENVR_OPENVR_H

#include <coffee/core/base/cdebug.h>
#include <coffee/graphics_apis/chmd_api.h>

namespace Coffee{
namespace OpenVR{

struct Context;

struct OVRImpl : HMD::CHMD_Binding
{
    struct Context;

    struct Device : HMD::CHMD_Binding::Device
    {
        Device(uint32 index);

        bool updateState();

        CString make() const;
        CString firmware() const;
        CMat4 view(Eye e) const;

        CVec3 angularVelocity() const;
        CVec3 velocity() const;

        bool isConnected() const;
    private:
        uint32 m_dIndex;
    };

    static bool InitializeBinding();
    static bool PollDevices(int32 *lastValidIndex = nullptr);
    static void Shutdown();

    static SWVersionInfo GetDriverInfo();
    static SWVersionInfo GetRuntimeInfo();

    static Device *GetDevice(uint32 idx);
    STATICINLINE Device *GetDefaultDevice()
    {
        return GetDevice(0);
    }

    static const Context* GetConstContext();
    static Context* GetContext();
    static bool SetContext(Context* context);
};
}

using OpenVRDev = OpenVR::OVRImpl;

}

#endif
