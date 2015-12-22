#ifndef COFFEE_AUDIO_OPENAL_SOUNDTRACK_H
#define COFFEE_AUDIO_OPENAL_SOUNDTRACK_H

#include <abstraction/openal/copenal_common.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

class CALSoundTrack : public CSoundTrack<CALSource,CALBuffer>
{
    struct _csample_data
    {
        CALBuffer* buf;
        uint64 b_pts;

        bool operator<(const _csample_data& d1)
        {
            return b_pts<d1.b_pts;
        }
        bool operator==(const _csample_data& d1)
        {
            return this==&d1;
        }
    };

    CSoundDevice<CALSource,CALBuffer> *m_dev;
    std::list<_csample_data> m_samples;
    CALSource* m_source;

public:
    CALSoundTrack(CSoundDevice<CALSource, CALBuffer> &dev);
    ~CALSoundTrack();

    virtual const CSoundDevice<CALSource,CALBuffer>& device();

    virtual void queueSample(CSoundSample<CALSource,CALBuffer>& sample);

    virtual void updateTrack(uint64 ts);

    virtual CALSource* object();
};

}
}
}

#endif
