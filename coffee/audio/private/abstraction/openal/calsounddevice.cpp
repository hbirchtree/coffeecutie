#include <abstraction/openal/calsounddevice.h>

#include <abstraction/openal/calsoundmixer.h>
#include <abstraction/openal/calsoundformat.h>
#include <abstraction/openal/calsoundstream.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

struct _cal_devdata
{
    _cal_devdata():
        m_ctxt(nullptr)
    {
    }

    CALContext* m_ctxt;
    bool b_input;
    CALSoundMixer* m_mixer;
    CALSoundFormat* m_format;
    CALSoundStream* m_inputstream;
};

CALSoundDevice::CALSoundDevice(const CSoundDeviceIdentifier& dev, bool input):
    CSoundDevice(0)
{
    m_data = new _cal_devdata;
    m_data->b_input = input;
    m_data->m_ctxt = coffee_audio_context_create(dev.stringIdentifier());
}

CSoundMixer<CALSource,CALBuffer> &CALSoundDevice::mixer()
{
    return *m_data->m_mixer;
}

CSoundFormat &CALSoundDevice::outputFormat()
{
    return *m_data->m_format;
}

bool CALSoundDevice::isCaptureDevice()
{
    return m_data->b_input;
}

CSoundStream<CALSource,CALBuffer> &CALSoundDevice::captureStreamer()
{
    if(!m_data->b_input)
        throw std::logic_error("This is not an input device!");
    return *m_data->m_inputstream;
}

CALContext *CALSoundDevice::alContext()
{
    return m_data->m_ctxt;
}

}
}
}
