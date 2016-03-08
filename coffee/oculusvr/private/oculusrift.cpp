#include <coffee/oculusvr/oculusrift.h>

#include <coffee/core/CDebug>

/* Oculus VR headers */
#include <OVR_CAPI.h>
#include <OVR_Version.h>
/**/

namespace Coffee{
namespace OculusRift{

/* Log callback for the Oculus runtime */
void ovr_log_callback(int level, const char* msg)
{
    cLog(__FILE__,__LINE__,"OculusVR","{0}:{1}",level,msg);
}

const constexpr int32 OVR_Max_Devices = 8;
const constexpr int32 OVR_Num_Human_Eyes = 2;

struct OculusVR::Context
{
    Context()
    {
        cDebug("Creating Oculus context");
        for(szptr i=0;i<OVR_Max_Devices;i++)
            devices[i] = nullptr;
    }
    ~Context()
    {
        cDebug("Destroying Oculus context");
        for(szptr i=0;i<OVR_Max_Devices;i++)
            if(devices[i])
                ovrHmd_Destroy(devices[i]);
    }

    ovrInitParams iparams;

    ovrHmd devices[OVR_Max_Devices];
    ovrRenderAPIConfig apiconfig;
};

struct OculusVR::Device::ExtraData
{
    ovrDistortionMesh d_meshes[OVR_Num_Human_Eyes];
    ovrFovPort d_fov[OVR_Num_Human_Eyes];
    ovrEyeRenderDesc d_eyedesc[OVR_Num_Human_Eyes];

