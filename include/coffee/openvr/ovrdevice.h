#pragma once

#include "openvr_common.h"

namespace Coffee{
namespace OpenVRFun{

struct OVRDevice : HeadDisplayDriver_def::Device
{
    friend struct OVRActor;

    using ActorTracker = OVRActor;

private:
    LensDetails m_lenses[2];
    ActorTracker *m_actor;
    uint32 m_dIndex;

public:
    OVRDevice(uint32 index);
    ~OVRDevice();

    C_DELETE_COPY_CONSTRUCTOR(OVRDevice);

    SWVersionInfo firmwareInfo() const;
    HWDeviceInfo deviceInfo() const;

    CRect windowCanvas() const;

    void startFrame();
    void endFrame();

    void reset();

    bool isConnected() const;

    ActorTracker& actor();
    LensDetails const* getLenses();
};

}
}
