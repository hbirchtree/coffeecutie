#include <oaf/ogg/ogg_decode.h>

#include <coffee/core/debug/formatting.h>

namespace oaf::decode::ogg {

using namespace Coffee::Logging;

decoder::~decoder()
{
    if(file_info)
        ov_clear(&file);
}

bool decoder::decode(
    gsl::span<const char> const&                       data,
    std::optional<std::chrono::system_clock::duration> start_time,
    std::optional<std::chrono::system_clock::duration> duration,
    buffer_t&                                          output)
{
    if(data.empty())
        return {};

    decode_data.input = data;
    decode_data.ptr   = 0;

    auto res = ov_open_callbacks(
        &decode_data,
        &file,
        nullptr,
        data.size(),
        ov_callbacks{
            .read_func =
                [](void* ptr, size_t size, size_t nmemb, void* src) -> size_t {
                auto* data  = reinterpret_cast<decode_data_t*>(src);
                auto  size_ = size * nmemb;
                auto  rem   = data->input.size() - data->ptr;
                size_       = std::min(size_, rem);
                if(size_ == 0)
                    return 0;
                memcpy(ptr, data->input.data() + data->ptr, size_);
                data->ptr += size_;
                return size_;
            },
            .seek_func =
                [](void* src, ogg_int64_t offset, int whence) {
                    auto* data = reinterpret_cast<decode_data_t*>(src);
                    switch(whence)
                    {
                    case SEEK_CUR:
                        if(offset > 0 &&
                           offset > (data->ptr + data->input.size()))
                            return -1;
                        if(offset < 0 && (-offset) > data->ptr)
                            return -1;
                        data->ptr += offset;
                        break;
                    case SEEK_SET:
                        if(offset > data->input.size())
                            return -1;
                        data->ptr = offset;
                        break;
                    case SEEK_END:
                        if(offset > 0)
                            return -1;
                        if(offset < 0 && (-offset) > data->input.size())
                            return -1;
                        data->ptr = data->input.size() + offset;
                        break;
                    }
                    return 0;
                },
            .close_func = nullptr,
            .tell_func =
                [](void* src) {
                    auto* data = reinterpret_cast<decode_data_t*>(src);
                    return static_cast<long>(data->ptr);
                },
        });
    if(res < 0)
        return false;

    file_info = ov_info(&file, -1);
    if(!file_info)
        return false;

    format_t fmt{};
    fmt.frequency = static_cast<libc_types::u32>(file_info->rate);
    fmt.channels  = static_cast<libc_types::u16>(file_info->channels);
    fmt.bits      = 16;

    /* Counted in frames, rounding to milliseconds dropped the tail of
     * every piece and left a click where the next one was queued */
    auto const total = ov_pcm_total(&file, -1);
    cDebug(
        "OGG stream: rate={}, channels={}, total={}",
        fmt.frequency,
        fmt.channels,
        total);
    auto const to_frames = [&](std::chrono::system_clock::duration t) {
        return static_cast<ogg_int64_t>(
            std::chrono::duration_cast<std::chrono::microseconds>(t).count() *
            fmt.frequency / 1000000);
    };
    ogg_int64_t const first = start_time ? to_frames(*start_time) : 0;
    if(first >= total)
        return false;
    ogg_int64_t const last =
        duration ? std::min(total, first + to_frames(*duration)) : total;
    if(first > 0)
        ov_pcm_seek(&file, first);

    std::size_t const frame_bytes = 2u * fmt.channels;
    std::vector<char> buf(4096);
    std::vector<char> out;
    out.reserve(static_cast<std::size_t>(last - first) * frame_bytes);

    int         current_section;
    ogg_int64_t remaining = last - first;
    while(remaining > 0)
    {
        auto res =
            ov_read(&file, buf.data(), buf.size(), 0, 2, 1, &current_section);
        if(res == OV_HOLE)
            continue;
        if(res <= 0)
            break;
        auto const frames = std::min<ogg_int64_t>(
            static_cast<ogg_int64_t>(res) / frame_bytes, remaining);
        out.insert(out.end(), buf.begin(), buf.begin() + frames * frame_bytes);
        remaining -= frames;
    }

    output.upload(gsl::span<const char>(out.data(), out.size()), fmt);
    return true;
}

} // namespace oaf::decode::ogg
