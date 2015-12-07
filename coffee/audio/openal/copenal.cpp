#include "copenal.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <coffee/core/coffee_macros.h>
#include <coffee/core/base/cdebug.h>

#include <thread>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

struct CALhnd
{
    ALuint handle;
};

struct CALContext
{
    CALContext();

    ALCcontext *context;
    ALCdevice *device;
    CALCallback callback; /*!< Callback to be called on error*/
    std::thread::id context_thread; /*!< Which thread the context is currently located on*/
};

struct CALCaptureDevice
{
    ALCdevice* capdevice;
};

static CALCallback error_callback = nullptr;

constexpr _cbasic_static_map<CSourceProperty,ALenum,18> al_source_prop_map = {
{CSourceProperty::Pitch,AL_PITCH},
{CSourceProperty::Gain,AL_GAIN},
{CSourceProperty::MinGain,AL_MIN_GAIN},
{CSourceProperty::MaxGain,AL_MAX_GAIN},
{CSourceProperty::MaxDist,AL_MAX_DISTANCE},
{CSourceProperty::RolloffFactor,AL_ROLLOFF_FACTOR},
{CSourceProperty::ReferenceDistance,AL_REFERENCE_DISTANCE},
{CSourceProperty::ConeOuterGain,AL_CONE_OUTER_GAIN},
{CSourceProperty::ConeOuterAngle,AL_CONE_OUTER_ANGLE},
{CSourceProperty::ConeInnerAngle,AL_CONE_INNER_ANGLE},
{CSourceProperty::Relative,AL_SOURCE_RELATIVE},
{CSourceProperty::Looping,AL_LOOPING},

    {CSourceProperty::Position,AL_POSITION},
    {CSourceProperty::Velocity,AL_VELOCITY},
    {CSourceProperty::Direction,AL_DIRECTION},
    {CSourceProperty::OffsetBytes,AL_BYTE_OFFSET},
    {CSourceProperty::OffsetSamples,AL_SAMPLE_OFFSET},
    {CSourceProperty::OffsetSeconds,AL_SEC_OFFSET},

};

CALContext::CALContext():
    context(nullptr),
    device(nullptr),
    callback(nullptr),
    context_thread()
{
}

CALListener::CALListener():
    orientation_forward(0,0,0),
    orientation_up(0,0,0),
    position(0,0,0),
    velocity(0,0,0),
    gain(0)
{
}

CALSource::CALSource():
    position(0,0,0),
    velocity(0,0,0),
    direction(0,0,0),
    handle(nullptr),
    state(AL_STOPPED)
{
}

CALBuffer::CALBuffer():
    handle(nullptr)
{
}

ALenum _al_get_model(const CDistanceModel& m)
{
    return AL_DISTANCE_MODEL + (uint32)m;
}
ALuint _al_get_handle(const CALBuffer* b)
{
    return b->handle->handle;
}
ALuint _al_get_handle(const CALSource* b)
{
    return b->handle->handle;
}
ALenum _al_get_fmt(CAudioFormat const& fmt)
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

CALContext *coffee_audio_context_create(cstring device)
{
    CALContext* context = new CALContext;

    if(!device)
        device = coffee_audio_context_device_default();

    context->device = alcOpenDevice(device);
    if(!context->device)
    {
        cWarning("Failed to open ALC device");
        delete context;
        return nullptr;
    }

    cMsg("ALC","Initialized, using device: %s",
         alcGetString(context->device,ALC_DEVICE_SPECIFIER));

    context->context = alcCreateContext(context->device,NULL);

    coffee_audio_context_make_current(context);

    CALVersion ver = coffee_audio_context_version(context);
    cMsg("ALC","Context version: %i.%i",ver.major,ver.minor);

    return context;
}

void coffee_audio_context_set_distance_model(CDistanceModel const& m)
{
    alDistanceModel(_al_get_model(m));
}

void coffee_audio_context_destroy(CALContext *context)
{
    if(context->context_thread==std::this_thread::get_id())
        alcMakeContextCurrent(context->context);

    alcDestroyContext(context->context);
    if(alcCloseDevice(context->device)!=ALC_TRUE)
        cWarning("Failed to close AL device");
    context->context = nullptr;
    context->device = nullptr;
    cMsg("ALC","Destroyed");

    delete context;
}

bool coffee_audio_context_make_current(CALContext *context)
{
    if(std::this_thread::get_id()==context->context_thread)
        return true;

    ALCboolean b = alcMakeContextCurrent(context->context);
    if(b==ALC_FALSE)
        return false;
    context->context_thread = std::this_thread::get_id();
    return true;
}

void coffee_audio_context_get_error(const CALContext *context)
{
    ALenum err = alGetError();
    if(context&&context->callback&&!error_callback)
        error_callback = context->callback;
    if(err!=ALC_NO_ERROR&&error_callback)
    {
        CALReport* rep = new CALReport;
        rep->message = alGetString(err);
        error_callback(rep);
        delete rep;
    }
}

