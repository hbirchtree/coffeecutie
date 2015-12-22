#include <abstraction/openal/calsoundformat.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

CALSoundFormat::CALSoundFormat():
    i_samrate(0),
    i_channels(0),
    i_bitd(0)
{

}

uint32 CALSoundFormat::sampleSize() const
{
    return i_samrate*i_channels*(i_bitd/8);
}

uint32 CALSoundFormat::samplerate() const
{
    return i_samrate;
}

uint16 CALSoundFormat::channels() const
{
    return i_channels;
}

uint8 CALSoundFormat::bitDepth() const
{
    return i_bitd;
}

void CALSoundFormat::setSamplerate(const uint32 &smrt)
{
    i_samrate = smrt;
}

void CALSoundFormat::setChannels(const uint16 &chans)
{
    i_channels = chans;
}

void CALSoundFormat::setBitDepth(const uint8 &bitd)
{
    i_bitd = bitd;
}

}
}
}
