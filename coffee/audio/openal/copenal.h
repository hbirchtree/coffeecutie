#ifndef COPENAL_HEADER
#define COPENAL_HEADER

#include <coffee/core/coffee_types.h>
#include <coffee/core/coffee_macros.h>
#include <coffee/core/base/cdebug.h>
#include <coffee/audio/caudio.h>
#include <thread>
#include <functional>

#include <AL/al.h>
#include <AL/alc.h>

namespace Coffee{
namespace CAudio{
/*!
 * \brief Basic wrapping for OpenAL, nothing extensive
 *
 */
namespace COpenAL{

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
    Looping
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

typedef std::function<void(CALReport*)> CALCallback;

/*!
 * \brief Presents an OpenAL context in which sound can be played
 */
struct CALContext
{
    CALContext();

    ALCcontext *context;
    ALCdevice *device;
    CALCallback callback; /*!< Callback to be called on error*/
    std::thread::id context_thread; /*!< Which thread the context is currently located on*/
};

/*!
 * \brief An AL buffer containg an audio sample
 */
struct CALBuffer
{
    ALuint handle;
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
    ALuint handle; /*!< AL handle*/
    uint16 state; /*!< Source state*/
};

/*!
 * \brief Create an OpenAL context
 * \param context Context object to be bound with the new context
 */
extern bool coffee_audio_context_create(CALContext* context);
/*!
 * \brief Destroy an OpenAL context
 * \param context Context object to destroy
 */
extern void coffee_audio_context_destroy(CALContext* context);
/*!
 * \brief Make the context current to this thread
 * \param context Context to make current
 */
extern bool coffee_audio_context_make_current(CALContext* context);
/*!
 * \brief Check if an extension is supported
 * \param context
 * \param extension
 * \return True if supported
 */
extern bool coffee_audio_context_check_extension(
        const CALContext* context, cstring extension);
/*!
 * \brief Check if error is present. If it is, call context's callback
 * \param context
 */
extern void coffee_audio_context_get_error(const CALContext* context = nullptr);

/*!
 * \brief Allocate audio buffer with sample data
 * \param buffer
 * \param sample
 */
extern void coffee_audio_alloc(
        CALBuffer* buffer, CAudioSample* sample);
/*!
 * \brief Allocate audio buffer
 * \param buffer
 */
extern void coffee_audio_alloc(CALBuffer* buffer);
/*!
 * \brief Allocate audio source
 * \param source
 */
extern void coffee_audio_alloc(CALSource* source);
/*!
 * \brief Free an AL source
 * \param source
 */
extern void coffee_audio_free(CALSource* source);
/*!
 * \brief Free an AL buffer
 * \param buffer
 */
extern void coffee_audio_free(CALBuffer* buffer);

/*!
 * \brief Get second offset into buffer
 * \param source
 * \return
 */
extern int32 coffee_audio_source_get_offset_seconds(
        const CALSource* source);
/*!
 * \brief Get sample offset into buffer
 * \param source
 * \return
 */
extern int32 coffee_audio_source_get_offset_samples(
        const CALSource* source);
/*!
 * \brief Get byte offset into buffer
 * \param source
 * \return
 */
extern int32 coffee_audio_source_get_offset_bytes(
        const CALSource* source);

/*!
 * \brief Set second offset into buffer
 * \param source
 * \param off
 */
extern void coffee_audio_source_set_offset_seconds(
        CALSource* source, int32 off);
/*!
 * \brief Set sample offset into buffer
 * \param source
 * \param off
 */
extern void coffee_audio_source_set_offset_samples(
        CALSource* source, int32 off);
/*!
 * \brief Set byte offset into buffer
 * \param source
 * \param off
 */
extern void coffee_audio_source_set_offset_bytes(
        CALSource* source, int32 off);

/*!
 * \brief Set source playback mode
 * \param source
 * \param state
 */
extern void coffee_audio_source_set_state(
        CALSource* source, CALPlaybackState state);

extern void coffee_audio_source_transform(
        CALSource* source, const CVec3 &position,
        const CVec3 &velocity, const CVec3 &direction);

/*!
 * \brief Set playback mode for multiple sources
 * \param sources An array of source handles
 * \param numSources Number of sources to process
 * \param state
 */
extern void coffee_audio_source_set_states(
        const CALSource **sources,
        szptr numSources,
        CALPlaybackState state);

/*!
 * \brief Set AL listener properties
 * \param listener
 */
extern void coffee_audio_listener_set(const CALListener* listener);

/*!
 * \brief Queue buffers for playback
 * \param source
 * \param numBuffers
 * \param buffers
 */
extern void coffee_audio_source_queue_buffers(
        CALSource* source, szptr numBuffers, CALBuffer** buffers);
/*!
 * \brief Dequeue buffers if it has not been played yet
 * \param source
 * \param numBuffers
 * \param buffers
 */
extern void coffee_audio_source_dequeue_buffers(
        CALSource* source, szptr numBuffers, CALBuffer** buffers);

/*!
 * \brief Set integer property of source
 * \param source
 * \param prop
 * \param val
 */
extern void coffee_audio_source_seti(
        CALSource* source, CSourceProperty prop, int32 val);
/*!
 * \brief Set scalar property of source
 * \param source
 * \param prop
 * \param val
 */
extern void coffee_audio_source_setf(
        CALSource* source, CSourceProperty prop, scalar val);

extern void coffee_audio_context_set_distance_model(
        CDistanceModel m);

}
}
}

#endif
