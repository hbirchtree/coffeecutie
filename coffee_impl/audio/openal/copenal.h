#ifndef COPENAL_HEADER
#define COPENAL_HEADER

#include "coffee_impl/audio/caudio.h"
#include <thread>
#include <functional>
#include "coffee_types.h"
#include "coffee_macros.h"
#include "coffee/cdebug.h"

#include <AL/al.h>
#include <AL/alc.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

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
    std::thread::id context_thread; /*!< Which thread the context is currently located on*/
    CALCallback callback = nullptr; /*!< Callback to be called on error*/

    ALCcontext *context = nullptr;
    ALCdevice *device = nullptr;
};

/*!
 * \brief An AL buffer containg an audio sample
 */
struct CALBuffer
{
    ALuint handle       = 0; /*!< AL handle*/

    uint8 bits         = 0; /*!< Bit depth of sample*/
    uint32 size         = 0; /*!< Size of buffer*/
    ALenum format       = 0; /*!< Format of buffer*/
    uint32 frequency    = 0; /*!< Frequency of buffer*/
    uint8 channels     = 0; /*!< Channels of buffer*/
};

/*!
 * \brief A listener which will receive audio from a source
 */
struct CALListener
{
    scalar gain         = 10; /*!< Audio gain*/

    CVec3 orientation_forward; /*!< Listener orientation*/
    CVec3 orientation_up; /*!< Listener orientation*/

    CVec3 position; /*!< Listener position*/
    CVec3 velocity; /*!< Listener velocity*/
};

enum CALPlaybackState
{
    CALStateStopped,
    CALStatePlaying,
    CALStatePaused,
    CALStateRewind,
};

/*!
 * \brief A source which will play a sample from a buffer
 */
struct CALSource
{
    ALuint handle   = 0; /*!< AL handle*/

    const CVec3 position; /*!< Source position*/
    const CVec3 velocity; /*!< Source velocity*/
    const CVec3 direction; /*!< Source direction vector*/
    const uint16 state = AL_STOPPED; /*!< Source state*/
};

/*!
 * \brief Data defining a CALSource
 */
struct CALSourceData
{
    scalar pitch        = 1; /*!< Pitch multiplier*/
    scalar gain         = 10; /*!< Audio gain*/
    scalar max_dist     = 10; /*!< Outside of this radius, sound will not be heard*/
    scalar rolloff_dist = 10; /*!< ???*/
    scalar ref_dist     = 10; /*!< At this distance, sound is 50% reduced*/
    scalar min_gain     = 10; /*!< Minimum gain possible for the source*/
    scalar max_gain     = 10; /*!< Maximum gain possible for this source*/

    scalar cone_out_gain    = 10; /*!< Gain outside cone*/
    scalar cone_in_angle    = 10; /*!< Inner angle of audio cone*/
    scalar cone_out_angle   = 360; /*!< Outer angle of audio cone, default 360*/

    uint8  source_relative = AL_FALSE;

    uint8 looping = AL_FALSE;
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
extern bool coffee_audio_context_check_extension(const CALContext* context, cstring extension);
/*!
 * \brief Check if error is present. If it is, call context's callback
 * \param context
 */
extern void coffee_audio_context_get_error(const CALContext* context);


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
 * \brief Set propreties of source
 * \param source
 * \param data
 */
extern void coffee_audio_source_set(
        CALSource* source, const CALSourceData* data);

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
extern void coffee_audio_source_set_states(const CALSource **sources, szptr numSources, CALPlaybackState state);

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

}
}
}

#endif
