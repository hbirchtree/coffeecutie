#pragma once

#include "../../coffee_mem_macros.h"
#include "../tdef/stltypes.h"

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
    const CString manufacturer;
    const CString model;
    const CString firmware;
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

    inline bool operator>=(const _cbasic_version<T>& v)
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
    inline bool operator<(const _cbasic_version<T>& v)
    {
        return !((*this)>=v);
    }
    inline bool operator<=(const _cbasic_version<T>& v)
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
    inline bool operator>(const _cbasic_version<T>& v)
    {
        return !((*this)<=v);
    }
    inline bool operator==(const _cbasic_version<T>& v)
    {
        return this->major==v.major
                &&this->minor==v.minor
                &&this->revision==v.revision;
    }
};

}