#ifndef COFFEE_AUDIO_OPENAL_SOUNDTRACK_H
#define COFFEE_AUDIO_OPENAL_SOUNDTRACK_H

#include "copenal_common.h"

namespace Coffee {
namespace CAudio {
namespace COpenAL {

class CALSoundTrack : public CSoundTrack<CALSource, CALBuffer>
{
    struct _csample_data
    {
        CALBuffer*            buf;
        CSoundProperty const* prop;
        u64                   b_pts;

        bool operator<(const _csample_data& d1) const
        {
            return b_pts < d1.b_pts;
        }

        bool operator==(const _csample_data& d1) const
        {
            return this == &d1;
        }
    };

    CSoundDevice<CALSource, CALBuffer>* m_dev;
    List<_csample_data>                 m_samples;
    CALSource*                          m_source;

    CSoundProperty const*    m_defaultProp;
    CListenerProperty const* m_listenProp;

  public:
    CALSoundTrack(CSoundDevice<CALSource, CALBuffer>& dev);
    ~CALSoundTrack();

    virtual const CSoundDevice<CALSource, CALBuffer>& device();

    virtual void queueSample(CSoundSample<CALSource, CALBuffer>& sample);

    virtual void updateTrack(u64 ts);

    virtual CALSource* object();

    void assignProperties(const CSoundProperty* props);
    void assignProperties(const CListenerProperty* props);
};

} // namespace COpenAL
} // namespace CAudio
} // namespace Coffee

#endif
