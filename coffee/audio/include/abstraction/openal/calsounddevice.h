#ifndef COFFEE_AUDIO_OPENAL_SOUNDDEV_H
#define COFFEE_AUDIO_OPENAL_SOUNDDEV_H

#include <abstraction/openal/copenal_common.h>
#include <abstraction/openal/calsoundmixer.h>
#include <abstraction/openal/calsoundformat.h>
#include <abstraction/openal/calsoundstream.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

class CALSoundDevice : public CSoundDevice
{
    CALContext* m_ctxt = nullptr;
    bool b_input = false;
    CALSoundMixer m_mixer;
    CALSoundFormat m_format;
    CALSoundStream m_inputstream;

public:
    CALSoundDevice(const CSoundDeviceIdentifier &dev, bool input);

    CSoundMixer &mixer();
    CSoundFormat &outputFormat();
    bool isCaptureDevice();
    CSoundStream &captureStreamer();
};

}
}
}

#endif
