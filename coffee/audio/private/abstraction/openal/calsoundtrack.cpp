#include <coffee/audio/abstraction/openal/calsoundtrack.h>


namespace Coffee{
namespace CAudio{
namespace COpenAL{

CALSoundTrack::CALSoundTrack(CSoundDevice<CALSource,CALBuffer>& dev):
    CSoundTrack(&dev),
    m_dev(&dev)
{
    m_source = new CALSource;
    alAlloc(m_source);
}

CALSoundTrack::~CALSoundTrack()
{
    alFree(m_source);
    delete m_source;
}

const CSoundDevice<CALSource, CALBuffer> &CALSoundTrack::device()
{
    return *m_dev;
}

void CALSoundTrack::queueSample(CSoundSample<CALSource, CALBuffer> &sample)
{
    _csample_data f;
    f.buf = sample.buffer().object();
    f.b_pts = sample.pts();

    m_samples.push_back(f);
}

void CALSoundTrack::updateTrack(uint64 ts)
{
    std::vector<const _csample_data*> removal;
    m_samples.sort();
    for(const _csample_data& smp : m_samples)
    {
        if(ts >= smp.b_pts)
        {
            source_queue_buffers(m_source,1,&smp.buf);
            source_set_state(m_source,CALPlaybackState::Playing);
            removal.push_back(&smp);
        }
    }
    for(const _csample_data* r : removal)
        m_samples.remove(*r);
}

CALSource *CALSoundTrack::object()
{
    return m_source;
}

}
}
}
