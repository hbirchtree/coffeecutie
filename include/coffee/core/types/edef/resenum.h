#pragma once

#include "enumfun.h"
#include "../tdef/integertypes.h"

namespace Coffee{

enum class TypeEnum : uint8
{
    UByte, Byte,
    UShort, Short,
    UInt, Int,
    ULL, LL,
    Scalar, BigScalar,
};

enum class ResourceAccess
{
    /* Data access modifiers */
    ExclusiveLocking = 0x1, /*<! Absolute mutex */
    SharedLocking    = 0x2, /*<! Mutex with multiple participants */

    /* Caching */
    NoCache = 0x8, /*<! Avoid caching at all cost, even if it impacts perf */
    GreedyCache = 0x2000, /*<! Cache it all in */
    Persistent = 0x40, /*<! If shared, make changes visible for other participants */
    Streaming  = 0x80, /*<! Streaming, will cache as needed, not greedy */
    Virtual = 0x200, /*<! Reduce need to load all data */

    /* R/W access */
    ReadOnly   = 0x10,
    WriteOnly  = 0x20,
    ReadWrite  = 0x30,
    Append     = 0x100, /*<! If file exists, append to it */
    Executable = 0x400, /*<! You should know what this does before you try to use it. */

    /* File operations or policies */
    NewFile = 0x800, /*<! Don't overwrite files that exist */
    Discard = 0x4, /*<! Truncate data that exists in file */

    /* Misc. flags */
    HugeFile = 0x1000, /*<! Sometimes used to indicate a wish for large pages */
};
C_FLAGS(ResourceAccess,uint32);

enum class BitFormat : uint8
{
    Byte, ByteR,
    UByte, UByteR,
    UByte_332, UByte_233R,

    Short, ShortR,
    UShort, UShortR,
    UShort_565, UShort_565R,
    UShort_5551, UShort_1555R,

    Int, IntR,
    UInt, UIntR,
    UInt_5999R,
    UInt_1010102, UInt_2101010R,

    Scalar_16,
    Scalar_32,
    Scalar_64,
    Scalar_11_11_10,

    Scalar_32_Int_24_8,
};

}