#pragma once

#include <coffee/core/CBase>
#include <coffee/core/CTypes>
#include <coffee/CAudio>

namespace Coffee{
namespace FFMedia{

using namespace CAudio;
using namespace CResources;

using CFFMessageCallback = void(*)(void*,int,cstring,va_list);

struct FFDecodeContext;
struct FFVideoPlayer;

struct CFFVideoFormat
{
    CSize size;
};

struct CFFVideoDescriptor
{
    struct
    {
        SampleFormat format;
        uint32 samplerate;
    } audio;
    CFFVideoFormat video;
};

struct CFFAudioPacket
{
    int32 frequency;
    int16 channels;
    int16 bitdepth;

    int32 samples;

    uint64 pts;

    int16* data;
};

template<typename PacketT>
struct CFFPacketTarget
{
    Queue<PacketT> packet_queue;
    Mutex queue_mutex;
};

struct CFFStreamTarget
{
    void* location;
    size_t max_size;
    bool updated;
    int64 pts;
};

struct CFFSubtitleRect
{
    CRect rect;
    cstring text;

    uint64 b_pts;
    uint64 e_pts;
};

struct CFFSubtitleTarget
{
    Vector<CFFSubtitleRect> subtitles;
};

struct CFFVideoTarget
{
    CFFPacketTarget<CFFAudioPacket> a;
    CFFStreamTarget v;
    CFFSubtitleTarget s;
};

struct CFFStreamDescriptor
{
    enum StreamType
    {
        Video,Audio,Subtitle
    };

    struct Stream
    {
        StreamType type;
        CString label;
    };

    Vector<Stream> streams;
};

}
}
