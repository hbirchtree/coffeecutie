#ifndef COFFEE_AUDIO_OPENAL_SOUNDDEVID_H
#define COFFEE_AUDIO_OPENAL_SOUNDDEVID_H

#include "../../openal/copenal.h"
#include <coffee/interfaces/caudiomixer_api.h>

namespace Coffee {
namespace CAudio {
namespace COpenAL {

using namespace CSoundAbstraction;

class CALSoundDeviceIdentifier : public CSoundDeviceIdentifier
{
    szptr   i_idx;
    cstring s_id;

  public:
    CALSoundDeviceIdentifier(szptr idx, cstring sid);
    virtual ~CALSoundDeviceIdentifier();

    szptr   deviceIndex() const;
    cstring stringIdentifier() const;
};

} // namespace COpenAL
} // namespace CAudio
} // namespace Coffee

#endif
