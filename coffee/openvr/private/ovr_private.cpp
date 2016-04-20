#include "ovr_private.h"

namespace Coffee{
namespace OpenVRFun{

thread_local OVRImpl::Context *m_Context = nullptr;

CString VRGetTrackedDevString(vr::TrackedDeviceIndex_t index,
                              vr::ETrackedDeviceProperty prop,
                              vr::ETrackedPropertyError* err_p)
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

}
}
