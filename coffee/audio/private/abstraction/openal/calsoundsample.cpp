#include <coffee/audio/abstraction/openal/calsoundsample.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

CALSoundSample::CALSoundSample(CSoundDevice<CALSource, CALBuffer> &dev,
                               CSoundBuffer<CALSource,CALBuffer>& buf,
                               CSoundFormat &fmt):
    CSoundSample(&dev),
    m_dev(&dev),
    m_fmt(&fmt),
    m_buffer(&buf),
    m_properties(nullptr)
{
}

CALSoundSample::~CALSoundSample()
{
}

const CSoundDevice<CALSource, CALBuffer> &CALSoundSample::device()
{
    return *m_dev;
}

CSoundBuffer<CALSource, CALBuffer> &CALSoundSample::buffer()
{
    return *m_buffer;
}

CSoundFormat &CALSoundSample::format()
{
    return *m_fmt;
}

uint64 CALSoundSample::samples() const
{
    return m_samples;
}

uint64 CALSoundSample::pts() const
{
    return m_pts;
}

void CALSoundSample::setPts(const uint64& pts)
{
    m_pts = pts;
}

const CSoundProperty *CALSoundSample::properties()
{
    return m_properties;
}

void CALSoundSample::assignProperties(const CSoundProperty *props)
{
    m_properties = props;
}

}
}
}
