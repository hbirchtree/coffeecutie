#ifndef COFFEE_AUDIO_OPENAL_SOUNDBUF_H
#define COFFEE_AUDIO_OPENAL_SOUNDBUF_H

#include "copenal_common.h"
#include "calsoundformat.h"
#include "calsounddevice.h"

namespace Coffee{
namespace CAudio{
namespace COpenAL{

class CALSoundBuffer : public CSoundBuffer<CALSource,CALBuffer>
{
    CALSoundDevice& m_dev;
    szptr i_bufSize;
    CALBuffer* m_buffer;
    CSoundFormat* m_fmt;
public:
    CALSoundBuffer(CALSoundDevice& dev);
    ~CALSoundBuffer();

    const CSoundDevice<CALSource,CALBuffer> &device();

    szptr size() const;

    CSoundFormat &format();
    void setFormat(CSoundFormat &fmt);

    void fillBuffer(c_cptr data, const szptr &size);

    CALBuffer* object();
};

}
}
}

#endif
