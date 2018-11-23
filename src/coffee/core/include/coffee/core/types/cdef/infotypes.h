#pragma once

#include <coffee/core/base.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace Coffee {

/*!
 * \brief Describes details of a hardware device, only informative
 */
struct HWDeviceInfo
{
    FORCEDINLINE HWDeviceInfo()
    {
    }
    FORCEDINLINE HWDeviceInfo(CString model, CString firmware) :
        model(model), firmware(firmware)
    {
    }
    FORCEDINLINE HWDeviceInfo(
        CString manufacturer, CString model, CString firmware) :
        manufacturer(manufacturer),
        model(model), firmware(firmware)
    {
    }
    FORCEDINLINE HWDeviceInfo(
        CString manufacturer, CString model, CString firmware, CString serial) :
        manufacturer(manufacturer),
        model(model), firmware(firmware), serial(serial)
    {
    }
    //    FORCEDINLINE HWDeviceInfo(HWDeviceInfo&& dev):
    //        HWDeviceInfo(dev.manufacturer, dev.model,
    //                     dev.firmware, dev.serial)
    //    {
    //    }
    //    FORCEDINLINE HWDeviceInfo(const HWDeviceInfo& dev):
    //        HWDeviceInfo(dev.manufacturer, dev.model,
    //                     dev.firmware, dev.serial)
    //    {
    //    }

    const CString manufacturer;
    const CString model;
    const CString firmware;
    const CString serial;
};

struct SWVersionInfo
{
    FORCEDINLINE SWVersionInfo(CString const& name, u32 major, u32 minor) :
        name(name), product(0), major_(major), minor_(minor), patch(0),
        build("")
    {
    }

    FORCEDINLINE SWVersionInfo(
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
    FORCEDINLINE SWVersionInfo() : SWVersionInfo({}, 0, 0)
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

/*!
 * \brief A template for version structures
 */
template<typename T>
struct _cbasic_version
{
    T major;
    T minor;
    T revision;

    inline bool operator>=(const _cbasic_version<T>& v) const
    {
        if(this->major > v.major)
            return true;
        if(this->major == v.major)
        {
            if(this->minor > v.minor)
                return true;
            if(this->minor == v.minor)
            {
                if(this->revision >= v.revision)
                    return true;
                else
                    return false;
            } else
                return false;
        } else
            return false;
    }
    inline bool operator<(const _cbasic_version<T>& v) const
    {
        return !((*this) >= v);
    }
    inline bool operator<=(const _cbasic_version<T>& v) const
    {
        if(this->major < v.major)
            return true;
        if(this->major == v.major)
        {
            if(this->minor < v.minor)
                return true;
            if(this->minor == v.minor)
            {
                if(this->revision <= v.revision)
                    return true;
                else
                    return false;
            } else
                return false;
        } else
            return false;
    }
    inline bool operator>(const _cbasic_version<T>& v) const
    {
        return !((*this) <= v);
    }
    inline bool operator==(const _cbasic_version<T>& v) const
    {
        return this->major == v.major && this->minor == v.minor &&
               this->revision == v.revision;
    }
};

using Version = _cbasic_version<u32>;

} // namespace Coffee
