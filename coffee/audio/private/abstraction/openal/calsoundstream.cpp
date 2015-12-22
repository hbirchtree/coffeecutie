#include <abstraction/openal/calsoundstream.h>
#include <abstraction/openal/calsounddevice.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

CALSoundStream::CALSoundStream(CALSoundDevice &device,
                               const CSoundDeviceIdentifier& inputId,
                               const CSoundFormat& fmt,
                               const int32 &bufferMultiplier):
    CSoundStream(&device)
{
    b_inputStream = true;
    m_capFmt.bitdepth = fmt.bitDepth();
    m_capFmt.channels = fmt.channels();
    m_capFmt.samplerate = fmt.samplerate();
    m_capDev = coffee_audio_capture_create(
                device.alContext(),
                inputId.stringIdentifier(),
                m_capFmt,
                fmt.sampleSize()*bufferMultiplier);
}

CALSoundStream::CALSoundStream(CALSoundDevice &device,
                               const CSoundFormat& fmt):
    CSoundStream(&device)
{
    b_inputStream = false;
    m_dev = &device;
    m_soundSource = new CALSource;
    coffee_audio_alloc(m_soundSource);
    m_capFmt.bitdepth = fmt.bitDepth();
    m_capFmt.channels = fmt.channels();
    m_capFmt.frequency = fmt.samplerate();
}

const CSoundDevice<CALSource,CALBuffer> &CALSoundStream::device()
{
    return *m_dev;
}

bool CALSoundStream::isInputStream() const
{
    return b_inputStream;
}

szptr CALSoundStream::collectSamples(c_ptr data, const szptr &max_samples)
{
    if(!b_inputStream)
        return 0;

    CAudioSample smp;
    smp.data = (int16*)data;
    smp.samples = max_samples;
    smp.fmt = m_capFmt;

    coffee_audio_capture_grab_samples(m_capDev,smp);

    return max_samples;
}

void CALSoundStream::feedData(c_cptr data, const CSoundFormat &fmt, const szptr &samples)
{
    CALBuffer buf;
    CALBuffer* p_buf = &buf;

    coffee_audio_alloc(p_buf);

    CAudioSample samp;
    samp.data = (int16*)data;
    samp.fmt.bitdepth = fmt.bitDepth();
    samp.fmt.channels = fmt.channels();
    samp.fmt.frequency = fmt.samplerate();

    samp.samples = samples;

    coffee_audio_buffer_data(p_buf,&samp);

    coffee_audio_source_queue_buffers(m_soundSource,1,&p_buf);
}

void CALSoundStream::feedBuffer(CSoundBuffer<CALSource, CALBuffer> &buffer)
{
    CALBuffer* p_buf = buffer.object();
    coffee_audio_source_queue_buffers(m_soundSource,1,&p_buf);
}

CALSource *CALSoundStream::object()
{
    return m_soundSource;
}


}
}
}
