#ifndef COFFEE_OPENVR_OPENVR_H
#define COFFEE_OPENVR_OPENVR_H

#include <base/cdebug.h>
#include <chmd_api.h>

namespace Coffee{
namespace OpenVR{

struct OVR_Context;

struct OVRImpl : HMD::CHMD_Binding{
    struct OVRDevice : Device
    {
        OVRDevice(uint32 index);

        bool updateState();

        CString make() const;
        CString firmware() const;
        CMat4 view(Eye e) const;

        CVec3 angularVelocity() const;
        CVec3 velocity() const;

        bool isConnected() const;
    private:
        uint32 m_dIndex;
    };

    static bool InitializeBinding();

    static bool PollDevices(uint32 *lastValidIndex = nullptr);

//    static void SubmitTextures(vr::Texture_t* left, vr::Texture_t* right);

    static void Shutdown();

    static OVRDevice *GetDevice(uint32 idx);

    /* Because we make it thread_local for more verbosity when something gets f***ed */
    static const OVR_Context* GetConstContext();
    /*!
     * \brief Remove context from this thread
     * \return
     */
    static OVR_Context* GetContext();
    /*!
     * \brief Set this context into this thread. Make sure there is not one there already!
     * \param context
     * \return
     */
    static bool SetContext(OVR_Context* context);
};
}

using OpenVRDev = OpenVR::OVRImpl;

}

#endif
