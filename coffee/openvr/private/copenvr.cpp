#include <coffee/openvr/copenvr.h>

#include <coffee/core/CDebug>
#include "ovr_private.h"

#include <coffee/core/coffee_strings.h>
#include <coffee/openvr/ovrdevice.h>

#include <coffee/graphics_apis/gleam/rhi/gleam_vertex_rhi.h>
#include <coffee/graphics_apis/gleam/rhi/gleam_buffer_rhi.h>

namespace Coffee{
namespace OpenVRFun{

bool LoadRenderModels(OVRImpl::Context* ctxt)
{
    if(!ctxt->context.VRRenderModels()->GetRenderModelCount())
        return false;
    return true;
}

bool OVRImpl::InitializeBinding()
{
    vr::EVRInitError err = vr::VRInitError_None;

    vr::IVRSystem* sys = vr::VR_Init(&err,vr::VRApplication_Overlay);

    if(err != vr::VRInitError_None)
        return false;

    m_Context = new OVRImpl::Context;
    m_Context->ivrsys = sys;

    {
        CString v1 = VRGetTrackedDevString(vr::k_unTrackedDeviceIndex_Hmd,
                                           vr::Prop_TrackingSystemName_String,
                                           nullptr);
        CString v2 = VRGetTrackedDevString(vr::k_unTrackedDeviceIndex_Hmd,
                                           vr::Prop_TrackingFirmwareVersion_String,
                                           nullptr);
        SWVersionInfo d(v1,1,0,0,0,v2);
        MemCpy(&m_Context->m_driver,&d,sizeof(d));
    }
    {
        CString v1 = VRGetTrackedDevString(vr::k_unTrackedDeviceIndex_Hmd,
                                           vr::Prop_ManufacturerName_String,
                                           nullptr);
        CString v2 = VRGetTrackedDevString(vr::k_unTrackedDeviceIndex_Hmd,
                                           vr::Prop_SerialNumber_String,
                                           nullptr);
        HWDeviceInfo d(v1,v2);
        MemCpy(&m_Context->m_unit,&d,sizeof(d));
    }

    return true;
}

bool OVRImpl::PollDevices(int32* lastValidIndex)
{
    bool stat = m_Context->context.VRSystem();
    if(lastValidIndex)
        *lastValidIndex = stat;
    if(stat)
        m_Context->m_devices[0] = new OVRDevice(0);
    return stat;
}

void OVRImpl::PollEvents()
{
    vr::VREvent_t ev;
    while(m_Context->ivrsys->PollNextEvent(&ev,sizeof(ev)))
    {
        switch(ev.eventType)
        {
        case vr::VREvent_TrackedDeviceActivated:
        {

            break;
        }
        }
    }
}

void OVRImpl::Shutdown()
{
    vr::VR_Shutdown();
}

SWVersionInfo OVRImpl::GetDriverInfo()
{
    return SWVersionInfo("OpenVR",1,0);
}

SWVersionInfo OVRImpl::GetRuntimeInfo()
{
    return m_Context->m_driver;
}

OVRImpl::Device *OVRImpl::GetDevice(uint32 idx)
{
    return m_Context->m_devices[idx];
}

const OVRImpl::Context *OVRImpl::GetConstContext()
{
    return m_Context;
}

OVRImpl::Context *OVRImpl::GetContext()
{
    OVRImpl::Context* ctxt = m_Context;
    m_Context = nullptr;
    return ctxt;
}

bool OVRImpl::SetContext(OVRImpl::Context *context)
{
    return m_Context = context;
}

OVRImpl::RModel *OVRImpl::GetRenderModel(OVRImpl::Device *dev)
{
    C_UNUSED(dev);


}

}
}
