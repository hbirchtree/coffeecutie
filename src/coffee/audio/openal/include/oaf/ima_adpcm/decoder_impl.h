#pragma once

#include <gsl/span>

namespace oaf::decode::ima_adpcm {

size_t LoadSamples(
    gsl::span<float>           dstSamples,
    gsl::span<const std::byte> src,
    const size_t               srcChan,
    const size_t               srcOffset,
    const size_t               srcStep,
    const size_t               samplesPerBlock);

}
