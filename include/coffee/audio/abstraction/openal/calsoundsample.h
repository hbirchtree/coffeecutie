#ifndef COFFEE_AUDIO_OPENAL_SOUNDSAMPLE_H
#define COFFEE_AUDIO_OPENAL_SOUNDSAMPLE_H

#include "copenal_common.h"

namespace Coffee{
namespace CAudio{
namespace COpenAL{

class CALSoundSample : public CSoundSample<CALSource,CALBuffer>
{
    CSoundDevice<CALSource,CALBuffer>* m_dev;
    CSoundFormat* m_fmt;
    CSoundBuffer<CALSource,CALBuffer>* m_buffer;
    uint64 m_pts;
    uint64 m_samples;

public:
    CALSoundSample(CSoundDevice<CALSource,CALBuffer>& dev,
                   CSoundBuffer<CALSource,CALBuffer>& buf,
                   CSoundFormat& fmt);
    ~CALSoundSample();

    const CSoundDevice<CALSource,CALBuffer>& device();

    CSoundBuffer<CALSource,CALBuffer>& buffer();
    CSoundFormat& format();
    uint64 samples() const;

    uint64 pts() const;
    void setPts(const uint64& pts);

};

}
}
}

#endif
