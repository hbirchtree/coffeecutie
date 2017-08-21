#ifndef COFFEE_AUDIO_OPENAL_SOUNDMANAGER_H
#define COFFEE_AUDIO_OPENAL_SOUNDMANAGER_H

#include "copenal_common.h"

namespace Coffee{
namespace CAudio{
namespace COpenAL{

using namespace CSoundAbstraction;

class CALSoundManager : public CSoundArbiter<CALSource,CALBuffer>
{
    Vector<cstring> idevices;
    Vector<cstring> odevices;
    Vector<CALSoundDeviceIdentifier> v_idevices;
    Vector<CALSoundDeviceIdentifier> v_odevices;

    uint32 d_idx = 0;

    C_MAYBE_UNUSED uint32 padding = -1;

public:
    CALSoundManager();
    ~CALSoundManager();

    CSoundDeviceIdentifier &defaultSoundDevice();
    uint32 numberSoundDevices();
    uint32 numberSoundInputDevices();
    CSoundDeviceIdentifier &soundDevice(const szptr &devEnum);
    CSoundDeviceIdentifier &soundInputDevice(const szptr &devEnum);
    CSoundDevice<CALSource,CALBuffer> *createDevice(const CSoundDeviceIdentifier &id);
    CSoundDevice<CALSource,CALBuffer> *createInputDevice(
            const CSoundDeviceIdentifier &p_id,
            const CSoundDeviceIdentifier &id);
};

using OpenALAPI = AudioAPI<CALSoundManager,CALSoundFormat,CALSource,CALBuffer>;

}
}
}

#endif
