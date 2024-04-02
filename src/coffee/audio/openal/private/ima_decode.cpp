#include <oaf/ima_adpcm/decode.h>

namespace oaf::decode::ima_adpcm {

using libc_types::i16;
using libc_types::i32;
using libc_types::i8;
using libc_types::u8;

using uint = libc_types::u32;

//
// Implementation stolen from openal-soft
// It's LGPL-licensed, so handle with care!
//

namespace {

/* IMA ADPCM Stepsize table */
constexpr std::array<int, 89> IMAStep_size{
    {7,     8,     9,     10,    11,    12,    13,    14,    16,    17,
     19,    21,    23,    25,    28,    31,    34,    37,    41,    45,
     50,    55,    60,    66,    73,    80,    88,    97,    107,   118,
     130,   143,   157,   173,   190,   209,   230,   253,   279,   307,
     337,   371,   408,   449,   494,   544,   598,   658,   724,   796,
     876,   963,   1060,  1166,  1282,  1411,  1552,  1707,  1878,  2066,
     2272,  2499,  2749,  3024,  3327,  3660,  4026,  4428,  4871,  5358,
     5894,  6484,  7132,  7845,  8630,  9493,  10442, 11487, 12635, 13899,
     15289, 16818, 18500, 20350, 22358, 24633, 27086, 29794, 32767}};

/* IMA4 ADPCM Codeword decode table */
constexpr std::array<int, 16> IMA4Codeword{
    {1, 3, 5, 7, 9, 11, 13, 15, -1, -3, -5, -7, -9, -11, -13, -15}};

/* IMA4 ADPCM Step index adjust decode table */
constexpr std::array<int, 16> IMA4Index_adjust{
    {-1, -1, -1, -1, 2, 4, 6, 8, -1, -1, -1, -1, 2, 4, 6, 8}};

} // namespace

inline size_t LoadSamples(
    gsl::span<float>           dstSamples,
    gsl::span<const std::byte> src,
    const size_t               srcChan,
    const size_t               srcOffset,
    const size_t               srcStep,
    const size_t               samplesPerBlock) noexcept
{
    static constexpr int MaxStepIndex{
        static_cast<int>(IMAStep_size.size()) - 1};

    size_t written{0};

    assert(srcStep > 0 || srcStep <= 2);
    assert(srcChan < srcStep);
    assert(samplesPerBlock > 1);
    const size_t blockBytes{((samplesPerBlock - 1) / 2 + 4) * srcStep};

    /* Skip to the ADPCM block containing the srcOffset sample. */
    src = src.subspan(srcOffset / samplesPerBlock * blockBytes);
    /* Calculate how many samples need to be skipped in the block. */
    size_t skip{srcOffset % samplesPerBlock};

    /* NOTE: This could probably be optimized better. */
    while(!src.empty())
    {
        auto nibbleData = src.begin();
        src             = src.subspan(blockBytes);

        /* Each IMA4 block starts with a signed 16-bit sample, and a signed
         * 16-bit table index. The table index needs to be clamped.
         */
        int sample{
            int(nibbleData[srcChan * 4]) |
            (int(nibbleData[srcChan * 4 + 1]) << 8)};
        int index{
            int(nibbleData[srcChan * 4 + 2]) |
            (int(nibbleData[srcChan * 4 + 3]) << 8)};
        nibbleData += ptrdiff_t((srcStep + srcChan) * 4);

        sample = (sample ^ 0x8000) - 32768;
        index  = std::clamp((index ^ 0x8000) - 32768, 0, MaxStepIndex);

        if(skip == 0)
        {
            dstSamples[0] = static_cast<float>(sample) / 32768.0f;
            dstSamples    = dstSamples.subspan<1>();
            if(dstSamples.empty())
                return 0;
        } else
            --skip;

        auto decode_sample = [&sample, &index](const uint nibble) {
            sample += IMA4Codeword[nibble] *
                      IMAStep_size[static_cast<uint>(index)] / 8;
            sample = std::clamp(sample, -32768, 32767);

            index += IMA4Index_adjust[nibble];
            index = std::clamp(index, 0, MaxStepIndex);

            return sample;
        };

        /* The rest of the block is arranged as a series of nibbles, contained
         * in 4 *bytes* per channel interleaved. So every 8 nibbles we need to
         * skip 4 bytes per channel to get the next nibbles for this channel.
         *
         * First, decode the samples that we need to skip in the block (will
         * always be less than the block size). They need to be decoded despite
         * being ignored for proper state on the remaining samples.
         */
        size_t       nibbleOffset{0};
        const size_t startOffset{skip + 1};
        for(; skip; --skip)
        {
            const size_t byteShift{(nibbleOffset & 1) * 4};
            const size_t wordOffset{(nibbleOffset >> 1) & ~size_t(3)};
            const size_t byteOffset{
                wordOffset * srcStep + ((nibbleOffset >> 1) & 3u)};
            ++nibbleOffset;

            std::ignore =
                decode_sample(uint(nibbleData[byteOffset] >> byteShift) & 15u);
        }

        /* Second, decode the rest of the block and write to the output, until
         * the end of the block or the end of output.
         */
        const size_t todo{
            std::min(samplesPerBlock - startOffset, dstSamples.size())};
        std::generate_n(dstSamples.begin(), todo, [&] {
            const size_t byteShift{(nibbleOffset & 1) * 4};
            const size_t wordOffset{(nibbleOffset >> 1) & ~size_t(3)};
            const size_t byteOffset{
                wordOffset * srcStep + ((nibbleOffset >> 1) & 3u)};
            ++nibbleOffset;

            const int result{
                decode_sample(uint(nibbleData[byteOffset] >> byteShift) & 15u)};
            return static_cast<float>(result) / 32768.0f;
        });
        dstSamples = dstSamples.subspan(todo);
        written += todo;
    }
    return written;
}

bool decoder::decode(
    gsl::span<const char> const& data, format_t const& format, buffer_t& output)
{
    std::vector<f32> buffer;
    buffer.resize(data.size() * 2);

    auto decoded = gsl::span(buffer.data(), buffer.size());

    size_t written1 = LoadSamples(
        decoded,
        gsl::span(reinterpret_cast<const std::byte*>(data.data()), data.size()),
        0,
        0,
        format.channels,
        65);
    size_t written2;
    if(format.channels == 2)
        written2 = LoadSamples(
            decoded.subspan(data.size()),
            gsl::span(
                reinterpret_cast<const std::byte*>(data.data()), data.size()),
            0,
            0,
            format.channels,
            65);

    if(format.channels == 2)
    {
        std::vector<f32> interleaved;
        interleaved.reserve(written1 * 2);
        for(size_t i = 0; i < written1; ++i)
        {
            f32 v1 = decoded[i], v2 = decoded[data.size() + i];
            interleaved.push_back(v1);
            interleaved.push_back(v2);
        }
        buffer = std::move(interleaved);
    } else
        buffer.resize(written1);
    decoded = gsl::span(buffer.data(), buffer.size());
    // LoadSamples(
    //     decoded,
    //     gsl::span(reinterpret_cast<const std::byte*>(data.data()),
    //     data.size()), 1, 0, format.channels, 65);

    format_t fmt = format;
    fmt.format    = format_t::f32;
    output.upload(decoded, fmt);

    return true;
}

} // namespace oaf::decode::ima_adpcm
