#pragma once

#include "caudiomixer_api.h"
#include <coffee/audio/caudio.h>

namespace Coffee{
namespace CAudio{

template<typename T>
FORCEDINLINE void LoadVorbisFromFile(T* dev)
{
}

template<typename T,
         typename std::enable_if<
             std::is_base_of<CSoundAbstraction::CSoundFormat, T>::value>::type* = nullptr>
FORCEDINLINE void AbstractFormatFromAudio(AudioFormat const& from, T& to)
{
    to.setSamplerate(from.samplerate);
    to.setChannels(from.channels);
    to.setBitDepth(from.bitdepth);
}

}
}
