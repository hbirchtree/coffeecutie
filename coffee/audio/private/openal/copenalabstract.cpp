#include <openal/copenalabstract.h>

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

CSoundDevice* CALSoundManager::createDevice(const CSoundDeviceIdentifier &id)
{
    return new CALSoundDevice(id,false);
}

CSoundDevice* CALSoundManager::createInputDevice(const CSoundDeviceIdentifier &id)
{
    return new CALSoundDevice(id,true);
}

CALSoundDeviceIdentifier::CALSoundDeviceIdentifier(const szptr &idx, cstring sid):
    i_idx(idx),
    s_id(sid)
{
}
szptr CALSoundDeviceIdentifier::deviceIndex() const
{
    return i_idx;
}
cstring CALSoundDeviceIdentifier::stringIdentifier() const
{
    return s_id;
}

CALSoundDevice::CALSoundDevice(const CSoundDeviceIdentifier& dev, bool input):
    b_input(input)
{
    m_ctxt = coffee_audio_context_create(dev.stringIdentifier());
}

CSoundMixer &CALSoundDevice::mixer()
{
    return m_mixer;
}

CSoundFormat &CALSoundDevice::outputFormat()
{
    return m_format;
}

bool CALSoundDevice::isCaptureDevice()
{
    return b_input;
}

CSoundStream &CALSoundDevice::captureStreamer()
{
    if(!b_input)
        throw std::logic_error("This is not an input device!");
    return m_inputstream;
}

CALSoundFormat::CALSoundFormat()
{
}

}
}
}
