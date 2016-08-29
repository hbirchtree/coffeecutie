#include <coffee/audio/abstraction/openal/calsoundmanager.h>

#include <coffee/audio/openal/copenal.h>
#include <coffee/audio/abstraction/openal/calsounddeviceidentifier.h>
#include <coffee/audio/abstraction/openal/calsounddevice.h>
#include <coffee/audio/abstraction/openal/calsoundformat.h>

#include <coffee/core/CDebug>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

CALSoundManager::CALSoundManager()
{
    this->a_idevices = context_devices_input(&idevices);
    this->a_odevices = context_devices_output(&odevices);

    cstring def_str = context_device_default();

    for(uint32 i=0;i<odevices;i++)
    {
        if(StrCmp(a_odevices[i],def_str))
            d_idx = i;
        v_odevices.push_back(CALSoundDeviceIdentifier(i,a_odevices[i]));
    }
    for(uint32 i=0;i<idevices;i++)
        v_idevices.push_back(CALSoundDeviceIdentifier(i,a_odevices[i]));
}

CALSoundManager::~CALSoundManager()
{
    CFree(a_idevices);
    CFree(a_odevices);
}

CSoundDeviceIdentifier &CALSoundManager::defaultSoundDevice()
{
    if(odevices<1)
    {
        cWarning("Failed to find any audio output devices!");
        cWarning("I shall now perform my next trick: RUNOUTTHEWINDOW()");
        RUNOUTTHEWINDOW();
    }
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
    CALSoundDevice* dev = new CALSoundDevice(id);
    if(!dev->is_loaded())
        return nullptr;
    return dev;
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
