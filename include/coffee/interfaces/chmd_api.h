#ifndef COFFEE_GRAPHICS_APIS_HMD_API_H
#define COFFEE_GRAPHICS_APIS_HMD_API_H

#include <coffee/core/types/cdef/infotypes.h>
#include <coffee/core/types/graphics_types.h>

namespace Coffee{
namespace HMD{

struct CHMD_Binding
{
    struct Context;

    static const constexpr cstring SystemName = "Generic";

    enum class Eye
    {
        Left,Right,
    };

    struct Device : HWDeviceInfo
    {
        Device(CString model, CString firmware,
	       CString manuf = "", CString serial = ""):
	    HWDeviceInfo(manuf,model,firmware,serial){}
        /*!
         * \brief Retrieve firmware info for device
         * \return
         */
        SWVersionInfo firmwareInfo() const;
        /*!
         * \brief Get device model, eg. Oculus DK2
         * \return
         */
        HWDeviceInfo deviceInfo() const;

        /*!
         * \brief Reset tracking status
         */
        void reset();

        /*!
         * \brief Called at beginning of frame
         */
        void startFrame();
        /*!
         * \brief Called at end of frame
         */
        void endFrame();

        /*!
         * \brief Get resolution of eye framebuffer
         * \param e
         * \param density
         * \return
         */
        CSize resolution(Eye e, uint32 density = 1) const;

        /*!
         * \brief Window position on screen
         * \return
         */
        CRect windowPos() const;

        /*!
         * \brief Far and near values for user viewport
         * \return
         */
        ZField zfield() const;
        /*!
         * \brief Field of view for user viewport
         * \return
         */
        FovDetail fov() const;

        /*!
         * \brief Bounding box, combination of zvalues and field of view
         * \return
         */
        BoundBox viewerSpace() const;

        /*!
         * \brief User's head transform
         * \return
         */
        CMat4 head() const;

        /*!
         * \brief Acquire view transform per-eye
         * \return
         */
        CMat4 view(Eye) const;

        /*!
         * \brief Acquire angular velocity of user body
         * \return
         */
        CVec3 angularVelocity() const;
        /*!
         * \brief Acquire positional velocity of user body
         * \return
         */
        CVec3 velocity() const;

        /*!
         * \brief Connection state of device, changes upon disconnection
         * \return
         */
        bool isConnected() const;
    };

    struct Lens
    {
        /*Why MN-matrix? Allows scaling of all components in one function.*/
        _cbasic_tmnmatrix<bigscalar,3,2> distortion;
    };

    /*!
     * \brief Initializes a thread-local context for the VR system
     * \return
     */
    static bool InitializeBinding();
    /*!
     * \brief Polls for updates on devices (if necessary) and returns some state to the user
     * \param lastValidIndex Represents the amount of valid devices
     * \return
     */
    static bool PollDevices(int32 *count = nullptr);
    /*!
     * \brief Shuts down the VR context on the current thread gracefully, resetting state and etc.
     */
    static void Shutdown();

    /*!
     * \brief Compile-time driver, if applicable
     * \return
     */
    static SWVersionInfo GetDriverInfo();
    /*!
     * \brief Runtime driver, if applicable
     * \return
     */
    static SWVersionInfo GetRuntimeInfo();

    /*!
     * \brief Get a device with a certain index
     * \param id Index of the device in question
     * \return Valid pointer on success, nullptr on failure
     */
    static Device* GetDevice(int32 id);
    /*!
     * \brief Acquire a handle for the default device, quite often device 0
     * \return Valid pointer on success, nullptr on failure (eg. no VR devices)
     */
    static Device* GetDefaultDevice();

    /* Because we make it thread_local for more verbosity when something gets f***ed */
    static const Context* GetConstContext();
    /*!
     * \brief Remove context from this thread
     * \return
     */
    static Context* GetContext();
    /*!
     * \brief Set this context into this thread. Make sure there is not one there already!
     * \param context
     * \return
     */
    static bool SetContext(Context* context);

    /*!
     * \brief For differentiating a dummy from a real HMD
     * \return
     */
    static bool IsDummy();
};

}

template<typename VR,typename DevT> FORCEDINLINE
DevT* QuickLoadHMD()
{
    DevT* dev = nullptr;
    int32 devcount;
    if(VR::InitializeBinding())
    {
        if(VR::PollDevices(&devcount)&&devcount>0)
            dev = VR::GetDefaultDevice();
        VR::PollDevices();
    }
    return dev;
}

}

#endif
