#pragma once

#include "../../coffee_mem_macros.h"
#include "../tdef/stltypes.h"
#include "../tdef/integertypes.h"

#include "../../plat/memory/string_ops.h"

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

struct _cbasic_arg_container
{
    FORCEDINLINE _cbasic_arg_container():
        argc(0),
        argv(nullptr),
        string_containment(false)
    {
    }
    FORCEDINLINE _cbasic_arg_container(int32& argc, cstring_w* argv):
        argc(argc),
        argv(argv),
        string_containment(false)
    {
    }

    FORCEDINLINE void cleanup()
    {
        if(string_containment)
        {
            for(int32 i=0;i<argc;i++)
                Mem::CFree(argv[i]);
            Mem::CFree(argv);
        }
    }

    STATICINLINE _cbasic_arg_container Clone(_cbasic_arg_container const& arg)
    {
        return Clone(arg.argc,arg.argv);
    }

    STATICINLINE _cbasic_arg_container Clone(int32 argc, cstring_w* argv)
    {
        _cbasic_arg_container arg;
        arg.argc = argc;
        arg.argv = Mem::CallocPtrs<cstring_w>(argc);
        for(int32 i=0;i<argc;i++)
        {
            szptr arglen = Mem::StrLen(argv[i]);
            arg.argv[i] = Mem::AllocT<sbyte_t>(arglen+1);
            arg.argv[i][arglen] = 0;
            Mem::StrCpy(arg.argv[i],argv[i]);
        }
        arg.string_containment = true;
        return arg;
    }

    cstring_w* argv;
    int32 argc;

private:
    bool string_containment;
};

using AppArg = _cbasic_arg_container;
using Version = _cbasic_version<uint32>;

}
