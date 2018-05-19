#ifndef COPENAL_HEADER
#define COPENAL_HEADER

#include <coffee/audio/caudio.h>
#include <coffee/core/types/edef/dbgenum.h>
#include <coffee/core/types/cdef/infotypes.h>
#include <coffee/core/types/tdef/stlfunctypes.h>
#include <coffee/core/types/vector_types.h>

namespace Coffee{
namespace CAudio{

struct OpenAL
{
    struct ALcontext;
    struct ALdev;
    struct ALhnd;
    struct ALcapdevice;

    using ALCallback = void(*)(Severity,cstring);
    using ALVersion = _cbasic_version<int32>;
    using ALbuffer = ALhnd;
    using ALsource = ALhnd;

    enum class PlaybackState
    {
        Stopped,
        Playing,
        Paused,
        Rewind,
    };

    enum class SourceProperty
    {
	None,

	Relative,
	Looping,

        Pitch,
        Gain,
        MinGain,
        MaxGain,

        MaxDist,
        RolloffFactor,
        ReferenceDistance,

        ConeOuterGain,
        ConeInnerAngle,
        ConeOuterAngle,

        Position,
        Velocity,
        Direction,

        OffsetSeconds,
        OffsetSamples,
        OffsetBytes,
    };

    enum class Attenuation
    {
	None	 = 0,
        /*Inverse, Linear and Exponent cannot be combined.*/
        Inverse  = 1,
        Linear   = 2,
        Exponent = 3,

        Clamped  = 0x10,
    };

    enum class ContextProperty
    {
	DevSpecifier,
	CaptureDevSpecifier,

	DefaultDev,
        DefaultCapDev,

	Version,
	Renderer,
	Vendor,
	Extensions,

	Doppler,
	SpeedOfSound,
	AttenuationModl,
    };

    enum class ListenerProperty
    {
	Gain,
	Position,
	Velocity,
	Orientation,
    };

    enum class Feature
    {

    };

    static ALcontext* CreateContext();
    static void DeleteContext(ALcontext* ctxt);
    static void ContextCurrent(ALcontext* ctxt);

    static void ContextSuspend(ALcontext* ctxt);
    static void ContextProcess(ALcontext* ctxt);

    static ALdev* DeviceGet(cstring id);
    static ALdev* CaptureDeviceGet(cstring id, SampleFormat f);

    static void DeviceClose(ALdev* d);
    static void CaptureDeviceClose(ALdev* d);

    static void Enable(Feature f);
    static void Disable(Feature f);

    static void ContextSetAttenuation(Attenuation m);

    static void ContextGeti(ContextProperty p, int32* v);
    static void ContextGetf(ContextProperty p, scalar* v);
    static void ContextSeti(ContextProperty p, const int32* v);
    static void ContextSetf(ContextProperty p, const scalar* v);

    static ALbuffer* BufAlloc();
    static void BufFree(ALbuffer* b);
    static void BufData(ALbuffer* b, SampleFormat f, const Bytes& d);
    static ALbuffer* BufCreate(SampleFormat f, const Bytes& d);

    static ALsource* SrcAlloc();
    static void SrcFree(ALsource* s);

    static void SrcState(ALsource** s,PlaybackState m);

    static void SrcSetBuffer(ALsource* s, const ALbuffer& b);
    static void SrcQueueBuffers(ALsource* s, uint32 n, const ALbuffer** b);
    static void SrcDequeueBuffers(ALsource* s, uint32 n, const ALbuffer** b);

    static void SrcGeti(ALsource* s, SourceProperty p, int32* v);
    static void SrcGetf(ALsource* s, SourceProperty p, scalar* v);
    static void SrcSeti(ALsource* s, SourceProperty p, const int32* v);
    static void SrcSetf(ALsource* s, SourceProperty p, const scalar* v);

    static void ListenerGeti(ListenerProperty p, int32* v);
    static void ListenerGetf(ListenerProperty p, scalar* v);
    static void ListenerSeti(ListenerProperty p, const int32* v);
    static void ListenerSetf(ListenerProperty p, const scalar* v);

    static void CaptureState(ALdev* d, bool start = true);
    static void CaptureCollect(ALdev* d, Bytes& target);

    struct Debug
    {
        static void CheckExtension(ALcontext* ctxt, cstring ext);

        static void SetDebugCallback(ALcontext* ctxt, ALCallback c);
        static void ProcessError(ALcontext* ctxt);
        static void ProcessContextError(ALcontext* ctxt);

