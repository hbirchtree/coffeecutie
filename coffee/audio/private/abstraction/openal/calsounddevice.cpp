#include <coffee/audio/abstraction/openal/calsounddevice.h>

#include <coffee/audio/abstraction/openal/calsoundmixer.h>
#include <coffee/audio/abstraction/openal/calsoundformat.h>
#include <coffee/audio/abstraction/openal/calsoundstream.h>

#include <coffee/audio/abstraction/openal/calsoundstream.h>
#include <coffee/audio/abstraction/openal/calsoundbuffer.h>
#include <coffee/audio/abstraction/openal/calsoundsample.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

struct _cal_devdata
{
    _cal_devdata():
        m_ctxt(nullptr),
        m_mixer(nullptr),
        m_format(nullptr),
        m_inputstream(nullptr),
        b_input(false)
    {
    }

    Vector<CALSoundStream*> m_streams;
    Vector<CALSoundSample*> m_samples;
    Vector<CALSoundBuffer*> m_buffers;
    CALContext* m_ctxt;
    CALSoundMixer* m_mixer;
    CALSoundFormat* m_format;
    CALSoundStream* m_inputstream;
    bool b_input;
};

bool CALSoundDevice::is_loaded()
{
    return (bool)m_data->m_ctxt;
}

CALSoundDevice::CALSoundDevice(const CSoundDeviceIdentifier& dev):
    CSoundDevice(0)
{
    m_data = new _cal_devdata;
    m_data->b_input = false;
    m_data->m_ctxt = context_create(dev.stringIdentifier());
    m_data->m_mixer = new CALSoundMixer(*this);
}

CALSoundDevice::CALSoundDevice(const CSoundDeviceIdentifier &card,
                               const CSoundDeviceIdentifier &capdev,
                               const CSoundFormat& fmt):
    CSoundDevice(0)
{
    m_data = new _cal_devdata;
    m_data->b_input = true;
    m_data->m_ctxt = context_create(card.stringIdentifier());
    m_data->m_inputstream = new CALSoundStream(*this,capdev,fmt,4);
}

CALSoundDevice::~CALSoundDevice()
{
    for(CALSoundSample* sample : m_data->m_samples)
        delete sample;
    for(CALSoundStream* stream : m_data->m_streams)
        delete stream;
    for(CALSoundBuffer* buffer : m_data->m_buffers)
        delete buffer;

    if(m_data->m_mixer)
        delete m_data->m_mixer;
    if(m_data->m_inputstream)
        delete m_data->m_inputstream;

    context_destroy(m_data->m_ctxt);

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
#if defined(COFFEE_USE_EXCEPTIONS)
        throw std::logic_error("This is not an input device!");
#else
        ABORTEVERYTHINGGOGOGO();
#endif
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
