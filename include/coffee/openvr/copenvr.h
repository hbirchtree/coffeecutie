#ifndef COFFEE_OPENVR_OPENVR_H
#define COFFEE_OPENVR_OPENVR_H

#include "ovrdevice.h"
#include "ovractor.h"

namespace Coffee{
namespace OpenVRFun{

struct OVRImpl : HeadDisplayDriver_def
{
    using Device = OVRDevice;
    using Context = OVRContext;
    using Actor = OVRActor;

    static const constexpr cstring SystemName = "OpenVR";

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

using OpenVR = OpenVRFun::OVRImpl;

}

#endif