        static ALVersion ContextVersion(ALcontext* ctxt);

        static bool IsEnabled(Feature f);
        static bool IsBuffer(ALbuffer* b);
        static bool IsSource(ALsource* s);

	static void GetBool(ALdev* d, ContextProperty p, bool* v);
	static void GetInt(ALdev* d, ContextProperty p, int32* v);
	static void GetScalar(ALdev* d, ContextProperty p, scalar* v);
	static void GetBigScalar(ALdev* d, ContextProperty p, bigscalar* v);
	static cstring GetString(ALdev* d, ContextProperty p);

        static cstring* GetDevices(ALcontext* d);
        static cstring* GetCaptureDevices(ALcontext* d);
    };

    struct EAX
    {
        /* ??? */
    };
};

C_FLAGS(OpenAL::Attenuation,uint32);

/*!
 * \brief Basic wrapping for OpenAL, nothing extensive
 *
 */
namespace COpenAL{

struct CALhnd;
struct CALCaptureDevice;
using CALVersion = _cbasic_version<int32>;

enum class CALPlaybackState : uint8
{
    Stopped,
    Playing,
    Paused,
    Rewind,
};

enum class CSourceProperty : uint8
{
    Pitch,

    Gain,
    MinGain,
    MaxGain,

    MaxDist,
    RolloffFactor,
    ReferenceDistance,

    ConeOuterGain,
    ConeInnerAngle,
    ConeOuterAngle,

    Relative,
    Looping,

    Position,
    Velocity,
    Direction,

    OffsetSeconds,
    OffsetSamples,
    OffsetBytes,

    PlaybackState,

    BuffersProcessed,
};

enum class CDistanceModel : uint8
{
    InverseDistance = 1,
    InverseDistanceClamped = 2,
    LinearDistance = 3,
    LinearDistanceClamped = 4,
    ExponentDistance = 5,
    ExponentDistanceClamped = 6,
};

/*!
 * \brief Presents an error message from OpenAL
 */
struct CALReport
{
    cstring message;
};

using CALCallback = Function<void(CALReport*)>;

/*!
 * \brief Presents an OpenAL context in which sound can be played
 */
struct CALContext;

/*!
 * \brief An AL buffer containg an audio sample
 */
struct CALBuffer
{
    CALBuffer();

    CALhnd* handle;
};

/*!
 * \brief A listener which will receive audio from a source
 */
struct CALListener
{
    CALListener();

    CVec3 orientation_forward; /*!< Listener orientation*/
    CVec3 orientation_up; /*!< Listener orientation*/

    CVec3 position; /*!< Listener position*/
    CVec3 velocity; /*!< Listener velocity*/

    scalar gain; /*!< Audio gain*/
};

/*!
 * \brief A source which will play a sample from a buffer
 */
struct CALSource
{
    CALSource();

