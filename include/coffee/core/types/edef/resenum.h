#pragma once

#undef None

#include "enumfun.h"
#include "../tdef/integertypes.h"

#undef DELETE

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

enum class ResourceAccess : u32
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
    AssetFile     = 0x8000,  /*!< Packaged in virtual file system, system-specific */
    ConfigFile    = 0x10000, /*!< Stored in configuration directory, system-specific */
    TemporaryFile = 0x20000, /*!< Stored in a temporary directory that is expected to be wiped, system-specific result */
    CachedFile    = 0x40000, /*!< Stored, but could be wiped at any time */

    SystemFile    = 0x80000, /*!< System-dependent, but mostly refers to passing through the path. If you give it abc.text, it will most likely become $PWD/abc.txt, /abc.txt will be /abc.txt */

    Immutable     = 0x100000, /*!< For buffer APIs, do not allow reallocation of the buffer. Does not do anything for resource APIs */

    NoDereference = 0x200000, /*!< When handling a Url, operate on the link, not the file it refers to. Default behavior is to follow links. */

    /* Masks */
    LockingMask = ExclusiveLocking|SharedLocking,
    AccessMask  = ReadWrite|Append|Executable,
    CreateMask  = NewFile|Discard,
    CachingMask = NoCache|GreedyCache|Persistent|Streaming|Virtual|HugeFile,
    StorageMask = SpecifyStorage|AssetFile|ConfigFile
                    |TemporaryFile|CachedFile|SystemFile,
};
C_FLAGS(ResourceAccess,uint32);

enum class HTTPAccess
{
    None = 0x0,

    Secure = 0x1, /* Whether to use SSL */

    GET = 0x2, /* Use GET request */
    POST = 0x4, /* Use POST request */
    PUT = 0x8, /* Use PUT request */
    UPDATE = 0x40,
    DELETE = 0x80,
    PATCH = 0x100,
    HEAD = 0x200,


    NoRedirect = 0x10, /*!< Do not follow redirect locations */
    NoVerify   = 0x20, /*!< When using SSL, do not verify certificates */

    DefaultAccess = GET|Secure,
    DefaultPOST  = POST|Secure,

    SecurityMask = Secure,
    RequestMask = GET|POST|PUT|UPDATE|DELETE|PATCH|HEAD,
};
C_FLAGS(HTTPAccess, u32);
using RSCA = ResourceAccess;

}
