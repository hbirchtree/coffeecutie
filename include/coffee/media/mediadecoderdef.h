#pragma once

namespace Coffee{
namespace GenericMedia{

struct MediaDecoderDef
{
    enum StreamType
    {
        VideoStream,
        AudioStream,
        SubtitleStream,
    };

    struct Source;
    struct Context;
    struct Stream;

    struct AudioFormat;
    struct SubtitleFormat;
    struct VideoFormat;

    struct AudioPacket;
    struct SubtitlePacket;
    struct VideoPacket;
};

}
}