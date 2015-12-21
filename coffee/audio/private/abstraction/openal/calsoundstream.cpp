#include <abstraction/openal/calsoundstream.h>
#include <abstraction/openal/calsounddevice.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

CALSoundStream::CALSoundStream(CALSoundDevice &device,
                               CSoundDeviceIdentifier& inputId,
                               const int32 &bufferMultiplier):
    CSoundStream(&device)
{
    b_inputStream = true;
    CSoundFormat& sfmt = device.outputFormat();
    m_capFmt.bitdepth = sfmt.bitDepth();
    m_capFmt.channels = sfmt.channels();
    m_capFmt.samplerate = sfmt.samplerate();
    m_capDev = coffee_audio_capture_create(
                device.alContext(),
                inputId.stringIdentifier(),
                m_capFmt,
                sfmt.sampleSize()*bufferMultiplier);
}

CALSoundStream::CALSoundStream(CALSoundDevice &device,
                               const CSoundFormat& fmt):
    CSoundStream(&device)
{
    b_inputStream = false;
    m_dev = &device;
    m_soundSource = new CALSource;
    coffee_audio_alloc(m_soundSource);
}

const CSoundDevice<CALSource,CALBuffer> &CALSoundStream::device()
{
    return *m_dev;
}

bool CALSoundStream::isInputStream()
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
    coffee_audio_source_queue_buffers(m_soundSource,1,&p_buf);
}

void CALSoundStream::feedBuffer(CSoundBuffer<CALSource, CALBuffer> &buffer)
{
    CALBuffer* p_buf = buffer.object();
    coffee_audio_source_queue_buffers(m_soundSource,1,&p_buf);
}


}
}
}
