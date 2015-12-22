#include <abstraction/openal/calsounddevice.h>

#include <abstraction/openal/calsoundmixer.h>
#include <abstraction/openal/calsoundformat.h>
#include <abstraction/openal/calsoundstream.h>

#include <abstraction/openal/calsoundstream.h>
#include <abstraction/openal/calsoundbuffer.h>
#include <abstraction/openal/calsoundsample.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

struct _cal_devdata
{
    _cal_devdata():
        m_ctxt(nullptr),
        b_input(false),
        m_mixer(nullptr),
        m_format(nullptr),
        m_inputstream(nullptr)
    {
    }

    CALContext* m_ctxt;
    bool b_input;
    CALSoundMixer* m_mixer;
    CALSoundFormat* m_format;
    CALSoundStream* m_inputstream;
    std::vector<CALSoundStream*> m_streams;
    std::vector<CALSoundSample*> m_samples;
    std::vector<CALSoundBuffer*> m_buffers;
};

CALSoundDevice::CALSoundDevice(const CSoundDeviceIdentifier& dev):
    CSoundDevice(0)
{
    m_data = new _cal_devdata;
    m_data->b_input = false;
    m_data->m_ctxt = coffee_audio_context_create(dev.stringIdentifier());
    m_data->m_mixer = new CALSoundMixer(*this);
}

CALSoundDevice::CALSoundDevice(const CSoundDeviceIdentifier &card,
                               const CSoundDeviceIdentifier &capdev,
                               const CSoundFormat& fmt):
    CSoundDevice(0)
{
    m_data = new _cal_devdata;
    m_data->b_input = true;
    m_data->m_ctxt = coffee_audio_context_create(card.stringIdentifier());
    m_data->m_inputstream = new CALSoundStream(*this,capdev,fmt,4);
}

CALSoundDevice::~CALSoundDevice()
{
    coffee_audio_context_destroy(m_data->m_ctxt);
    if(m_data->m_mixer)
        delete m_data->m_mixer;
    if(m_data->m_inputstream)
        delete m_data->m_inputstream;
    delete m_data;
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

CSoundBuffer<CALSource,CALBuffer> &CALSoundDevice::genBuffer()
{
    szptr i = m_data->m_buffers.size();
    m_data->m_buffers.push_back(new CALSoundBuffer(*this));
    return *m_data->m_buffers[i];
}

CSoundSample<CALSource,CALBuffer> &CALSoundDevice::genSample(CSoundBuffer<CALSource,CALBuffer>& buf,
                                        CSoundFormat& fmt)
{
    szptr i = m_data->m_samples.size();
    m_data->m_samples.push_back(new CALSoundSample(*this,buf,fmt));
    return *m_data->m_samples[i];
}

CSoundStream<CALSource,CALBuffer> &CALSoundDevice::genStream(CSoundFormat& fmt)
{
    szptr i = m_data->m_streams.size();
    m_data->m_streams.push_back(new CALSoundStream(*this,fmt));
    return *m_data->m_streams[i];
}

}
}
}
