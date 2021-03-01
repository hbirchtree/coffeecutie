extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <peripherals/stl/types.h>
#include <peripherals/libc/types.h>

namespace av {

const char* init_codec()
{
    AVFormatContext* context;
    avformat_alloc_output_context2(&context, nullptr, "mp4", nullptr);

    return avcodec_license();
}

}
