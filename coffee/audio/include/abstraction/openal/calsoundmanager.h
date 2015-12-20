#ifndef COFFEE_AUDIO_OPENAL_SOUNDDEVID_H
#define COFFEE_AUDIO_OPENAL_SOUNDDEVID_H

#include "copenal_common.h"
#include "calsounddeviceidentifier.h"

namespace Coffee{
namespace CAudio{
namespace COpenAL{

class CALSoundManager : public CSoundManager
{
    uint32 d_idx = 0;

    uint32 odevices = 0;
    uint32 idevices = 0;
    cstring* a_idevices = nullptr;
    cstring* a_odevices = nullptr;
    std::vector<CALSoundDeviceIdentifier> v_idevices;
    std::vector<CALSoundDeviceIdentifier> v_odevices;

public:
    CALSoundManager();

    CSoundDeviceIdentifier &defaultSoundDevice();
    uint32 numberSoundDevices();
    uint32 numberSoundInputDevices();
    CSoundDeviceIdentifier &soundDevice(const szptr &devEnum);
    CSoundDeviceIdentifier &soundInputDevice(const szptr &devEnum);
    CSoundDevice *createDevice(const CSoundDeviceIdentifier &id);
    CSoundDevice *createInputDevice(const CSoundDeviceIdentifier &id);
};

}
}
}

#endif
