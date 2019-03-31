#ifndef COFFEE_AUDIO_OPENAL_SOUNDMIXER_H
#define COFFEE_AUDIO_OPENAL_SOUNDMIXER_H

#include "copenal_common.h"
#include "calsounddevice.h"

namespace Coffee{
namespace CAudio{
namespace COpenAL{

class CALSoundMixer : public CSoundMixer<CALSource,CALBuffer>
{
    CSoundDevice<CALSource,CALBuffer> *m_dev;
    Vector<CSoundTrack<CALSource,CALBuffer>*> m_tracks;
public:
    CALSoundMixer(CSoundDevice<CALSource,CALBuffer>& dev);
    ~CALSoundMixer();

    u64 createTrack();
    CSoundTrack<CALSource,CALBuffer> &soundtrack(const u64 &track);
};

}
}
}

#endif
