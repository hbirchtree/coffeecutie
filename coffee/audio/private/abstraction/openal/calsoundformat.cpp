#include <abstraction/openal/calsoundformat.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

uint32 CALSoundFormat::sampleSize()
{
    return i_samrate*i_channels*(i_bitd/8);
}

uint32 CALSoundFormat::samplerate()
{
    return i_samrate;
}

uint16 CALSoundFormat::channels()
{
    return i_channels;
}

uint8 CALSoundFormat::bitDepth()
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
