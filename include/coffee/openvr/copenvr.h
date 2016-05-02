#ifndef COFFEE_OPENVR_OPENVR_H
#define COFFEE_OPENVR_OPENVR_H

#include <coffee/graphics_apis/gleam/rhi/gleam_rhi.h>

#include "ovrdevice.h"
#include "ovractor.h"

namespace Coffee{
namespace OpenVRFun{

using GLMDesc = GLEAMAPI::V_DESC;
using GLMBuffer = GLEAMAPI::BUF_A;

struct OVRRenderModel
{
    OVRRenderModel():
        descriptor(nullptr)
    {
        MemClear(buffers,sizeof(buffers[0])*Num_Buffers);
    }

    enum BufferDesc
    {
        Position = 0,
        Normal = 1,
        TexCoord = 2,

        Num_Buffers = 3,
    };

    GLMDesc* descriptor;
    GLMBuffer* buffers[Num_Buffers];
};

struct OVRImpl : HeadDisplayDriver_def
{
    using Device = OVRDevice;
    using Context = OVRContext;
    using Actor = OVRActor;
    using Controller = OVRController;

    using RModel = OVRRenderModel;

    static const constexpr cstring SystemName = "OpenVR";

    static bool InitializeBinding();
    static bool PollDevices(int32 *lastValidIndex = nullptr);
    static void PollEvents();
    static void Shutdown();

    static SWVersionInfo GetDriverInfo();
    static SWVersionInfo GetRuntimeInfo();

    static Device *GetDevice(uint32 idx);
    STATICINLINE Device *GetDefaultDevice()
    {
        return GetDevice(0);
    }

    static const Context* GetConstContext();
    static Context* GetContext();
    static bool SetContext(Context* context);

    static RModel* GetRenderModel(Device* dev);
};
}

using OpenVR = OpenVRFun::OVRImpl;

}

#endif
