#pragma once

#include <coffee/core/CDebug>

#include <coffee/openvr/openvr_common.h>
#include <coffee/openvr/copenvr.h>

#include <openvr.h>

namespace Coffee{
namespace OpenVRFun{

struct OVRContext
{
    OVRContext():
        m_driver("None",0,0),
        m_unit("None","Undefined")
    {
    }

    vr::TrackedDevicePose_t devicePoses[vr::k_unMaxTrackedDeviceCount];

    vr::COpenVRContext context;

    vr::IVRSystem* ivrsys;

    SWVersionInfo m_driver;
    HWDeviceInfo m_unit;

    Map<uint32,OVRDevice*> m_devices;

    vr::TrackedDevicePose_t m_DevicePose[vr::k_unMaxTrackedDeviceCount];
    bool m_DeviceEnabled[vr::k_unMaxTrackedDeviceCount];
};

thread_local extern OVRImpl::Context *m_Context;

extern CString VRGetTrackedDevString(vr::TrackedDeviceIndex_t index,
                                     vr::ETrackedDeviceProperty prop,
                                     vr::ETrackedPropertyError* err_p = nullptr);

}
}
