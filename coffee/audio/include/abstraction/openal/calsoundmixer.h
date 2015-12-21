#ifndef COFFEE_AUDIO_OPENAL_SOUNDMIXER_H
#define COFFEE_AUDIO_OPENAL_SOUNDMIXER_H

#include <abstraction/openal/copenal_common.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

class CALSoundMixer : public CSoundMixer<CALSource,CALBuffer>
{
public:
    uint64 createTrack();
    CSoundTrack<CALSource,CALBuffer> &soundtrack(const uint64 &track);
};

}
}
}

#endif
