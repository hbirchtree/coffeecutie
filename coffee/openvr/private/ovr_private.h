#pragma once

#include <coffee/core/CDebug>

#include <coffee/openvr/openvr_common.h>
#include <coffee/openvr/copenvr.h>

#include <openvr.h>

namespace Coffee{
namespace OpenVRFun{

struct OVRContext
{
    vr::TrackedDevicePose_t devicePoses[vr::k_unMaxTrackedDeviceCount];

    vr::IVRSystem* m_HMD;
    vr::IVRRenderModels* m_RModels;
};

thread_local extern OVRImpl::Context *m_Context;

extern CString VRGetTrackedDevString(vr::TrackedDeviceIndex_t index,
                                     vr::ETrackedDeviceProperty prop,
                                     vr::ETrackedPropertyError* err_p = nullptr);

}
}
