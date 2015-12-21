#include <abstraction/openal/calsoundbuffer.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

CALSoundBuffer::CALSoundBuffer(CALSoundDevice &dev):
    CSoundBuffer(&dev),
    m_dev(dev)
{
    m_buffer = new CALBuffer;
    coffee_audio_alloc(m_buffer);
}

CALSoundBuffer::~CALSoundBuffer()
{
    coffee_audio_free(m_buffer);
    delete m_buffer;
}

const CSoundDevice<CALSource,CALBuffer> &CALSoundBuffer::device()
{
    return m_dev;
}

szptr CALSoundBuffer::size() const
{
    return i_bufSize;
}

CSoundFormat &CALSoundBuffer::format()
{
    return *m_fmt;
}

void CALSoundBuffer::setFormat(CSoundFormat &fmt)
{
    m_fmt = &fmt;
}

void CALSoundBuffer::fillBuffer(c_cptr data, const szptr &size)
{
    CAudioSample smp;

    smp.fmt.bitdepth = m_fmt->bitDepth();
    smp.fmt.channels = m_fmt->channels();
    smp.fmt.frequency = m_fmt->samplerate();

    smp.samples = size;
    smp.data = (int16*)data;

    coffee_audio_buffer_data(m_buffer,&smp);
}

}
}
}
