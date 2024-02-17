#include <coffee/audio/abstraction/openal/calsoundformat.h>

namespace Coffee {
namespace CAudio {
namespace COpenAL {

CALSoundFormat::CALSoundFormat()
    : i_samrate(0)
    , i_channels(0)
    , i_bitd(0)
{
}

u32 CALSoundFormat::sampleSize() const
{
    return i_samrate * i_channels * (i_bitd / 8);
}

u32 CALSoundFormat::samplerate() const
{
    return i_samrate;
}

u16 CALSoundFormat::channels() const
{
    return i_channels;
}

u8 CALSoundFormat::bitDepth() const
{
    return i_bitd;
}

void CALSoundFormat::setSamplerate(const u32& smrt)
{
    i_samrate = smrt;
}

void CALSoundFormat::setChannels(const u16& chans)
{
    i_channels = chans;
}

void CALSoundFormat::setBitDepth(const u8& bitd)
{
    i_bitd = bitd;
}

} // namespace COpenAL
} // namespace CAudio
} // namespace Coffee
