#ifndef COFFEE_OPENVR_OPENVR_H
#define COFFEE_OPENVR_OPENVR_H

#include <coffee/graphics_apis/include/chmd_api.h>
#include <openvr.h>

namespace Coffee{
namespace OpenVR{

struct OVRImpl : HMD::CHMD_Binding{
    struct OVRDevice
    {
        CString identifier() const;
        CMat4 view(Eye e) const;

        CVec3 radianVelocity() const;
        CVec3 velocity() const;

        bool isConnected() const;
    };

    static bool InitializeBinding();
    static bool InitializeHMD(Device* dev);

    static void Shutdown();

    static Device* GetDevice(int32 id);
    static Device* GetDefaultDevice();
};

}
}

#endif
