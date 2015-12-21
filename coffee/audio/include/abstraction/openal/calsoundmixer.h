#ifndef COFFEE_AUDIO_OPENAL_SOUNDMIXER_H
#define COFFEE_AUDIO_OPENAL_SOUNDMIXER_H

#include <abstraction/openal/copenal_common.h>
#include <abstraction/openal/calsounddevice.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

class CALSoundMixer : public CSoundMixer<CALSource,CALBuffer>
{
    CSoundDevice<CALSource,CALBuffer> *m_dev;
    std::vector<CSoundTrack<CALSource,CALBuffer>*> m_tracks;
public:
    CALSoundMixer(CSoundDevice<CALSource,CALBuffer>& dev);

    uint64 createTrack();
    CSoundTrack<CALSource,CALBuffer> &soundtrack(const uint64 &track);
};

}
}
}

#endif
