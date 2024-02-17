#include <coffee/audio/abstraction/openal/calsoundmanager.h>

#include <coffee/core/CProfiling>

#include <coffee/audio/abstraction/openal/calsounddevice.h>
#include <coffee/audio/abstraction/openal/calsounddeviceidentifier.h>
#include <coffee/audio/abstraction/openal/calsoundformat.h>
#include <coffee/audio/openal/copenal.h>

#include <coffee/core/CDebug>

namespace Coffee {
namespace CAudio {
namespace COpenAL {

CALSoundManager::CALSoundManager()
{
    DProfContext _("Enumerating audio devices");

    context_devices_input(idevices);
    context_devices_output(odevices);

    cstring def_str = context_device_default();

    szptr i = 0;
    for(cstring dev : odevices)
    {
        if(libc::str::cmp(odevices[i], def_str))
            d_idx = i;
        v_odevices.push_back(CALSoundDeviceIdentifier(i, dev));
        i++;
    }
    i = 0;
    for(cstring dev : idevices)
    {
        v_idevices.push_back(CALSoundDeviceIdentifier(i, dev));
        i++;
    }
}

CALSoundManager::~CALSoundManager()
{
}

CSoundDeviceIdentifier& CALSoundManager::defaultSoundDevice()
{
    if(odevices.size() < 1)
        Throw(undefined_behavior("No sound device found"));

    return v_odevices[d_idx];
}

u32 CALSoundManager::numberSoundDevices()
{
    return odevices.size();
}

u32 CALSoundManager::numberSoundInputDevices()
{
    return idevices.size();
}

CSoundDeviceIdentifier& CALSoundManager::soundDevice(const szptr& devEnum)
{
    return v_odevices[devEnum];
}

CSoundDeviceIdentifier& CALSoundManager::soundInputDevice(const szptr& devEnum)
{
    return v_idevices[devEnum];
}

CSoundDevice<CALSource, CALBuffer>* CALSoundManager::createDevice(
    const CSoundDeviceIdentifier& id)
{
    CALSoundDevice* dev = new CALSoundDevice(id);
    if(!dev->is_loaded())
        return nullptr;
    return dev;
}

CSoundDevice<CALSource, CALBuffer>* CALSoundManager::createInputDevice(
    const CSoundDeviceIdentifier& p_id, const CSoundDeviceIdentifier& id)
{
    CALSoundFormat fmt;
    fmt.setBitDepth(16);
    fmt.setChannels(1);
    fmt.setSamplerate(44100);
    return new CALSoundDevice(p_id, id, fmt);
}

} // namespace COpenAL
} // namespace CAudio
} // namespace Coffee
