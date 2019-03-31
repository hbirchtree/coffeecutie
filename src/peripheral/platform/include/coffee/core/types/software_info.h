#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace {

using namespace ::libc_types;
using namespace ::stl_types;

} // namespace

namespace platform {
namespace info {

struct SoftwareVersion
{
    FORCEDINLINE SoftwareVersion(CString const& name, u32 major, u32 minor) :
        name(name), product(0), major_(major), minor_(minor), patch(0),
        build("")
    {
    }

    FORCEDINLINE SoftwareVersion(
        CString const& name,
        u32            prod,
        u32            major,
        u32            minor,
        u32            patch,
        CString const& build) :
        name(name),
        product(prod), major_(major), minor_(minor), patch(patch), build(build)
    {
    }
    FORCEDINLINE SoftwareVersion() : SoftwareVersion({}, 0, 0)
    {
    }

    const CString name;

    const u32 product;
    union
    {
        const u32 major;
        const u32 major_;
    };
    union
    {
        const u32 minor;
        const u32 minor_;
    };
    const u32     patch;
    const CString build;
};

} // namespace info
} // namespace platform

namespace Coffee {
using SWVersionInfo = platform::info::SoftwareVersion;
}
