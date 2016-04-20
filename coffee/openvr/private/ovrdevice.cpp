#include <coffee/openvr/ovrdevice.h>
#include <coffee/openvr/ovractor.h>

#include "ovr_private.h"

namespace Coffee{
namespace OpenVRFun{

OVRDevice::OVRDevice(uint32 index):
    HeadDevice(
        VRGetTrackedDevString(index,vr::Prop_TrackingSystemName_String),
        VRGetTrackedDevString(index,vr::Prop_TrackingFirmwareVersion_String)),
    m_actor(new ActorTracker(this)),
    m_dIndex(index)
{
}

SWVersionInfo OVRDevice::firmwareInfo() const
{
    return SWVersionInfo("OpenVR",1,0);
}

HWDeviceInfo OVRDevice::deviceInfo() const
{
    CString make = VRGetTrackedDevString(vr::k_unTrackedDeviceIndex_Hmd+m_dIndex,
                                         vr::Prop_TrackingSystemName_String);
    CString firm = VRGetTrackedDevString(vr::k_unTrackedDeviceIndex_Hmd+m_dIndex,
                                         vr::Prop_TrackingFirmwareVersion_String);
    return HWDeviceInfo("Generic",make,firm);
}

CRect OVRDevice::windowCanvas() const
{
    return CRect();
}

void OVRDevice::startFrame()
{

}

void OVRDevice::endFrame()
{

}

void OVRDevice::reset()
{

}

bool OVRDevice::isConnected() const
{
    return m_Context->m_HMD->IsTrackedDeviceConnected(m_dIndex);
}

OVRDevice::ActorTracker &OVRDevice::actor()
{
    return *m_actor;
}

const LensDetails *OVRDevice::getLenses()
{
    return m_lenses;
}

}
}
