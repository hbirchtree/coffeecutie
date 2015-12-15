#include <cffmedia.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

namespace Coffee{
namespace CFFMedia{

void coffee_ffmedia_init()
{
    av_log_set_flags(AV_LOG_SKIP_REPEATED);
    avcodec_register_all();
    avdevice_register_all();
    av_register_all();
}

}
}
