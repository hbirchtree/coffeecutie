#ifndef COFFEE_AUDIO_OPENAL_SOUNDDEV_H
#define COFFEE_AUDIO_OPENAL_SOUNDDEV_H

#include "copenal_common.h"

namespace Coffee{
namespace CAudio{
namespace COpenAL{

struct _cal_devdata;

class CALSoundDevice : public CSoundDevice<CALSource,CALBuffer>
{
    friend class CALSoundManager;

    _cal_devdata* m_data;

    bool is_loaded();

public:
    CALSoundDevice(const CSoundDeviceIdentifier &dev);
    CALSoundDevice(const CSoundDeviceIdentifier &card,
                   const CSoundDeviceIdentifier &capdev,
                   const CSoundFormat &fmt);
    ~CALSoundDevice();

    CSoundMixer<CALSource,CALBuffer> &mixer();
    CSoundFormat &outputFormat();
    bool isCaptureDevice();
    CSoundStream<CALSource,CALBuffer> &captureStreamer();

    CALContext* alContext();

    CSoundBuffer<CALSource,CALBuffer> &genBuffer();
    CSoundSample<CALSource,CALBuffer> &genSample(CSoundBuffer<CALSource, CALBuffer> &buf,
                            CSoundFormat &fmt);
    CSoundStream<CALSource, CALBuffer> &genStream(CSoundFormat &fmt);
};

}
}
}

#endif
