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

    using namespace std::chrono_literals;

    auto total          = ov_pcm_total(&file, -1);
    auto audio_duration = std::chrono::milliseconds(
        (total * 1000) / fmt.frequency);
    cDebug(
        "OGG stream: rate={}, channels={}, total={}, duration={}ms",
        fmt.frequency,
        fmt.channels,
        total,
        audio_duration.count());

    auto time_offset   = start_time.value_or(0ms);
    auto time_duration = duration.value_or(audio_duration);

    // If we're trying to play a region outside the file, stop
    if(time_offset > audio_duration)
        return false;
    // If we're kind of within the file, but kind of outside, cap it
    if((time_offset + time_duration) > audio_duration)
        time_duration = audio_duration - time_offset;

    auto time_end = time_offset + time_duration;

    auto d_duration =
        std::chrono::duration_cast<stl_types::Chrono::seconds_f64>(time_end);

    auto sample_offset =
        (std::chrono::duration_cast<std::chrono::milliseconds>(time_offset)
             .count() *
         fmt.frequency * fmt.channels) /
        1000;

    ov_pcm_seek(&file, sample_offset);

    std::vector<char> buf(4096);
    std::vector<char> out;

    bool        eof{false};
    int         current_section;
    ogg_int64_t num_samples_read{0};
    do
    {
        auto res =
            ov_read(&file, buf.data(), buf.size(), 0, 2, 1, &current_section);
        if(res == 0)
            break;
        else if(res < 0)
        {
            // ... oops
        } else
        {
            out.insert(out.end(), buf.begin(), buf.begin() + res);
            // This is inaccurate right now
            if(ov_time_tell(&file) > d_duration.count())
                break;
        }
    } while(true);

    output.upload(gsl::span<const char>(out.data(), out.size()), fmt);
    return true;
}

} // namespace oaf::decode::ogg
