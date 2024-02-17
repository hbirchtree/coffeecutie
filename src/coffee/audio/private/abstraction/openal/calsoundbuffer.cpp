#include <coffee/audio/abstraction/openal/calsoundbuffer.h>
#include <coffee/core/CProfiling>

namespace Coffee {
namespace CAudio {
namespace COpenAL {

CALSoundBuffer::CALSoundBuffer(CALSoundDevice& dev)
    : CSoundBuffer(&dev)
    , m_dev(dev)
{
    m_buffer = new CALBuffer;
    alAlloc(m_buffer);
}

CALSoundBuffer::~CALSoundBuffer()
{
    alFree(m_buffer);
    delete m_buffer;
}

const CSoundDevice<CALSource, CALBuffer>& CALSoundBuffer::device()
{
    return m_dev;
}

szptr CALSoundBuffer::size() const
{
    return i_bufSize;
}

CSoundFormat& CALSoundBuffer::format()
{
    return *m_fmt;
}

void CALSoundBuffer::setFormat(CSoundFormat& fmt)
{
    m_fmt = &fmt;
}

void CALSoundBuffer::fillBuffer(c_cptr data, const szptr& size)
{
    DProfContext _("Uploading audio sample");

    AudioSample smp;

    smp.fmt.bitdepth  = m_fmt->bitDepth();
    smp.fmt.channels  = m_fmt->channels();
    smp.fmt.frequency = m_fmt->samplerate();

    smp.samples = C_FCAST<u32>(size);
    smp.data    = C_RCAST<i16*>(C_CCAST<c_ptr>(data));

    buffer_data(m_buffer, &smp);
}

CALBuffer* CALSoundBuffer::object()
{
    return m_buffer;
}

} // namespace COpenAL
} // namespace CAudio
} // namespace Coffee
