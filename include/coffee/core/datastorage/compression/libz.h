#pragma once

#include "standard.h"

namespace Coffee{
namespace Compression{

struct LibZCompressor : Compressor_def
{
    static bool Compress(CByteData const& uncompressed, CByteData* target);

    static bool Decompress(CByteData const& compressed, CByteData* target);
};

}
}
