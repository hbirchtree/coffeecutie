#ifndef COFFEE_AUDIO_OPENAL_SOUNDDEV_H
#define COFFEE_AUDIO_OPENAL_SOUNDDEV_H

#include <abstraction/openal/copenal_common.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

struct _cal_devdata;

class CALSoundDevice : public CSoundDevice<CALSource,CALBuffer>
{
    _cal_devdata* m_data;

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
};

}
}
}

#endif
