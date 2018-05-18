#ifndef COFFEE_AUDIO_OPENAL_SOUNDSTREAM_H
#define COFFEE_AUDIO_OPENAL_SOUNDSTREAM_H

#include "copenal_common.h"
#include "calsounddevice.h"

namespace Coffee{
namespace CAudio{
namespace COpenAL{

class CALSoundStream : public CSoundStream<CALSource,CALBuffer>
{
    CALSoundDevice* m_dev;

    CALCaptureDevice* m_capDev;

    CALSource* m_soundSource;
    AudioFormat m_capFmt;
    bool b_inputStream;

    Queue<CALBuffer*> m_available;
    Vector<CALBuffer*> m_expended;

public:
    CALSoundStream(CALSoundDevice &device,
                   const CSoundDeviceIdentifier &inputId,
                   const CSoundFormat &fmt,
                   const int32& bufferMultiplier);
    CALSoundStream(CALSoundDevice& device,
                   const CSoundFormat &fmt);

    ~CALSoundStream();

    const CSoundDevice<CALSource,CALBuffer> &device();

    bool isInputStream() const;
    szptr collectSamples(c_ptr data, const szptr &max_samples);

    void feedData(c_cptr data, const CSoundFormat &fmt, const szptr &samples);
    void feedBuffer(CSoundBuffer<CALSource,CALBuffer> &buffer);

    CALSource* object();

    void startStream();
    void stopStream();
    void pauseStream();
private:
    void collectBuffers();
};

}
}
}

#endif
