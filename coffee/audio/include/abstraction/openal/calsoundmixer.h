#ifndef COFFEE_AUDIO_OPENAL_SOUNDMIXER_H
#define COFFEE_AUDIO_OPENAL_SOUNDMIXER_H

#include <abstraction/openal/copenal_common.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

class CALSoundMixer : public CSoundMixer
{
public:
    uint64 createTrack();
    CSoundTrack &soundtrack(const uint64 &track);
};

}
}
}

#endif
