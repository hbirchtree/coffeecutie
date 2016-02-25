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

struct OculusVR::Context
{
    Context()
    {
        for(szptr i=0;i<OVR_Max_Devices;i++)
            devices[i] = nullptr;
    }
    ~Context()
    {
        for(szptr i=0;i<OVR_Max_Devices;i++)
            if(devices[i])
                ovrHmd_Destroy(devices[i]);
    }

    ovrInitParams iparams;

    ovrHmd devices[OVR_Max_Devices];
};

thread_local OculusVR::Context* OculusContext = nullptr;

bool OculusVR::InitializeBinding()
{
    static_assert(!ovrFalse,"Falsity of ovrFalse");
    static_assert(ovrTrue,"Truth of ovrTrue");

    cDebug("OculusVR runtime version: {0}.{1}.{2}.{3}.{4}",
           OVR_PRODUCT_VERSION,
           OVR_MAJOR_VERSION,
           OVR_MINOR_VERSION,
           OVR_PATCH_VERSION,
           OVR_BUILD_NUMBER);

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


    if(!PollDevices(nullptr))
    {
        return false;
    }

    cDebug("Oculus version string: {0}",ovr_GetVersionString());

    ovrHmd c = ovrHmd_Create(0);

    ovrTrackingState s = ovrHmd_GetTrackingState(c,0);
    cDebug("Accelerometer values: {0},{1},{2}",
           s.RawSensorData.Accelerometer.x,
           s.RawSensorData.Accelerometer.y,
           s.RawSensorData.Accelerometer.z);

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

OculusVR::Device::Device(uint32 idx, bool dontcare):
    HMD::CHMD_Binding::Device(
        OculusContext->devices[idx]->ProductName,
        cStringFormat("{0}.{1}",
                      OculusContext->devices[idx]->FirmwareMajor,
                      OculusContext->devices[idx]->FirmwareMinor),
        OculusContext->devices[idx]->Manufacturer),
    m_idx(idx)
{
    uint32 s_caps =
            ovrTrackingCap_MagYawCorrection;
    uint32 r_caps =
            ovrTrackingCap_Orientation|
            ovrTrackingCap_Position;
    ovrHmd_ConfigureTracking(OculusContext->devices[m_idx],
                             s_caps,(dontcare) ? 0 : r_caps);
}

SWVersionInfo OculusVR::Device::GetFirmwareInfo()
{
    ovrHmd dev = OculusContext->devices[m_idx];
    return SWVersionInfo(dev->ProductName,
                         dev->FirmwareMajor,
                         dev->FirmwareMinor);
}

void OculusVR::Device::reset()
{
    ovrHmd_RecenterPose(OculusContext->devices[m_idx]);
}

bool OculusVR::Device::isConnected() const
{
    return OculusContext->devices[m_idx];
}

}
}
