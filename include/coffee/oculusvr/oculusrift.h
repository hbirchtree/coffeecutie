#pragma once

#include <coffee/interfaces/chmd_api.h>
#include <coffee/graphics_apis/SMesh>

namespace Coffee{
namespace OculusRift{

struct OculusVR : HMD::CHMD_Binding
{
    static const constexpr cstring SystemName = "Oculus Rift";

    struct Context;

    struct Device : HMD::CHMD_Binding::Device
    {
        struct ExtraData;

        Device(uint32 idx, bool dontcare = false, scalar fov = 90);

	SWVersionInfo firmwareInfo() const;
	HWDeviceInfo deviceInfo() const;

        void reset();

        void startFrame();
        void endFrame();

        CSize resolution(Eye e, uint32 density = 1) const;

	CRect windowPos() const;

        ZField zfield() const;
        FovDetail fov() const;

	BoundBox viewerSpace() const;

	CMat4 head() const;
	CMat4 view(Eye e) const;

        CVec3 angularVelocity() const;
        CVec3 velocity() const;

	Mesh const& distortionMesh(Eye e) const;

        bool isConnected() const;
    private:
        uint32 m_idx;
        ExtraData* m_data; /*!< Oculus data */
    };

    static bool InitializeBinding();
    static bool PollDevices(int32 *lastValidIndex = nullptr);
    static void Shutdown();

    static SWVersionInfo GetDriverInfo();
    static SWVersionInfo GetRuntimeInfo();

    static Device* GetDevice(int32 id);
    STATICINLINE Device* GetDefaultDevice()
    {
        return GetDevice(0);
    }
};

}
}
