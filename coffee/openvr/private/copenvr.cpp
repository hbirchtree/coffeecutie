#include <coffee/openvr/copenvr.h>
#include <openvr.h>

#include <coffee/core/coffee_strings.h>

namespace Coffee{
namespace OpenVR{

struct OVRImpl::Context
{
    vr::TrackedDevicePose_t devicePoses[vr::k_unMaxTrackedDeviceCount];

    vr::IVRSystem* m_HMD;
    vr::IVRRenderModels* m_RModels;
};

thread_local OVRImpl::Context *m_Context = nullptr;

CString VRGetTrackedDevString(vr::TrackedDeviceIndex_t index,
                              vr::ETrackedDeviceProperty prop,
                              vr::ETrackedPropertyError* err_p = NULL)
{
    if(!m_Context)
        return "";

    vr::IVRSystem* sys = m_Context->m_HMD;

    uint32 size = sys->GetStringTrackedDeviceProperty(index,prop,NULL,0,err_p);
    if(size == 0)
        return "";
    cstring_w c_string = new sbyte_t[size];
    sys->GetStringTrackedDeviceProperty(index,prop,c_string,size,err_p);
    CString out = c_string;
    delete[] c_string;
    return out;
}

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

//void OVRImpl::SubmitTextures(vr::Texture_t *left, vr::Texture_t *right)
//{
//    vr::VRCompositor()->Submit(vr::Eye_Left,left);
//    vr::VRCompositor()->Submit(vr::Eye_Right,right);
//}

void OVRImpl::Shutdown()
{
    vr::VR_Shutdown();
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

bool OVRImpl::SetContext(Context *context)
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

OVRImpl::Device::Device(uint32 index):
    HMD::CHMD_Binding::Device(
        VRGetTrackedDevString(index,vr::Prop_TrackingSystemName_String),
        VRGetTrackedDevString(index,vr::Prop_TrackingFirmwareVersion_String)),
    m_dIndex(index)
{
}

CString OVRImpl::Device::make() const
{
    if(!m_Context)
        return "";
    return VRGetTrackedDevString(vr::k_unTrackedDeviceIndex_Hmd+m_dIndex,
                                 vr::Prop_TrackingSystemName_String);
}

CString OVRImpl::Device::firmware() const
{
    if(!m_Context)
        return "";
    return VRGetTrackedDevString(vr::k_unTrackedDeviceIndex_Hmd+m_dIndex,
                                 vr::Prop_TrackingFirmwareVersion_String);
}

CMat4 OVRImpl::Device::view(HMD::CHMD_Binding::Eye e) const
{
    if(!m_Context||!isConnected())
        return CMat4();
    const vr::HmdMatrix34_t& pose = m_Context->devicePoses[m_dIndex].mDeviceToAbsoluteTracking;
    CMat4 mat;

    for(size_t i=0;i<CMat4::col_type::size;i++)
        for(size_t j=0;j<3;j++)
            mat[i][j] = pose.m[j][i];

    for(size_t i=0;i<3;i++)
        mat[3][i] = 0.0f;
    mat[3][3] = 1.0f;

    return mat;
}

CVec3 OVRImpl::Device::angularVelocity() const
{
    if(!m_Context||!isConnected())
        return CVec3();
    const vr::HmdVector3_t& angle = m_Context->devicePoses[m_dIndex].vAngularVelocity;

    return CVec3(angle.v[0],angle.v[1],angle.v[2]);
}

CVec3 OVRImpl::Device::velocity() const
{
    if(!m_Context||!isConnected())
        return CVec3();
    const vr::HmdVector3_t& vel = m_Context->devicePoses[m_dIndex].vVelocity;

    return CVec3(vel.v[0],vel.v[1],vel.v[2]);
}

bool OVRImpl::Device::isConnected() const
{
    if(!m_Context)
        return false;
    return m_Context->m_HMD->IsTrackedDeviceConnected(m_dIndex);
}

}
}
