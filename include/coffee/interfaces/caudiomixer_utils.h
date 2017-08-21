#pragma once

#include "caudiomixer_api.h"
#include <coffee/audio/caudio.h>
#include <coffee/core/CFiles>

namespace Coffee{
namespace CAudio{

FORCEDINLINE bool LoadVorbisFromFile(AudioSample& sample, Resource&& file)
{
    bool status = true;

    if(!CResources::FileMap(file))
        return false;

    if(!CAudio::Stb::LoadVorbis(&sample, &file))
        status = false;

    CResources::FileUnmap(file);

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