    CVec3 position; /*!< Source position*/
    CVec3 velocity; /*!< Source velocity*/
    CVec3 direction; /*!< Source direction vector*/
    CALhnd* handle; /*!< AL handle*/
    uint16 state; /*!< Source state*/
};

extern CALVersion context_version(CALContext *ctxt);

/*!
 * \brief Create an OpenAL context
 * \param context Context object to be bound with the new context
 */
extern CALContext* context_create(cstring device = nullptr);
/*!
 * \brief Set error callback for the OpenAL context
 * \param callback
 */
extern void context_set_debug_callback(
        CALContext *context, CALCallback callback);
/*!
 * \brief Destroy an OpenAL context
 * \param context Context object to destroy
 */
extern void context_destroy(
        CALContext* context);
/*!
 * \brief Make the context current to this thread
 * \param context Context to make current
 */
extern bool context_make_current(
        CALContext* context);
/*!
 * \brief Check if an extension is supported
 * \param context
 * \param extension
 * \return True if supported
 */
extern bool context_check_extension(
        const CALContext* context, cstring extension);
/*!
 * \brief Check if error is present. If it is, call context's callback
 * \param context
 */
extern void context_get_error(
        const CALContext* context = nullptr);

extern uint32 context_max_sources(const CALContext* context = nullptr);

/*!
 * \brief Create an audio capture device and initialize it with codec information. OpenAL does the rest.
 * \param context
 * \param device
 * \param fmt
 * \return A valid pointer if the function succeeded
 */
extern CALCaptureDevice* capture_create(
        CALContext* context,
        cstring device,
	const AudioFormat &fmt, uint32 samples);
/*!
 * \brief Free the audio capture device when you are done with it.
 * \param dev
 */
extern void capture_free(CALCaptureDevice *dev);

extern void capture_start(CALCaptureDevice* dev);
extern void capture_grab_samples(CALCaptureDevice* dev, AudioSample &sample);
extern void capture_stop(CALCaptureDevice* dev);

/*!
 * \brief Allocate audio buffer with sample data
 * \param buffer
 * \param sample
 */
extern void alAlloc(
	CALBuffer* buffer, const AudioSample* sample);
/*!
 * \brief Allocate audio buffer
 * \param buffer
 */
extern void alAlloc(CALBuffer* buffer);
/*!
 * \brief Allocate audio source
 * \param source
 */
extern void alAlloc(CALSource* source);
/*!
 * \brief Free an AL source
 * \param source
 */
extern void alFree(CALSource* source);
/*!
 * \brief Free an AL buffer
 * \param buffer
 */
extern void alFree(CALBuffer* buffer);

/*!
 * \brief Set AL listener properties
 * \param listener
 */
extern void listener_set(const CALListener* listener);

/*!
 * \brief Get second offset into buffer
 * \param source
 * \return
 */
extern int32 source_get_offset_seconds(
        const CALSource* source);
/*!
 * \brief Get sample offset into buffer
 * \param source
 * \return
 */
extern int32 source_get_offset_samples(
        const CALSource* source);
/*!
 * \brief Get byte offset into buffer
 * \param source
 * \return
 */
extern int32 source_get_offset_bytes(
        const CALSource* source);

/*!
 * \brief Set second offset into buffer
 * \param source
 * \param off
 */
extern void source_set_offset_seconds(
        CALSource* source, int32 const& off);
/*!
 * \brief Set sample offset into buffer
 * \param source
 * \param off
 */
extern void source_set_offset_samples(
        CALSource* source, int32 const& off);
/*!
 * \brief Set byte offset into buffer
 * \param source
 * \param off
 */
extern void source_set_offset_bytes(
        CALSource* source, const int32 &off);

/*!
 * \brief Set source playback mode
 * \param source
 * \param state
 */
extern void source_set_state(
        CALSource* source, CALPlaybackState state);

extern bool source_playing(CALSource* source);

extern void source_transform(
        CALSource* source, const CVec3 &position,
        const CVec3 &velocity, const CVec3 &direction);

/*!
 * \brief Set playback mode for multiple sources
 * \param sources An array of source handles
 * \param numSources Number of sources to process
 * \param state
 */
extern void source_set_states(
        const CALSource **sources,
        szptr numSources,
        CALPlaybackState state);

/*!
 * \brief Queue buffers for playback
 * \param source
 * \param numBuffers
 * \param buffers
 */
extern void source_queue_buffers(
        CALSource* source, szptr numBuffers, const CALBuffer * const* buffers);
/*!
 * \brief Dequeue buffers if it has not been played yet
 * \param source
 * \param numBuffers
 * \param buffers
 */
extern void source_dequeue_buffers(
        CALSource* source, szptr numBuffers, const CALBuffer * const* buffers);

/*!
 * \brief Set integer property of source
 * \param source
 * \param prop
 * \param val
 */
extern void source_seti(
        CALSource* source, CSourceProperty const& prop, const int32* val);
/*!
 * \brief Set scalar property of source
 * \param source
 * \param prop
 * \param val
 */
extern void source_setf(
        CALSource* source, CSourceProperty const& prop, const scalar *val);


extern void source_seti(
        CALSource* source, CSourceProperty const& prop, int32 const& val);
extern void source_setf(
        CALSource* source, CSourceProperty const& prop, scalar const& val);

extern int32 source_geti(
        CALSource* source, CSourceProperty const& prop);
extern scalar source_getf(
        CALSource* source, CSourceProperty const& prop);

/*!
 * \brief Fill buffer with data, sets full buffer
 * \param buffer
 * \param sample
 */
extern void buffer_data(
	CALBuffer* buffer, const AudioSample* sample);

extern void context_set_distance_model(const CDistanceModel &m);

extern void context_devices_output(Vector<cstring>& devices);
extern void context_devices_input(Vector<cstring>& devices);
extern cstring context_device_default();

}
}
namespace Strings{

extern CString to_string(CAudio::COpenAL::CALBuffer const& buf);
extern CString to_string(CAudio::COpenAL::CALSource const& src);

}
}

#endif
