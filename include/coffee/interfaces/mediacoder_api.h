#pragma once

namespace Coffee{
namespace GenericMedia{

struct MediaCoderDef
{
    enum StreamType
    {
        UndefStream,
        VideoStream,
        AudioStream,
        SubtlStream,
    };

    struct Source;
    struct Context;
    struct Stream;

    struct Decoder;
    struct Encoder;

    struct AudioFormat;
    struct SubtlFormat;
    struct VideoFormat;

    struct AudioPacket;
    struct SubtlPacket;
    struct VideoPacket;
};

}
}
