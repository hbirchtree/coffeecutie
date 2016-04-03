#pragma once

#include <coffee/core/types/cdef/memtypes.h>
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{
namespace Compression{

struct Compressor_def
{
    static bool Compress(CByteData const&,CByteData*);
    static bool Decompress(CByteData const&,CByteData*);
};

}
}
