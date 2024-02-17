#ifndef COFFEE_AUDIO_OPENAL_SOUNDFORMAT_H
#define COFFEE_AUDIO_OPENAL_SOUNDFORMAT_H

#include "copenal_common.h"

namespace Coffee {
namespace CAudio {
namespace COpenAL {

class CALSoundFormat : public CSoundFormat
{
    u32 i_samrate;
    u16 i_channels;
    u8  i_bitd;

  public:
    CALSoundFormat();

    u32  sampleSize() const;
    u32  samplerate() const;
    u16  channels() const;
    u8   bitDepth() const;
    void setSamplerate(const u32& smrt);
    void setChannels(const u16& chans);
    void setBitDepth(const u8& bitd);
};

} // namespace COpenAL
} // namespace CAudio
} // namespace Coffee

#endif