    ovrEyeType firstEye;
};

thread_local OculusVR::Context* OculusContext = nullptr;

static_assert(!ovrFalse,"Falsity of ovrFalse");
static_assert(ovrTrue,"Truth of ovrTrue");

bool OculusVR::InitializeBinding()
{

    ovrInitParams* fptr = 0;

    ovrInitParams flags = {};
    flags.LogCallback = ovr_log_callback;

#ifndef NDEBUG
    fptr = &flags;
#endif

    if(!ovr_Initialize(fptr))
    {
        return false;
    }

    OculusContext = new Context;
    OculusContext->iparams = flags;

    Context* ctxt = OculusContext;

    if(!PlatformData::IsMobile())
        ctxt->apiconfig.Header.API = ovrRenderAPI_OpenGL;
    else
        ctxt->apiconfig.Header.API = ovrRenderAPI_Android_GLES;


    if(!PollDevices(nullptr))
    {
        return false;
    }

    return true;
}

bool OculusVR::PollDevices(int32 *lastValidIndex)
{
    if(!OculusContext)
        return false;

    int count = ovrHmd_Detect();

    if(lastValidIndex)
        *lastValidIndex = count;

    return true;
}

void OculusVR::Shutdown()
{
    ovr_Shutdown();
}

SWVersionInfo OculusVR::GetDriverInfo()
{
    return SWVersionInfo("Oculus SDK",
                         OVR_PRODUCT_VERSION,
                         OVR_MAJOR_VERSION,
                         OVR_MINOR_VERSION,
                         OVR_PATCH_VERSION,
                         conversion::to_string(OVR_BUILD_NUMBER));
}

SWVersionInfo OculusVR::GetRuntimeInfo()
{
    return SWVersionInfo("Oculus Runtime",0,0);
}

OculusVR::Device* OculusVR::GetDevice(int32 id)
{
    if(id >= ovrHmd_Detect())
        return nullptr;
    OculusContext->devices[id] = ovrHmd_Create(id);
    return new Device(id);
}

OculusVR::Device::Device(uint32 idx, bool dontcare, scalar fov):
    HMD::CHMD_Binding::Device(
        OculusContext->devices[idx]->ProductName,
        cStringFormat("{0}.{1}",
                      OculusContext->devices[idx]->FirmwareMajor,
                      OculusContext->devices[idx]->FirmwareMinor),
        OculusContext->devices[idx]->Manufacturer,
        OculusContext->devices[idx]->SerialNumber),
    m_idx(idx),
    m_data(nullptr)
{
    ovrHmd dev = OculusContext->devices[m_idx];

    cDebug("Connecting to device: {0}",this->deviceInfo());

    uint32 devcaps = dev->HmdCaps;

    if(!(devcaps&ovrHmdCap_ExtendDesktop))
        cDebug("Something is up with the monitor!");

    uint32 s_caps = dev->TrackingCaps;
    uint32 r_caps = ovrTrackingCap_Orientation|ovrTrackingCap_Position;
    ovrHmd_ConfigureTracking(dev,s_caps,(dontcare) ? 0 : r_caps);

    ExtraData* data = new ExtraData;

    for(uint32 i=0;i<ovrEye_Count;i++)
    {
        data->d_fov[i] = dev->DefaultEyeFov[i];
        data->d_eyedesc[i] = ovrHmd_GetRenderDesc(dev,(ovrEyeType)i,data->d_fov[i]);
    }

    ovrDistortionCaps dcaps = ovrDistortionCap_TimeWarp;

    ovrHmd_ConfigureRendering(dev,&OculusContext->apiconfig,
                              dcaps,data->d_fov,
                              data->d_eyedesc);

    data->firstEye = dev->EyeRenderOrder[0];

    ovrBool m = ovrTrue;
    for(uint32 i=0;i<ovrEye_Count;i++)
        ovrBool m = m&ovrHmd_CreateDistortionMesh(
                    dev,(ovrEyeType)i,data->d_fov[i],dcaps,&data->d_meshes[i]);

    if(!m)
    {
        m_data = data;
    }else
        delete data;
}

SWVersionInfo OculusVR::Device::firmwareInfo() const
{
    ovrHmd dev = OculusContext->devices[m_idx];
    return SWVersionInfo(
                dev->Manufacturer,
                dev->FirmwareMajor,
                dev->FirmwareMinor);
}

HWDeviceInfo OculusVR::Device::deviceInfo() const
{
    ovrHmd dev = OculusContext->devices[m_idx];
    return HWDeviceInfo(
                dev->Manufacturer,
                dev->ProductName,
                cStringFormat("{0}",firmwareInfo()),
                dev->SerialNumber);
}

void OculusVR::Device::reset()
{
    ovrHmd_RecenterPose(OculusContext->devices[m_idx]);
}

void OculusVR::Device::startFrame()
{
    ovrHmd_BeginFrameTiming(OculusContext->devices[m_idx],0);
}

void OculusVR::Device::endFrame()
{
    ovrHmd_EndFrameTiming(OculusContext->devices[m_idx]);
}

CSize OculusVR::Device::resolution(Eye e, uint32 density) const
{
    ovrEyeType t;
    if(e==Eye::Left)
        t = ovrEye_Left;
    else if(e==Eye::Right)
        t = ovrEye_Right;
    ovrSizei sz = ovrHmd_GetFovTextureSize(
                OculusContext->devices[m_idx],
                t,m_data->d_fov[t],density);
    return CSize(sz.w,sz.h);
}

ZField OculusVR::Device::zfield() const
{
    ovrHmd dev = OculusContext->devices[m_idx];
    return ZField(dev->CameraFrustumNearZInMeters,
                  dev->CameraFrustumFarZInMeters);
}

FovDetail OculusVR::Device::fov() const
{
    ovrHmd dev = OculusContext->devices[m_idx];
    return FovDetail(dev->CameraFrustumHFovInRadians,
                     dev->CameraFrustumVFovInRadians);
}

BoundBox OculusVR::Device::viewerSpace() const
{
    ZField f = zfield();
    FovDetail a = fov();
    return BoundBox(a.h,a.v,f.far-f.near);
}

CMat4 OculusVR::Device::head() const
{
    ovrTrackingState t = ovrHmd_GetTrackingState(
                OculusContext->devices[m_idx],
                0);
    ovrVector3f p = t.HeadPose.ThePose.Position;
    ovrQuatf q = t.HeadPose.ThePose.Orientation;
    CMat4 m = translation(CMat4(),CVec3(p.x,p.y,p.z)*CVec3(6)*CVec3(1,1,1));
    m *= matrixify(CQuat(q.w,q.x*-1.0,q.y*-1.0,q.z));
    return m;
}

CMat4 OculusVR::Device::view(HMD::CHMD_Binding::Eye e) const
{
    ovrPosef m;
    ovrEyeType t;
    if(e==Eye::Left)
        t = ovrEye_Left;
    else
        t = ovrEye_Right;
    m = ovrHmd_GetHmdPosePerEye(OculusContext->devices[m_idx],t);
    CMat4 mt =
            translation(
                CMat4(),
                CVec3(m.Position.x,
                      m.Position.y,
                      m.Position.z)
                *CVec3(6));
    mt *= matrixify(CQuat(m.Orientation.w,
                          m.Orientation.x*-1.0,
                          m.Orientation.y*-1.0,
                          m.Orientation.z));
    return mt;
}

CVec3 OculusVR::Device::angularVelocity() const
{
    ovrTrackingState v = ovrHmd_GetTrackingState(OculusContext->devices[m_idx],0);
    return CVec3(v.HeadPose.AngularVelocity.x,
                 v.HeadPose.AngularVelocity.y,
                 v.HeadPose.AngularVelocity.z);
}

CVec3 OculusVR::Device::velocity() const
{
    ovrTrackingState v = ovrHmd_GetTrackingState(OculusContext->devices[m_idx],0);
    return CVec3(v.HeadPose.LinearVelocity.x,
                 v.HeadPose.LinearVelocity.y,
                 v.HeadPose.LinearVelocity.z);
}

bool OculusVR::Device::isConnected() const
{
    return OculusContext->devices[m_idx];
}

}
}
