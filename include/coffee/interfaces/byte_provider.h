#pragma once

#include <coffee/core/types/cdef/memtypes.h>
#include <coffee/core/base/files/url.h>

namespace Coffee{
/*!
 * \brief
 * This class is used to indicate that it can be static_cast'ed to Bytes
 */
struct ByteProvider
{
    operator Bytes() const
    {
        return Bytes();
    }

    operator Path() const
    {
        return Path();
    }
};
}
