#include <coffee/core/datastorage/compression/libz.h>

#include <zlib.h>

namespace Coffee{
namespace Compression{

bool LibZCompressor::Compress(CByteData const& uncompressed, CByteData* target)
{
    return true;
}

bool LibZCompressor::Decompress(CByteData const& compressed, CByteData* target)
{
    return true;
}

}
}
