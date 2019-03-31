#pragma once

#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/time_types.h>
#include <url/url.h>

namespace semantic {
/*!
 * \brief
 * This class is used to indicate that it can be static_cast'ed to Bytes
 *
 * It serves as a type-check for all Resource types.
 * The current list of Resource types is as such:
 *
 * - Coffee::CResources::Resource, for example:
 *       - Filesystem resources
 *       - Android assets
 *       - Win32 resources,
 *       - macOS/iOS resources
 *       - Emscripten preload files
 * - Coffee::Net::Resource, HTTP/HTTPS resources (depends on ASIO)
 * - Coffee::VirtFS::Resource, resources embedded in virtual filesystems
 *       contained by Coffee::VirtFS::VFS
 *
 * This API is used by other interfaces for generic access to data,
 *  such as Coffee::RHI::LoadTexture()
 *
 *
 */
struct ByteProvider
{
    operator Bytes() const
    {
        return Bytes();
    }

    operator platform::url::Path() const
    {
        return platform::url::Path();
    }
};

struct DataTimestamp
{
    operator stl_types::Chrono::steady_clock::time_point() const
    {
        return {};
    }
};

} // namespace semantic
