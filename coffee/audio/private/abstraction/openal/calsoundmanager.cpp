#include <abstraction/openal/calsoundmanager.h>

#include <openal/copenal.h>
#include <abstraction/openal/calsounddeviceidentifier.h>
#include <abstraction/openal/calsounddevice.h>
#include <abstraction/openal/calsoundformat.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

CALSoundManager::CALSoundManager()
{
    this->a_idevices = coffee_audio_context_devices_input(&idevices);
    this->a_odevices = coffee_audio_context_devices_output(&odevices);

    cstring def_str = coffee_audio_context_device_default();

    for(uint32 i=0;i<odevices;i++)
    {
        if(c_strcmp(a_odevices[i],def_str))
            d_idx = i;
        v_odevices.push_back(CALSoundDeviceIdentifier(i,a_odevices[i]));
    }
    for(uint32 i=0;i<idevices;i++)
        v_idevices.push_back(CALSoundDeviceIdentifier(i,a_odevices[i]));
}

CSoundDeviceIdentifier &CALSoundManager::defaultSoundDevice()
{
    return v_odevices[d_idx];
}

uint32 CALSoundManager::numberSoundDevices()
{
    return odevices;
}

uint32 CALSoundManager::numberSoundInputDevices()
{
    return idevices;
}

CSoundDeviceIdentifier &CALSoundManager::soundDevice(const szptr &devEnum)
{
    return v_odevices[devEnum];
}

CSoundDeviceIdentifier &CALSoundManager::soundInputDevice(const szptr &devEnum)
{
    return v_idevices[devEnum];
}

CSoundDevice<CALSource,CALBuffer>* CALSoundManager::createDevice(const CSoundDeviceIdentifier &id)
{
    return new CALSoundDevice(id);
}

CSoundDevice<CALSource, CALBuffer> *CALSoundManager::createInputDevice(
        const CSoundDeviceIdentifier &p_id,
        const CSoundDeviceIdentifier &id)
{
    CALSoundFormat fmt;
    fmt.setBitDepth(16);
    fmt.setChannels(1);
    fmt.setSamplerate(44100);
    return new CALSoundDevice(p_id,id,fmt);
}

}
}
}
