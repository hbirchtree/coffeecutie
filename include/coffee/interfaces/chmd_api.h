#ifndef COFFEE_GRAPHICS_APIS_HMD_API_H
#define COFFEE_GRAPHICS_APIS_HMD_API_H

#include <coffee/core/types/cdef/infotypes.h>
#include <coffee/core/types/graphics_types.h>
#include <coffee/graphics_apis/scene/cnode.h>

namespace Coffee{
namespace HMD{

using Nodes = CGraphicsData::NodeHierarchy<uint32>;

struct LensDetails
{
    CSize resolution;
    FovDetail horizontal;
    ZField vertical;
    scalar density;
};

struct ActorTracker_def
{
    enum BodySide
    {
        LeftSide,
        RightSide,
    };

    enum class Eye
    {
        Left,
        Right,
    };

    enum BodyParts
    {
        Spine,

        LeftEye = 1,
        RightEye,

        Head = 20,

        LeftHand,
        RightHand,
        LeftFoot,
        RightFoot,
    };

    bool tracksHead() const;
    bool tracksHands() const;
    bool tracksFeet() const;

    /*!
     * \brief Reset tracking status
     */
    void reset();

    /*!
     * \brief Get projection matrix for eye, with position relative to spine
     * \param e Which eye
     * \return
     */
    Matf4 camera(Eye e);

    /*!
     * \brief Root transform of actor
     * \return
     */
    Nodes& spine() const;

    /*!
     * \brief playSpace
     * \return
     */
    BoundBox playSpace() const;

    /*!
     * \brief Acquire angular velocity of user body
     * \return
     */
    Vecf3 angularVelocity() const;
    /*!
     * \brief Acquire positional velocity of user body
     * \return
     */
    Vecf3 velocity() const;
};

struct HeadDevice : HWDeviceInfo
{
    HeadDevice(CString model, CString firmware,
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
     * \brief Called at beginning of frame
     */
    void startFrame();
    /*!
     * \brief Called at end of frame
     */
    void endFrame();

    /*!
     * \brief Window position and size on screen
     * \return
     */
    CRect windowCanvas() const;

    /*!
     * \brief Connection state of device, changes upon disconnection
     * \return
     */
    bool isConnected() const;

    /*!
     * \brief Get the actor for this device
     * \return
     */
    ActorTracker_def& actor();

    LensDetails const* getLenses();
};

struct HeadDisplayDriver_def
{
    struct Context;

    static const constexpr cstring SystemName = "Generic";

    using Device = HeadDevice;
    using Actor = ActorTracker_def;

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
    static bool IsDummy(){return false;}
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
