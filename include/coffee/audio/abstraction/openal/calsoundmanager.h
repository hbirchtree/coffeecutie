#ifndef COFFEE_AUDIO_OPENAL_SOUNDMANAGER_H
#define COFFEE_AUDIO_OPENAL_SOUNDMANAGER_H

#include "copenal_common.h"

namespace Coffee{
namespace CAudio{
namespace COpenAL{

using namespace CSoundAbstraction;

class CALSoundManager : public CSoundArbiter<CALSource,CALBuffer>
{
    uint32 d_idx = 0;

    uint32 odevices = 0;
    uint32 idevices = 0;
    cstring* a_idevices = nullptr;
    cstring* a_odevices = nullptr;
    Vector<CALSoundDeviceIdentifier> v_idevices;
    Vector<CALSoundDeviceIdentifier> v_odevices;

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
