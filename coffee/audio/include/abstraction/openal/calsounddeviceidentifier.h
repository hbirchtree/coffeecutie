#ifndef COFFEE_AUDIO_OPENAL_SOUNDDEVID_H
#define COFFEE_AUDIO_OPENAL_SOUNDDEVID_H

#include <abstraction/openal/copenal_common.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

class CALSoundDeviceIdentifier : public CSoundDeviceIdentifier
{
    szptr i_idx;
    cstring s_id;
public:
    CALSoundDeviceIdentifier(const szptr& idx, cstring sid);

    szptr deviceIndex() const;
    cstring stringIdentifier() const;
};

}
}
}

#endif
