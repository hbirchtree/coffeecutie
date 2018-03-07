#pragma once

#include "caudiomixer_api.h"
#include <coffee/audio/caudio.h>
#include <coffee/core/CFiles>

namespace Coffee{
namespace CAudio{

template<typename Resource,
         typename implements<ByteProvider, Resource>::type* = nullptr>
FORCEDINLINE bool LoadVorbisFromFile(AudioSample& sample, Resource&& file)
{
    bool status = true;

    if(!CAudio::Stb::LoadVorbis(&sample, C_OCAST<Bytes>(file)))
        status = false;

    return status;
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
