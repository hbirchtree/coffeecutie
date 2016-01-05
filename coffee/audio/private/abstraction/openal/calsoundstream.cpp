#include <abstraction/openal/calsoundstream.h>
#include <abstraction/openal/calsounddevice.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

CALSoundStream::CALSoundStream(CALSoundDevice &device,
                               const CSoundDeviceIdentifier& inputId,
                               const CSoundFormat& fmt,
                               const int32 &bufferMultiplier):
    CSoundStream(&device),
    m_soundSource(nullptr)
{
    b_inputStream = true;
    m_capFmt.bitdepth = fmt.bitDepth();
    m_capFmt.channels = fmt.channels();
    m_capFmt.samplerate = fmt.samplerate();
    m_capDev = capture_create(
                device.alContext(),
                inputId.stringIdentifier(),
                m_capFmt,
                fmt.sampleSize()*bufferMultiplier);
}

CALSoundStream::CALSoundStream(CALSoundDevice &device,
                               const CSoundFormat& fmt):
    CSoundStream(&device),
    m_capDev(nullptr)
{
    b_inputStream = false;
    m_dev = &device;
    m_soundSource = new CALSource;
    alAlloc(m_soundSource);
    m_capFmt.bitdepth = fmt.bitDepth();
    m_capFmt.channels = fmt.channels();
    m_capFmt.frequency = fmt.samplerate();
}

CALSoundStream::~CALSoundStream()
{
    if(m_soundSource)
    {
        alFree(m_soundSource);
        delete m_soundSource;
    }
    if(m_capDev)
    {
        capture_free(m_capDev);
    }
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

    capture_grab_samples(m_capDev,smp);

    return max_samples;
}

void CALSoundStream::feedData(c_cptr data, const CSoundFormat &fmt, const szptr &samples)
{
    CALBuffer buf;
    CALBuffer* p_buf = &buf;

    alAlloc(p_buf);

    CAudioSample samp;
    samp.data = (int16*)data;
    samp.fmt.bitdepth = fmt.bitDepth();
    samp.fmt.channels = fmt.channels();
    samp.fmt.frequency = fmt.samplerate();

    samp.samples = samples;

    buffer_data(p_buf,&samp);

    source_queue_buffers(m_soundSource,1,&p_buf);
    source_set_state(m_soundSource,CALPlaybackState::Playing);
}

void CALSoundStream::feedBuffer(CSoundBuffer<CALSource, CALBuffer> &buffer)
{
    CALBuffer* p_buf = buffer.object();
    source_queue_buffers(m_soundSource,1,&p_buf);
}

CALSource *CALSoundStream::object()
{
    return m_soundSource;
}


}
}
}
