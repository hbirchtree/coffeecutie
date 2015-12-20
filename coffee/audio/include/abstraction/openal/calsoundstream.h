#ifndef COFFEE_AUDIO_OPENAL_SOUNDSTREAM_H
#define COFFEE_AUDIO_OPENAL_SOUNDSTREAM_H

#include <abstraction/openal/copenal_common.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

class CALSoundStream : public CSoundStream
{
public:
    const CSoundDevice &device();
    CSoundBuffer &buffer();
    void setSoundBuffer(CSoundBuffer &dest);
    bool isInputStream();
};

}
}
}

#endif
