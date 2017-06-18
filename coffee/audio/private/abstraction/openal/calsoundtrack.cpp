#include <coffee/audio/abstraction/openal/calsoundtrack.h>


namespace Coffee{
namespace CAudio{
namespace COpenAL{

static inline void applyListenerProperties(CALSource* src, const CListenerProperty* props)
{

}

static inline void applySourceProperties(CALSource* src, const CSoundProperty* props)
{
    using P = CSoundProperty;

    for(u32 i=0;i<P::ActiveProperty_Num;i++)
    {
        u32 attr = 1 << i;
        i32 b;
        if(props->active & attr)
            switch(attr)
            {
            case P::Looping:
                b = props->looping;
                source_seti(src, CSourceProperty::Looping, &b);
                break;
            default:
                break;
            }
    }
}

CALSoundTrack::CALSoundTrack(CSoundDevice<CALSource,CALBuffer>& dev):
    CSoundTrack(&dev),
    m_dev(&dev),
    m_defaultProp(nullptr),
    m_listenProp(nullptr)
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
    f.prop = sample.properties();

    m_samples.push_back(f);
}

void CALSoundTrack::updateTrack(uint64 ts)
{
    Vector<_csample_data*> removal;
    m_samples.sort();

    if(m_listenProp)
        applyListenerProperties(m_source, m_listenProp);
    if(m_defaultProp)
        applySourceProperties(m_source, m_defaultProp);

    for(_csample_data& smp : m_samples)
    {
        if(ts >= smp.b_pts)
        {
            if(smp.prop)
                applySourceProperties(m_source, smp.prop);
            source_queue_buffers(m_source,1,&smp.buf);
            source_set_state(m_source,CALPlaybackState::Playing);
            removal.push_back(&smp);
        }
    }
    for(_csample_data* r : removal)
        m_samples.remove(*r);
}

CALSource *CALSoundTrack::object()
{
    return m_source;
}

void CALSoundTrack::assignProperties(const CSoundProperty *props)
{
    m_defaultProp = props;
}

void CALSoundTrack::assignProperties(const CListenerProperty *props)
{
    m_listenProp = props;
}

}
}
}
