#include <coffee/openvr/copenvr.h>

#include <coffee/core/CDebug>
#include "ovr_private.h"

#include <coffee/core/coffee_strings.h>
#include <coffee/openvr/ovrdevice.h>

namespace Coffee{
namespace OpenVRFun{

bool OVRImpl::InitializeBinding()
{
    vr::EVRInitError err = vr::VRInitError_None;

    vr::IVRSystem* sys = vr::VR_Init(&err,vr::VRApplication_Scene);

    if(err != vr::VRInitError_None)
    {
        cLog(__FILE__,__LINE__,CFStrings::Graphics_VR_Library_Name,
             CFStrings::Graphics_VR_Library_InitError,
             vr::VR_GetVRInitErrorAsEnglishDescription(err));
        return false;
    }

    vr::IVRRenderModels* models =
            (vr::IVRRenderModels*)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version,&err);

    if(!models)
    {
        cLog(__FILE__,__LINE__,CFStrings::Graphics_VR_Library_Name,
             CFStrings::Graphics_VR_Library_InitError,
             vr::VR_GetVRInitErrorAsEnglishDescription(err));
        Shutdown();
        return false;
    }

    if(!vr::VRCompositor())
    {
        Shutdown();
        cLog(__FILE__,__LINE__,CFStrings::Graphics_VR_Library_Name,
             CFStrings::Graphics_VR_Library_CompositeError);
    }

    m_Context = new Context;
    m_Context->m_HMD = sys;
    m_Context->m_RModels = models;

    return true;
}

bool OVRImpl::PollDevices(int32* lastValidIndex)
{
    if(lastValidIndex)
        *lastValidIndex = -1;
    if(!m_Context)
        return false;
    return false;
    vr::VRCompositor()->WaitGetPoses(m_Context->devicePoses,
                                     vr::k_unMaxTrackedDeviceCount,
                                     nullptr,0);
    if(lastValidIndex)
    {
        *lastValidIndex = -1;
        for(uint32 i=0;i<vr::k_unMaxTrackedDeviceCount;++i)
            if(m_Context->devicePoses[i].bPoseIsValid)
            {
                *lastValidIndex = i;
            }else
                break;
    }

    return true;
}

void OVRImpl::Shutdown()
{
    vr::VR_Shutdown();
}

SWVersionInfo OVRImpl::GetDriverInfo()
{
    return SWVersionInfo("SteamVR",1,0);
}

SWVersionInfo OVRImpl::GetRuntimeInfo()
{
    return SWVersionInfo("OpenVR",1,0);
}

OVRImpl::Device *OVRImpl::GetDevice(uint32 idx)
{
    return new Device(idx);
}

const OVRImpl::Context *OVRImpl::GetConstContext()
{
    return m_Context;
}

OVRImpl::Context *OVRImpl::GetContext()
{
    Context* ptr = m_Context;
    m_Context = nullptr;
    return ptr;
}

bool OVRImpl::SetContext(OVRImpl::Context *context)
{
    if(!m_Context)
    {
        m_Context = context;
        return true;
    }
    else
    {
        cLog(__FILE__,__LINE__,CFStrings::Graphics_VR_Library_Name,
             CFStrings::Graphics_VR_Library_CurrencyError);
        return false;
    }
}

}
}
