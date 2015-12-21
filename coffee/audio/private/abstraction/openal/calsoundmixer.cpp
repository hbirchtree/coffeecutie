#include <abstraction/openal/calsoundmixer.h>

#include <abstraction/openal/calsoundtrack.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

CALSoundMixer::CALSoundMixer(CSoundDevice<CALSource, CALBuffer> &dev):
    CSoundMixer(&dev),
    m_dev(&dev)
{
}

uint64 CALSoundMixer::createTrack()
{
    m_tracks.push_back(new CALSoundTrack(*m_dev));
    return m_tracks.size()-1;
}

CSoundTrack<CALSource, CALBuffer> &CALSoundMixer::soundtrack(const uint64 &track)
{
    if(track>=m_tracks.size())
        throw std::logic_error("Invalid soundtrack index!");
    return *m_tracks[track];
}


}
}
}
