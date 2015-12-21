#ifndef COFFEE_AUDIO_OPENAL_SOUNDFORMAT_H
#define COFFEE_AUDIO_OPENAL_SOUNDFORMAT_H

#include <abstraction/openal/copenal_common.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

class CALSoundFormat : public CSoundFormat
{
    uint32 i_samrate;
    uint16 i_channels;
    uint8 i_bitd;

public:
    CALSoundFormat();

    uint32 sampleSize() const;
    uint32 samplerate() const;
    uint16 channels() const;
    uint8 bitDepth() const;
    void setSamplerate(const uint32 &smrt);
    void setChannels(const uint16 &chans);
    void setBitDepth(const uint8 &bitd);
};

}
}
}

#endif
