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
    u64 m_pts;
    u64 m_samples;

    CSoundProperty const* m_properties;

public:
    CALSoundSample(CSoundDevice<CALSource,CALBuffer>& dev,
                   CSoundBuffer<CALSource,CALBuffer>& buf,
                   CSoundFormat& fmt);
    ~CALSoundSample();

    const CSoundDevice<CALSource,CALBuffer>& device();

    CSoundBuffer<CALSource,CALBuffer>& buffer();
    CSoundFormat& format();
    u64 samples() const;

    u64 pts() const;
    void setPts(const u64& pts);

    const CSoundProperty *properties();
    void assignProperties(const CSoundProperty *props);
};

}
}
}

#endif
