#include "decoder.h"

#include <peripherals/stl/range.h>
#include <peripherals/stl/types.h>

namespace compressor {

template<class ImageType>
std::optional<ImageType> map_channels(
    const ImageType& image, const std::string& channels)
{
    enum channel_map_t : int
    {
        none          = -1,
        red_channel   = 0,
        blue_channel  = 1,
        green_channel = 2,
        alpha_channel = 3,
    };

    if(channels.empty() || channels.size() > 4)
        return std::nullopt;

    std::vector<channel_map_t> channel_map;

    for(auto const& channel : channels)
    {
        if(channel == 'r')
            channel_map.push_back(red_channel);
        if(channel == 'g')
            channel_map.push_back(green_channel);
        if(channel == 'b')
            channel_map.push_back(blue_channel);
        if(channel == 'a')
            channel_map.push_back(alpha_channel);
    }

    ImageType output;
    output.data_owner = semantic::Bytes::withSize(
        image.size.area() * 4 * sizeof(decltype(*output.data)));
    output.bpp  = channels.size();
    output.size = image.size;
    output.data
        = reinterpret_cast<decltype(output.data)>(output.data_owner.data);

    for(auto i : stl_types::range(output.size.area()))
    {
        const auto* src       = &image.data[i * image.bpp];
        auto*       dst       = &output.data[i * 4];
        uint32_t    component = 0;
        for(auto c : channel_map)
            dst[component++] = src[c];
    }

    return output;
}

template std::optional<rgba_image_t> map_channels(
    rgba_image_t const& image, std::string const& channels);

template std::optional<rgbaf_image_t> map_channels(
    rgbaf_image_t const& image, std::string const& channels);

} // namespace compressor
