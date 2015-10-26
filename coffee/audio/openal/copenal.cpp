#include "copenal.h"

namespace Coffee{
namespace CAudio{
namespace COpenAL{

bool coffee_audio_context_create(CALContext *context)
{
    context->device = alcOpenDevice(NULL);
    if(!context->device)
    {
        cWarning("Failed to open ALC device");
        return false;
    }
    cMsg("ALC","Initialized");
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
    if(err!=ALC_NO_ERROR&&context->callback)
    {
        CALReport* rep = new CALReport;
        context->callback(rep);
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
                buffer->handle,AL_FORMAT_STEREO16,
                sample->data,
                sample->fmt.samples*sample->fmt.channels*sizeof(sample->data[0]),
                sample->fmt.samplerate);
}

void coffee_audio_alloc(CALBuffer *buffer)
{
    alGenBuffers(1,&buffer->handle);
}

void coffee_audio_alloc(CALSource *source)
{
    alGenSources(1,&source->handle);
}

void coffee_audio_free(CALSource *source)
{
    alDeleteSources(1,&source->handle);
    source->handle = 0;
}

void coffee_audio_free(CALBuffer *buffer)
{
    alDeleteBuffers(1,&buffer->handle);
    buffer->handle = 0;
}

void coffee_audio_source_set(CALSource *source, const CALSourceData *data)
{
    alSourcef(source->handle,AL_PITCH,data->pitch);
    alSourcef(source->handle,AL_GAIN,data->gain);

    alSourcef(source->handle,AL_MAX_DISTANCE,data->max_dist);
    alSourcef(source->handle,AL_ROLLOFF_FACTOR,data->rolloff_dist);
    alSourcef(source->handle,AL_REFERENCE_DISTANCE,data->ref_dist);

    alSourcef(source->handle,AL_MIN_GAIN,data->min_gain);
    alSourcef(source->handle,AL_MAX_GAIN,data->max_gain);

    alSourcef(source->handle,AL_CONE_OUTER_GAIN,data->cone_out_gain);
    alSourcef(source->handle,AL_CONE_INNER_ANGLE,data->cone_in_angle);
    alSourcef(source->handle,AL_CONE_OUTER_ANGLE,data->cone_out_angle);

    alSourcei(source->handle,AL_SOURCE_RELATIVE,data->source_relative);
    alSourcei(source->handle,AL_LOOPING,data->looping);
}

int32 coffee_audio_source_get_offset_seconds(const CALSource *source)
{
    int32 i;
    alGetSourcei(source->handle,AL_SEC_OFFSET,&i);
    return i;
}

int32 coffee_audio_source_get_offset_samples(const CALSource *source)
{
    int32 i;
    alGetSourcei(source->handle,AL_SAMPLE_OFFSET,&i);
    return i;
}

int32 coffee_audio_source_get_offset_bytes(const CALSource *source)
{
    int32 i;
    alGetSourcei(source->handle,AL_BYTE_OFFSET,&i);
    return i;
}

void coffee_audio_source_set_offset_seconds(CALSource *source, int32 off)
{
    alSourcei(source->handle,AL_SEC_OFFSET,off);
}

void coffee_audio_source_set_offset_samples(CALSource *source, int32 off)
{
    alSourcei(source->handle,AL_SAMPLE_OFFSET,off);
}

void coffee_audio_source_set_offset_bytes(CALSource *source, int32 off)
{
    alSourcei(source->handle,AL_BYTE_OFFSET,off);
}

void coffee_audio_source_set_state(CALSource *source, CALPlaybackState state)
{
    switch(state)
    {
    case Coffee::CAudio::COpenAL::CALStateStopped:
        alSourceStop(source->handle);
        break;
    case Coffee::CAudio::COpenAL::CALStatePlaying:
        alSourcePlay(source->handle);
        break;
    case Coffee::CAudio::COpenAL::CALStatePaused:
        alSourcePause(source->handle);
        break;
    case Coffee::CAudio::COpenAL::CALStateRewind:
        alSourceRewind(source->handle);
        break;
    }
}

void coffee_audio_source_set_states(
        const CALSource **sources, szptr numSources, CALPlaybackState state)
{
    ALuint src[numSources];
    for(szptr i=0;i<numSources;i++)
        src[i] = sources[i]->handle;

    switch(state)
    {
    case Coffee::CAudio::COpenAL::CALStateStopped:
        alSourceStopv(numSources,src);
        break;
    case Coffee::CAudio::COpenAL::CALStatePlaying:
        alSourcePlayv(numSources,src);
        break;
    case Coffee::CAudio::COpenAL::CALStatePaused:
        alSourcePausev(numSources,src);
        break;
    case Coffee::CAudio::COpenAL::CALStateRewind:
        alSourceRewindv(numSources,src);
        break;
    }
}

void coffee_audio_listener_set(const CALListener *listener)
{
    alListenerf(AL_GAIN,listener->gain);

    alListenerfv(AL_POSITION,
                 (scalar*)&listener->position);
    alListenerfv(AL_VELOCITY,
                 (scalar*)&listener->velocity);
    alListenerfv(AL_ORIENTATION,
                 (scalar*)&listener->orientation_forward);
}

void coffee_audio_source_transform(
        CALSource *source, const CVec3& position,
        const CVec3& velocity, const CVec3& direction)
{
    memcpy((void*)&source->position,&position,sizeof(position));
    memcpy((void*)&source->velocity,&velocity,sizeof(velocity));
    memcpy((void*)&source->direction,&direction,sizeof(direction));

    alSourcefv(source->handle,AL_POSITION,(scalar*)&source->position);
    alSourcefv(source->handle,AL_VELOCITY,(scalar*)&source->velocity);
    alSourcefv(source->handle,AL_DIRECTION,(scalar*)&source->direction);
}

void coffee_audio_source_queue_buffers(
        CALSource *source, szptr numBuffers, CALBuffer **buffers)
{
    ALuint buf[numBuffers];
    for(szptr i=0;i<numBuffers;i++)
        buf[i] = buffers[i]->handle;
    alSourceQueueBuffers(source->handle,numBuffers,buf);
}

void coffee_audio_source_dequeue_buffers(
        CALSource *source, szptr numBuffers, CALBuffer **buffers)
{
    ALuint buf[numBuffers];
    for(szptr i=0;i<numBuffers;i++)
        buf[i] = buffers[i]->handle;
    alSourceUnqueueBuffers(source->handle,numBuffers,buf);
}

}
}
}