bool coffee_audio_context_check_extension(const CALContext *context, cstring extension)
{
    return alcIsExtensionPresent(context->device,extension)==ALC_TRUE;
}

void coffee_audio_alloc(CALBuffer *buffer)
{
    buffer->handle = new CALhnd;
    alGenBuffers(1,&(buffer->handle->handle));
    coffee_audio_context_get_error();
}

void coffee_audio_alloc(CALSource *source)
{
    source->handle = new CALhnd;
    alGenSources(1,&(source->handle->handle));
    coffee_audio_context_get_error();
}

void coffee_audio_free(CALSource *source)
{
    alDeleteSources(1,&(source->handle->handle));
    coffee_audio_context_get_error();
    source->handle = nullptr;
}

void coffee_audio_free(CALBuffer *buffer)
{
    alDeleteBuffers(1,&(buffer->handle->handle));
    coffee_audio_context_get_error();
    buffer->handle = nullptr;
}

int32 coffee_audio_source_get_offset_seconds(const CALSource *source)
{
    int32 i;
    alGetSourcei(_al_get_handle(source),AL_SEC_OFFSET,&i);
    coffee_audio_context_get_error();
    return i;
}

int32 coffee_audio_source_get_offset_samples(const CALSource *source)
{
    int32 i;
    alGetSourcei(_al_get_handle(source),AL_SAMPLE_OFFSET,&i);
    coffee_audio_context_get_error();
    return i;
}

int32 coffee_audio_source_get_offset_bytes(const CALSource *source)
{
    int32 i;
    alGetSourcei(_al_get_handle(source),AL_BYTE_OFFSET,&i);
    coffee_audio_context_get_error();
    return i;
}

void coffee_audio_listener_set(const CALListener *listener)
{
    alListenerf(AL_GAIN,listener->gain);
    coffee_audio_context_get_error();

    alListenerfv(AL_POSITION,(scalar*)&listener->position);
    coffee_audio_context_get_error();
    alListenerfv(AL_VELOCITY,(scalar*)&listener->velocity);
    coffee_audio_context_get_error();
    scalar *orient = new scalar[6];
    c_memcpy(&orient[0],&listener->orientation_forward,sizeof(CVec3));
    c_memcpy(&orient[2],&listener->orientation_up,sizeof(CVec3));
    alListenerfv(AL_ORIENTATION,orient);
    delete[] orient;
    coffee_audio_context_get_error();
}

void coffee_audio_source_queue_buffers(
        CALSource *source, szptr numBuffers, CALBuffer **buffers)
{
    ALuint* handles = new ALuint[numBuffers];
    for(szptr i=0;i<numBuffers;i++)
        handles[i] = buffers[i]->handle->handle;
    alSourceQueueBuffers(_al_get_handle(source),numBuffers,handles);
    delete[] handles;
    coffee_audio_context_get_error();
}

void coffee_audio_source_dequeue_buffers(
        CALSource *source, szptr numBuffers, CALBuffer **buffers)
{
    alSourceUnqueueBuffers(_al_get_handle(source),numBuffers,(ALuint*)(*buffers));
    coffee_audio_context_get_error();
}

void coffee_audio_source_seti(
        CALSource *source, CSourceProperty const& prop, const int32 *val)
{
    alSourceiv(_al_get_handle(source),coffee_get_value(prop,al_source_prop_map),val);
    coffee_audio_context_get_error();
}

void coffee_audio_source_setf(
        CALSource *source, CSourceProperty const& prop, const scalar* val)
{
    alSourcefv(_al_get_handle(source),coffee_get_value(prop,al_source_prop_map),val);
    coffee_audio_context_get_error();
}

void coffee_audio_source_set_state(CALSource *source, CALPlaybackState state)
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
    coffee_audio_context_get_error();
}

void coffee_audio_source_set_states(
        const CALSource **sources, szptr numSources, CALPlaybackState state)
{
    ALuint *src = new ALuint[numSources];
    for(szptr i=0;i<numSources;i++)
        src[i] = _al_get_handle(sources[i]);

    switch(state)
    {
    case CALPlaybackState::Stopped:
        alSourceStopv(numSources,src);
        break;
    case CALPlaybackState::Playing:
        alSourcePlayv(numSources,src);
        break;
    case CALPlaybackState::Paused:
        alSourcePausev(numSources,src);
        break;
    case CALPlaybackState::Rewind:
        alSourceRewindv(numSources,src);
        break;
    }
    coffee_audio_context_get_error();
    delete[] src;
}

void coffee_audio_buffer_data(CALBuffer *buffer, const CAudioSample *sample)
{
    ALenum fmt = _al_get_fmt(sample->fmt);

    alBufferData(
                _al_get_handle(buffer),fmt,
                sample->data,
                sample->fmt.samples*sample->fmt.channels*sizeof(sample->data[0]),
            sample->fmt.samplerate);
}

void coffee_audio_context_set_debug_callback(CALContext *context, CALCallback callback)
{
    context->callback = callback;
}

