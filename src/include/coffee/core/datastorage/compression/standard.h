#pragma once

#include <coffee/core/types/cdef/memtypes.h>
#include <coffee/core/types/cdef/memsafe.h>
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{
/*!
 * \brief Wrappers for compression libraries
 */
namespace Compression{

struct Compressor_def
{
    struct Opts
    {
    };

    static bool Compress(Bytes const&,Bytes*, Opts const&)
    {
        return false;
    }
    static bool Decompress(Bytes const&,Bytes*, Opts const&)
    {
        return false;
    }
};

}
}