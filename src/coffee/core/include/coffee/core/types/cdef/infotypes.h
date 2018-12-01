#pragma once

#include <coffee/core/base.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace Coffee {

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
