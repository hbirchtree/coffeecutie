#pragma once

#include "../../coffee_mem_macros.h"
#include "../tdef/stltypes.h"
#include "../tdef/integertypes.h"

namespace Coffee{

/*!
 * \brief Describes details of a hardware device, only informative
 */
struct HWDeviceInfo
{
    FORCEDINLINE HWDeviceInfo(CString model,
                              CString firmware):
        model(model),
        firmware(firmware)
    {}
    FORCEDINLINE HWDeviceInfo(CString manufacturer,
                              CString model,
                              CString firmware):
        manufacturer(manufacturer),
        model(model),
        firmware(firmware)
    {}
    FORCEDINLINE HWDeviceInfo(CString manufacturer,
			      CString model,
			      CString firmware,
			      CString serial):
	manufacturer(manufacturer),
	model(model),
	firmware(firmware),
	serial(serial)
    {}
    const CString manufacturer;
    const CString model;
    const CString firmware;
    const CString serial;
};

struct SWVersionInfo
{
    FORCEDINLINE SWVersionInfo(CString const& name,
                               uint32 major,
                               uint32 minor):
        name(name),
        product(0),
        major(major),
        minor(minor),
        patch(0),
        build("")
    {
    }

    FORCEDINLINE SWVersionInfo(CString const& name,
                               uint32 prod,
                               uint32 major,
                               uint32 minor,
                               uint32 patch,
                               CString const& build):
        name(name),
        product(prod),
        major(major),
        minor(minor),
        patch(patch),
        build(build)
    {
    }

    const CString name;

    const uint32 product;
    const uint32 major;
    const uint32 minor;
    const uint32 patch;
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
        if(this->major>v.major)
            return true;
        if(this->major==v.major)
        {
            if(this->minor>v.minor)
                return true;
            if(this->minor==v.minor)
            {
                if(this->revision>=v.revision)
                    return true;
                else
                    return false;
            }else
                return false;
        }else
            return false;
    }
    inline bool operator<(const _cbasic_version<T>& v) const
    {
        return !((*this)>=v);
    }
    inline bool operator<=(const _cbasic_version<T>& v) const
    {
        if(this->major<v.major)
            return true;
        if(this->major==v.major)
        {
            if(this->minor<v.minor)
                return true;
            if(this->minor==v.minor)
            {
                if(this->revision<=v.revision)
                    return true;
                else
                    return false;
            }else
                return false;
        }else
            return false;
    }
    inline bool operator>(const _cbasic_version<T>& v) const
    {
        return !((*this)<=v);
    }
    inline bool operator==(const _cbasic_version<T>& v) const
    {
        return this->major==v.major
                &&this->minor==v.minor
                &&this->revision==v.revision;
    }
};

}
