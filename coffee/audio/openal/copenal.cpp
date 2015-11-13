#include "copenal.h"

namespace Coffee{
namespace CAudio{
namespace COpenAL{

static CALCallback error_callback = nullptr;

constexpr struct {CSourceProperty k; ALenum v;} al_source_prop_map[12] = {
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
};

ALenum _al_get_prop(CSourceProperty p)
{
    for(size_t i=0;i<12;i++)
        if(al_source_prop_map[i].k==p)
            return al_source_prop_map[i].v;
    return AL_NONE;
}

bool coffee_audio_context_create(CALContext *context)
{
    cDebug("AL enumeration support: %i",
           coffee_audio_context_check_extension(context,"ALC_ENUMERATION_EXT"));

    const ALCchar* defaultDev = alcGetString(NULL,ALC_DEFAULT_DEVICE_SPECIFIER);

    context->device = alcOpenDevice(defaultDev);
    if(!context->device)
    {
        cWarning("Failed to open ALC device");
        return false;
    }

    cMsg("ALC","Initialized, using device: %s",
         alcGetString(context->device,ALC_DEVICE_SPECIFIER));

    context->context = alcCreateContext(context->device,NULL);

    coffee_audio_context_make_current(context);
    return true;
}

void coffee_audio_context_destroy(CALContext *context)
{
    if(context->context_thread==std::this_thread::get_id())
        alcMakeContextCurrent(nullptr);

    alcDestroyContext(context->context);
    if(alcCloseDevice(context->device)!=ALC_TRUE)
        cWarning("Failed to close AL device");
    context->context = nullptr;
    context->device = nullptr;
    cMsg("ALC","Destroyed");
}

bool coffee_audio_context_make_current(CALContext *context)
{
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
        error_callback(rep);
        delete rep;
    }
}

bool coffee_audio_context_check_extension(const CALContext *context, cstring extension)
{
    return alcIsExtensionPresent(context->device,extension)==ALC_TRUE;
}

void coffee_audio_alloc(CALBuffer *buffer, CAudioSample *sample)
{
    coffee_audio_alloc(buffer);
    alBufferData(
                *buffer,AL_FORMAT_STEREO16,
                sample->data,
                sample->fmt.samples*sample->fmt.channels*sizeof(sample->data[0]),
                sample->fmt.samplerate);
    coffee_audio_context_get_error();
}

void coffee_audio_alloc(CALBuffer *buffer)
{
    alGenBuffers(1,buffer);
    coffee_audio_context_get_error();
}

void coffee_audio_alloc(CALSource *source)
{
    alGenSources(1,&source->handle);
    coffee_audio_context_get_error();
}

void coffee_audio_free(CALSource *source)
{
    alDeleteSources(1,&source->handle);
    coffee_audio_context_get_error();
    source->handle = 0;
}

void coffee_audio_free(CALBuffer *buffer)
{
    alDeleteBuffers(1,buffer);
    coffee_audio_context_get_error();
    buffer = 0;
}

int32 coffee_audio_source_get_offset_seconds(const CALSource *source)
{
    int32 i;
    alGetSourcei(source->handle,AL_SEC_OFFSET,&i);
    coffee_audio_context_get_error();
    return i;
}

int32 coffee_audio_source_get_offset_samples(const CALSource *source)
{
    int32 i;
    alGetSourcei(source->handle,AL_SAMPLE_OFFSET,&i);
    coffee_audio_context_get_error();
    return i;
}

int32 coffee_audio_source_get_offset_bytes(const CALSource *source)
{
    int32 i;
    alGetSourcei(source->handle,AL_BYTE_OFFSET,&i);
    coffee_audio_context_get_error();
    return i;
}

void coffee_audio_source_set_offset_seconds(CALSource *source, int32 off)
{
    alSourcei(source->handle,AL_SEC_OFFSET,off);
    coffee_audio_context_get_error();
}

void coffee_audio_source_set_offset_samples(CALSource *source, int32 off)
{
    alSourcei(source->handle,AL_SAMPLE_OFFSET,off);
    coffee_audio_context_get_error();
}

void coffee_audio_source_set_offset_bytes(CALSource *source, int32 off)
{
    alSourcei(source->handle,AL_BYTE_OFFSET,off);
    coffee_audio_context_get_error();
}

void coffee_audio_source_set_state(CALSource *source, CALPlaybackState state)
{
    switch(state)
    {
    case CALPlaybackState::Stopped:
        alSourceStop(source->handle);
        break;
    case CALPlaybackState::Playing:
        alSourcePlay(source->handle);
        break;
    case CALPlaybackState::Paused:
        alSourcePause(source->handle);
        break;
    case CALPlaybackState::Rewind:
        alSourceRewind(source->handle);
        break;
    }
    coffee_audio_context_get_error();
}

void coffee_audio_source_set_states(
        const CALSource **sources, szptr numSources, CALPlaybackState state)
{
    ALuint *src = new ALuint[numSources];
    for(szptr i=0;i<numSources;i++)
        src[i] = sources[i]->handle;

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

void coffee_audio_listener_set(const CALListener *listener)
{
    alListenerf(AL_GAIN,listener->gain);
    coffee_audio_context_get_error();

    alListenerfv(AL_POSITION,(scalar*)&listener->position);
    coffee_audio_context_get_error();
    alListenerfv(AL_VELOCITY,(scalar*)&listener->velocity);
    coffee_audio_context_get_error();
    scalar *orient = new scalar[6];
    memcpy(&orient[0],&listener->orientation_forward,sizeof(CVec3));
    memcpy(&orient[2],&listener->orientation_forward,sizeof(CVec3));
    alListenerfv(AL_ORIENTATION,orient);
    delete[] orient;
    coffee_audio_context_get_error();
}

void coffee_audio_source_transform(
        CALSource *source, const CVec3& position,
        const CVec3& velocity, const CVec3& direction)
{
    memcpy((void*)&source->position,&position,sizeof(position));
    memcpy((void*)&source->velocity,&velocity,sizeof(velocity));
    memcpy((void*)&source->direction,&direction,sizeof(direction));

    alSourcefv(source->handle,AL_POSITION,(scalar*)&source->position);
    coffee_audio_context_get_error();
    alSourcefv(source->handle,AL_VELOCITY,(scalar*)&source->velocity);
    coffee_audio_context_get_error();
//    alSourcefv(source->handle,AL_DIRECTION,(scalar*)&source->direction);
    coffee_audio_context_get_error();
}

void coffee_audio_source_queue_buffers(
        CALSource *source, szptr numBuffers, CALBuffer **buffers)
{
    alSourceQueueBuffers(source->handle,numBuffers,*buffers);
    coffee_audio_context_get_error();
}

void coffee_audio_source_dequeue_buffers(
        CALSource *source, szptr numBuffers, CALBuffer **buffers)
{
    alSourceUnqueueBuffers(source->handle,numBuffers,*buffers);
    coffee_audio_context_get_error();
}

void coffee_audio_source_seti(CALSource *source, CSourceProperty prop, int32 val)
{
    alSourcei(source->handle,_al_get_prop(prop),val);
    coffee_audio_context_get_error();
}

void coffee_audio_source_setf(CALSource *source, CSourceProperty prop, scalar val)
{
    alSourcef(source->handle,_al_get_prop(prop),val);
    coffee_audio_context_get_error();
}

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
    handle(0),
    state(AL_STOPPED)
{
}

}
}
}