void coffee_audio_alloc(CALBuffer *buffer, const CAudioSample *sample)
{
    coffee_audio_alloc(buffer);
    coffee_audio_buffer_data(buffer,sample);
    coffee_audio_context_get_error();
}

void coffee_audio_source_set_offset_seconds(CALSource *source, int32 const& off)
{
    coffee_audio_source_seti(source,CSourceProperty::OffsetSeconds,&off);
}

void coffee_audio_source_set_offset_samples(CALSource *source, int32 const& off)
{
    coffee_audio_source_seti(source,CSourceProperty::OffsetSamples,&off);
}

void coffee_audio_source_set_offset_bytes(CALSource *source, int32 const& off)
{
    coffee_audio_source_seti(source,CSourceProperty::OffsetBytes,&off);
}

void coffee_audio_source_transform(
        CALSource *source, const CVec3& position,
        const CVec3& velocity, const CVec3& direction)
{
    source->position = position;
    source->velocity = velocity;
    source->direction = direction;

    coffee_audio_source_setf(source,CSourceProperty::Position,
                             (const scalar*)&source->position);
    coffee_audio_source_setf(source,CSourceProperty::Velocity,
                             (const scalar*)&source->velocity);
    coffee_audio_source_setf(source,CSourceProperty::Direction,
                             (const scalar*)&source->direction);
}

void coffee_audio_source_seti(
        CALSource *source, CSourceProperty const& prop, const int32 &val)
{
    coffee_audio_source_seti(source,prop,&val);
}

void coffee_audio_source_setf(
        CALSource *source, CSourceProperty const& prop, const scalar &val)
{
    coffee_audio_source_setf(source,prop,&val);
}

CALVersion coffee_audio_context_version(CALContext* ctxt)
{
    CALVersion v;
    alcGetIntegerv(ctxt->device,ALC_MAJOR_VERSION,1,&v.major);
    alcGetIntegerv(ctxt->device,ALC_MINOR_VERSION,1,&v.minor);
    return v;
}

cstring *coffee_audio_context_devices_output(int32* numDevices)
{
    *numDevices = 0;

    if(!alcIsExtensionPresent(NULL,"ALC_ENUMERATION_EXT"))
        return nullptr;

    const ALCchar* devices = alcGetString(NULL,ALC_DEVICE_SPECIFIER);

    cstring* arrdev = (cstring*)c_alloc(sizeof(cstring));

    while(*devices)
    {
        if(!(*numDevices))
            arrdev = (cstring*)c_realloc(arrdev,sizeof(cstring)*(*numDevices+1));
        arrdev[*numDevices] = devices;
        devices += c_strlen(devices)+1;
        (*numDevices)++;
    }

    return arrdev;
}

cstring *coffee_audio_context_devices_input(int32* numDevices)
{
    *numDevices = 0;

    if(!alcIsExtensionPresent(NULL,"ALC_ENUMERATION_EXT"))
        return nullptr;

    const ALCchar* cdevices = alcGetString(NULL,ALC_CAPTURE_DEVICE_SPECIFIER);

    cstring* arrdev = (cstring*)c_alloc(sizeof(cstring));

    while(*cdevices)
    {
        if(!(*numDevices))
            arrdev = (cstring*)c_realloc(arrdev,sizeof(cstring)*(*numDevices+1));
        arrdev[*numDevices] = cdevices;
        cdevices += c_strlen(cdevices)+1;
        (*numDevices)++;
    }

    return arrdev;
}

cstring coffee_audio_context_device_default()
{
    return alcGetString(NULL,ALC_DEFAULT_DEVICE_SPECIFIER);
}

CALCaptureDevice *coffee_audio_capture_create(
        CALContext *context, cstring device,
        CAudioFormat const& fmt)
{
    coffee_audio_context_make_current(context);

    CALCaptureDevice* cdev = new CALCaptureDevice;

    cdev->capdevice = alcCaptureOpenDevice(
                device,
                fmt.samplerate,
                _al_get_fmt(fmt),
                fmt.samples*fmt.samplerate*(fmt.bitdepth/8)*fmt.channels);

    cDebug("Creating AL capture device: %s,fq=%i,buffer=%i",
           device,fmt.samplerate,fmt.samples*(fmt.bitdepth/8)*fmt.channels);

    return cdev;
}

void coffee_audio_capture_free(CALCaptureDevice* dev)
{
    alcCaptureCloseDevice(dev->capdevice);
    delete dev;
}

void coffee_audio_capture_start(CALCaptureDevice *dev)
{
    alcCaptureStart(dev->capdevice);
}

void coffee_audio_capture_stop(CALCaptureDevice *dev)
{
    alcCaptureStop(dev->capdevice);
}

void coffee_audio_capture_grab_samples(CALCaptureDevice *dev, CAudioSample &sample)
{
    alcCaptureSamples(dev->capdevice,sample.data,sample.fmt.samples);
}

}
}
}
