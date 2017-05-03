#pragma once

#undef None

#include "enumfun.h"
#include "../tdef/integertypes.h"

namespace Coffee{

enum class Delay : uint8
{
    Wait,
    NoWait
};

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
    None = 0x0,

    /* Data access modifiers */
    ExclusiveLocking = 0x1, /*<! Absolute mutex */
    SharedLocking    = 0x2, /*<! Mutex with multiple participants */

    /* Caching */
    NoCache     = 0x4, /*<! Avoid caching at all cost, even if it impacts perf */
    GreedyCache = 0x8, /*<! Cache it all in */
    Persistent  = 0x10, /*<! If shared, make changes visible for other participants */
    Streaming   = 0x20, /*<! Streaming, will cache as needed, not greedy */
    Virtual     = 0x40, /*<! Reduce need to load all data */
    HugeFile    = 0x80, /*<! Sometimes used to indicate a wish for large pages */

    /* R/W access */
    ReadOnly   = 0x100,
    WriteOnly  = 0x200,
    ReadWrite  = ReadOnly|WriteOnly,
    Append     = 0x400, /*<! If file exists, append to it */
    Executable = 0x800, /*<! You should know what this does before you try to use it. */

    /* Access specifiers, will not be used with caching options, so we're borrowing some flags */
    Private    = Virtual,
    CopyOnWrite= Private,
    Shared     = SharedLocking,

    /* File operations or policies */
    NewFile = 0x1000, /*<! Don't overwrite files that exist */
    Discard = 0x2000, /*<! Truncate data that exists in file */

    /* Tell function to be particular about where to store this file */
    SpecifyStorage = 0x4000,
    /* Storage types */
    AssetFile     = 0x8000,  /* Packaged in virtual file system, system-specific */
    ConfigFile    = 0x10000, /* Stored in configuration directory, system-specific */
    TemporaryFile = 0x20000, /* Stored in a temporary directory that is expected to be wiped, system-specific result */

    /* Masks */
    LockingMask = ExclusiveLocking|SharedLocking,
    AccessMask  = ReadWrite|Append|Executable,
    CreateMask  = NewFile|Discard,
    CachingMask = NoCache|GreedyCache|Persistent|Streaming|Virtual|HugeFile,
    StorageMask = SpecifyStorage|AssetFile|ConfigFile|TemporaryFile,
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
