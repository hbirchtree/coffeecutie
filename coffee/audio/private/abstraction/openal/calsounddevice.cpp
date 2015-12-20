#include <abstraction/openal/calsounddevice.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

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

}
}
}
