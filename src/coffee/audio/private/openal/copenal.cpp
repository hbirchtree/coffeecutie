#include <coffee/audio/openal/copenal.h>

#include <coffee/core/stl_types.h>
#include <coffee/core/types/chunk.h>
#include <coffee/core/types/map.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/string_ops.h>
#include <peripherals/stl/thread_types.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

#include <al.h>
#include <alc.h>

namespace Coffee {
namespace CAudio {
namespace COpenAL {

struct CALhnd
{
    ALuint handle;
};

struct CALContext
{
    CALContext();

    ALCcontext* context;
    ALCdevice*  device;
    CALCallback callback; /*!< Callback to be called on error*/
    Thread::id
        context_thread; /*!< Which thread the context is currently located on*/
};

struct CALCaptureDevice
{
    ALCdevice* capdevice;
};

static CALCallback error_callback = nullptr;

constexpr _cbasic_static_map<CSourceProperty, ALenum, 20> al_source_prop_map = {
    {CSourceProperty::Pitch, AL_PITCH},
    {CSourceProperty::Gain, AL_GAIN},
    {CSourceProperty::MinGain, AL_MIN_GAIN},
    {CSourceProperty::MaxGain, AL_MAX_GAIN},
    {CSourceProperty::MaxDist, AL_MAX_DISTANCE},
    {CSourceProperty::RolloffFactor, AL_ROLLOFF_FACTOR},
    {CSourceProperty::ReferenceDistance, AL_REFERENCE_DISTANCE},
    {CSourceProperty::ConeOuterGain, AL_CONE_OUTER_GAIN},
    {CSourceProperty::ConeOuterAngle, AL_CONE_OUTER_ANGLE},
    {CSourceProperty::ConeInnerAngle, AL_CONE_INNER_ANGLE},
    {CSourceProperty::Relative, AL_SOURCE_RELATIVE},
    {CSourceProperty::Looping, AL_LOOPING},

    {CSourceProperty::Position, AL_POSITION},
    {CSourceProperty::Velocity, AL_VELOCITY},
    {CSourceProperty::Direction, AL_DIRECTION},
    {CSourceProperty::OffsetBytes, AL_BYTE_OFFSET},
    {CSourceProperty::OffsetSamples, AL_SAMPLE_OFFSET},
    {CSourceProperty::OffsetSeconds, AL_SEC_OFFSET},

    {CSourceProperty::PlaybackState, AL_SOURCE_STATE},

    {CSourceProperty::BuffersProcessed, AL_BUFFERS_PROCESSED},
};

CALContext::CALContext() :
    context(nullptr), device(nullptr), callback(nullptr), context_thread()
{
}

CALListener::CALListener() :
    orientation_forward(0, 0, 0), orientation_up(0, 0, 0), position(0, 0, 0),
    velocity(0, 0, 0), gain(0)
{
}

CALSource::CALSource() :
    position(0, 0, 0), velocity(0, 0, 0), direction(0, 0, 0), handle(nullptr),
    state(AL_STOPPED)
{
}

CALBuffer::CALBuffer() : handle(nullptr)
{
}

ALenum _al_get_model(const CDistanceModel& m)
{
    return AL_DISTANCE_MODEL + C_CAST<i32>(m);
}
ALuint _al_get_handle(const CALBuffer* b)
{
    if(b && b->handle)
        return b->handle->handle;
    else
        return 0;
}
ALuint _al_get_handle(const CALSource* b)
{
    if(b && b->handle)
        return b->handle->handle;
    else
        return 0;
}
void _al_check_rsc(const CALBuffer* b)
{
    if(!b || !b->handle)
        cWarning("Unsafe handle passed");
    auto hnd = _al_get_handle(b);
    if(alIsBuffer(hnd) != AL_TRUE)
        cWarning("Buffer handle not valid: {0}");
}
void _al_check_rsc(const CALSource* b)
{
    if(!b || !b->handle)
        cWarning("Unsafe handle passed");
    auto hnd = _al_get_handle(b);
    if(alIsSource(hnd) != AL_TRUE)
        cWarning("Source handle not valid: {0}");
}

ALenum _al_get_fmt(AudioFormat const& fmt)
{
    ALenum ofmt;
    switch(fmt.bitdepth)
    {
    case 8:
        if(fmt.channels == 1)
            ofmt = AL_FORMAT_MONO8;
        else
            ofmt = AL_FORMAT_STEREO8;
        break;

    case 16:
        if(fmt.channels == 1)
            ofmt = AL_FORMAT_MONO16;
        else
            ofmt = AL_FORMAT_STEREO16;
        break;
    default:
        ofmt = AL_FORMAT_MONO16;
    }
    return ofmt;
}

CALContext* context_create(cstring device)
{
    CALContext* context = new CALContext;

    if(!device)
        device = context_device_default();

    context->device = alcOpenDevice(device);
    if(!context->device)
    {
        cWarning("Failed to open ALC device");
        delete context;
        return nullptr;
    }

    cTag(
        "ALC",
        "Initialized, using device: {0}",
        alcGetString(context->device, ALC_DEVICE_SPECIFIER));

    context->context = alcCreateContext(context->device, nullptr);

    context_make_current(context);

    CALVersion ver = context_version(context);
    cTag("ALC", "Context version: {0}.{1}", ver.major, ver.minor);

    return context;
}

void context_set_distance_model(CDistanceModel const& m)
{
    alDistanceModel(_al_get_model(m));
}

void context_destroy(CALContext* context)
{
    if(context->context_thread == CurrentThread::get_id())
        alcMakeContextCurrent(context->context);

    alcDestroyContext(context->context);
    if(alcCloseDevice(context->device) != ALC_TRUE)
        cWarning("Failed to close AL device");
    context->context = nullptr;
    context->device  = nullptr;
    cTag("ALC", "Destroyed");

    delete context;
}

bool context_make_current(CALContext* context)
{
    if(CurrentThread::get_id() == context->context_thread)
        return true;

    ALCboolean b = alcMakeContextCurrent(context->context);
    if(b == ALC_FALSE)
        return false;
    context->context_thread = CurrentThread::get_id();
    return true;
}

void context_get_error(const CALContext* context)
{
    ALenum err = alGetError();
    if(context && context->callback && !error_callback)
        error_callback = context->callback;
    if(err != ALC_NO_ERROR && error_callback)
    {
        CALReport* rep = new CALReport;
        rep->message   = alGetString(err);
        error_callback(rep);
        delete rep;
    }
}

bool context_check_extension(const CALContext* context, cstring extension)
{
    return alcIsExtensionPresent(context->device, extension) == ALC_TRUE;
}

CALVersion context_version(CALContext* ctxt)
{
    CALVersion v;
    alcGetIntegerv(ctxt->device, ALC_MAJOR_VERSION, 1, &v.major);
    alcGetIntegerv(ctxt->device, ALC_MINOR_VERSION, 1, &v.minor);
    return v;
}

void context_set_debug_callback(CALContext* context, CALCallback callback)
{
    context->callback = callback;
}

void context_devices_output(Vector<cstring>& devices)
{
    if(!alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT"))
        return;

    const ALCchar* devices_c = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

    szptr i = 0;
    while(*devices_c)
    {
        devices.resize(i + 1);
        devices[i] = devices_c;
        devices_c += libc::str::len(devices_c) + 1;
        i++;
    }
}

void context_devices_input(Vector<cstring>& devices)
{
    if(!alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT"))
        return;

    const ALCchar* cdevices =
        alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER);

    if(!cdevices)
        return;

    szptr i = 0;
    while(*cdevices)
    {
        devices.resize(i + 1);
        devices[i] = cdevices;
        cdevices += libc::str::len(cdevices) + 1;
        i++;
    }
}

cstring context_device_default()
{
    return alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
}

void alAlloc(CALBuffer* buffer)
{
    buffer->handle = new CALhnd;
    alGenBuffers(1, &(buffer->handle->handle));
    context_get_error();
}

void alAlloc(CALSource* source)
{
    source->handle = new CALhnd;
    alGenSources(1, &(source->handle->handle));
    context_get_error();
}

void alFree(CALSource* source)
{
    alDeleteSources(1, &(source->handle->handle));
    context_get_error();
    delete source->handle;
    source->handle = nullptr;
}

void alFree(CALBuffer* buffer)
{
    alDeleteBuffers(1, &(buffer->handle->handle));
    context_get_error();
    delete buffer->handle;
    buffer->handle = nullptr;
}

void listener_set(const CALListener* listener)
{
    alListenerf(AL_GAIN, listener->gain);
    context_get_error();

    alListenerfv(AL_POSITION, C_RCAST<const scalar*>(&listener->position));
    context_get_error();
    alListenerfv(AL_VELOCITY, C_RCAST<const scalar*>(&listener->velocity));
    context_get_error();
    scalar* orient = new scalar[6];

    MemCpy(
        Bytes::Create(listener->orientation_forward),
        Bytes::From(&orient[0], 3));
    MemCpy(Bytes::Create(listener->orientation_up), Bytes::From(&orient[3], 3));

    //    MemCpy(&orient[0],&listener->orientation_forward,sizeof(CVec3));
    //    MemCpy(&orient[3],&listener->orientation_up,sizeof(CVec3));
    alListenerfv(AL_ORIENTATION, orient);
    delete[] orient;
    context_get_error();
}

i32 source_get_offset_seconds(const CALSource* source)
{
    _al_check_rsc(source);

    i32 i;
    alGetSourcei(_al_get_handle(source), AL_SEC_OFFSET, &i);
    context_get_error();
    return i;
}

i32 source_get_offset_samples(const CALSource* source)
{
    _al_check_rsc(source);

    i32 i;
    alGetSourcei(_al_get_handle(source), AL_SAMPLE_OFFSET, &i);
    context_get_error();
    return i;
}

i32 source_get_offset_bytes(const CALSource* source)
{
    _al_check_rsc(source);

    i32 i;
    alGetSourcei(_al_get_handle(source), AL_BYTE_OFFSET, &i);
    context_get_error();
    return i;
}

void source_queue_buffers(
    CALSource* source, szptr numBuffers, const CALBuffer* const* buffers)
{
    _al_check_rsc(source);

    ALuint* handles = new ALuint[numBuffers];
    for(szptr i = 0; i < numBuffers; i++)
    {
        _al_check_rsc(buffers[i]);
        auto hnd   = _al_get_handle(buffers[i]);
        handles[i] = hnd;
    }
    alSourceQueueBuffers(_al_get_handle(source), numBuffers, handles);
    delete[] handles;
    context_get_error();
}

void source_dequeue_buffers(
    CALSource* source, szptr numBuffers, const CALBuffer* const* buffers)
{
    _al_check_rsc(source);
    ALuint* handles = new ALuint[numBuffers];
    for(szptr i = 0; i < numBuffers; i++)
    {
        _al_check_rsc(buffers[i]);
        if(buffers[i] && buffers[i]->handle)
            handles[i] = buffers[i]->handle->handle;
    }
    alSourceUnqueueBuffers(_al_get_handle(source), numBuffers, handles);
    delete[] handles;
    context_get_error();
}

void source_seti(CALSource* source, CSourceProperty const& prop, const i32* val)
{
    _al_check_rsc(source);
    alSourceiv(
        _al_get_handle(source), get_value(prop, al_source_prop_map), val);
    context_get_error();
}

void source_setf(
    CALSource* source, CSourceProperty const& prop, const scalar* val)
{
    _al_check_rsc(source);
    alSourcefv(
        _al_get_handle(source), get_value(prop, al_source_prop_map), val);
    context_get_error();
}

void source_set_state(CALSource* source, CALPlaybackState state)
{
    switch(state)
    {
    case CALPlaybackState::Stopped:
        alSourceStop(_al_get_handle(source));
        break;
    case CALPlaybackState::Playing:
        alSourcePlay(_al_get_handle(source));
        break;
    case CALPlaybackState::Paused:
        alSourcePause(_al_get_handle(source));
        break;
    case CALPlaybackState::Rewind:
        alSourceRewind(_al_get_handle(source));
        break;
    }
    context_get_error();
}

bool source_playing(CALSource* source)
{
    return source_geti(source, CSourceProperty::PlaybackState) == AL_PLAYING;
}

void source_set_states(
    const CALSource** sources, szptr numSources, CALPlaybackState state)
{
    ALuint* src = new ALuint[numSources];
    for(szptr i = 0; i < numSources; i++)
        src[i] = _al_get_handle(sources[i]);

    switch(state)
    {
    case CALPlaybackState::Stopped:
        alSourceStopv(numSources, src);
        break;
    case CALPlaybackState::Playing:
        alSourcePlayv(numSources, src);
        break;
    case CALPlaybackState::Paused:
        alSourcePausev(numSources, src);
        break;
    case CALPlaybackState::Rewind:
        alSourceRewindv(numSources, src);
        break;
    }
    context_get_error();
    delete[] src;
}

void source_set_offset_seconds(CALSource* source, i32 const& off)
{
    source_seti(source, CSourceProperty::OffsetSeconds, &off);
}

void source_set_offset_samples(CALSource* source, i32 const& off)
{
    source_seti(source, CSourceProperty::OffsetSamples, &off);
}

void source_set_offset_bytes(CALSource* source, i32 const& off)
{
    source_seti(source, CSourceProperty::OffsetBytes, &off);
}

void source_transform(
    CALSource*   source,
    Vecf3 const& position,
    Vecf3 const& velocity,
    Vecf3 const& direction)
{
    _al_check_rsc(source);

    source->position  = position;
    source->velocity  = velocity;
    source->direction = direction;

    source_setf(
        source, CSourceProperty::Position, (const scalar*)&source->position);
    source_setf(
        source, CSourceProperty::Velocity, (const scalar*)&source->velocity);
    source_setf(
        source, CSourceProperty::Direction, (const scalar*)&source->direction);
}

void source_seti(CALSource* source, CSourceProperty const& prop, const i32& val)
{
    source_seti(source, prop, &val);
}

void source_setf(
    CALSource* source, CSourceProperty const& prop, const scalar& val)
{
    source_setf(source, prop, &val);
}

i32 source_geti(CALSource* source, const CSourceProperty& prop)
{
    _al_check_rsc(source);

    ALint v;
    alGetSourcei(
        _al_get_handle(source), get_value(prop, al_source_prop_map), &v);
    return v;
}

scalar source_getf(CALSource* source, const CSourceProperty& prop)
{
    _al_check_rsc(source);

    ALfloat v;
    alGetSourcef(
        _al_get_handle(source), get_value(prop, al_source_prop_map), &v);
    return v;
}

void buffer_data(CALBuffer* buffer, const AudioSample* sample)
{
    _al_check_rsc(buffer);

    ALenum fmt = _al_get_fmt(sample->fmt);

    alBufferData(
        _al_get_handle(buffer),
        fmt,
        sample->data,
        C_CAST<ALCsizei>(
            sample->samples * sample->fmt.channels * sizeof(sample->data[0])),
        C_CAST<ALCsizei>(sample->fmt.samplerate));
}

void alAlloc(CALBuffer* buffer, const AudioSample* sample)
{
    alAlloc(buffer);
    _al_check_rsc(buffer);
    buffer_data(buffer, sample);
    context_get_error();
}

CALCaptureDevice* capture_create(
    CALContext* context, cstring device, AudioFormat const& fmt, u32 samples)
{
    context_make_current(context);

    CALCaptureDevice* cdev = new CALCaptureDevice;

    cdev->capdevice = alcCaptureOpenDevice(
        device,
        fmt.samplerate,
        _al_get_fmt(fmt),
        C_CAST<ALCsizei>(
            samples * fmt.samplerate * (fmt.bitdepth / 8) * fmt.channels));

    cDebug(
        "Creating AL capture device: {0},fq={1},buffer={2}",
        device,
        fmt.samplerate,
        samples * (fmt.bitdepth / 8) * fmt.channels);

    return cdev;
}

void capture_free(CALCaptureDevice* dev)
{
    alcCaptureCloseDevice(dev->capdevice);
    delete dev;
}

void capture_start(CALCaptureDevice* dev)
{
    alcCaptureStart(dev->capdevice);
}

void capture_stop(CALCaptureDevice* dev)
{
    alcCaptureStop(dev->capdevice);
}

void capture_grab_samples(CALCaptureDevice* dev, AudioSample& sample)
{
    alcCaptureSamples(dev->capdevice, sample.data, sample.samples);
}

} // namespace COpenAL
} // namespace CAudio

namespace Strings {
CString to_string(CAudio::COpenAL::CALBuffer const& buf)
{
    return "CALBuffer(" + str::print::pointerify(buf.handle) + "," +
           ((buf.handle) ? cast_pod(buf.handle->handle) : CString("0x0")) + ")";
}

CString to_string(CAudio::COpenAL::CALSource const& src)
{
    return "CALSource(" + str::print::pointerify(src.handle) + "," +
           ((src.handle) ? cast_pod(src.handle->handle) : CString("0x0")) + ")";
}
} // namespace Strings
} // namespace Coffee
