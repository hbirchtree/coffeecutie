#include <coffee/audio/abstraction/openal/calsoundmixer.h>

#include <coffee/audio/abstraction/openal/calsoundtrack.h>

namespace Coffee {
namespace CAudio {
namespace COpenAL {

CALSoundMixer::CALSoundMixer(CSoundDevice<CALSource, CALBuffer>& dev)
    : CSoundMixer(&dev)
    , m_dev(&dev)
{
}

CALSoundMixer::~CALSoundMixer()
{
    for(CSoundTrack<CALSource, CALBuffer>* trck : m_tracks)
        delete trck;
}

u64 CALSoundMixer::createTrack()
{
    m_tracks.push_back(new CALSoundTrack(*m_dev));
    return m_tracks.size() - 1;
}

CSoundTrack<CALSource, CALBuffer>& CALSoundMixer::soundtrack(const u64& track)
{
    if(track >= m_tracks.size())
#if defined(COFFEE_USE_EXCEPTIONS)
        throw std::logic_error("Invalid soundtrack index!");
#else
        ABORTEVERYTHINGGOGOGO();
#endif
    return *m_tracks[track];
}

} // namespace COpenAL
} // namespace CAudio
} // namespace Coffee
