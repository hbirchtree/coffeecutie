#include <coffee/core/plat/environment/linux/sysinfo.h>
#include <coffee/core/plat/file/linux/file.h>
#include <coffee/core/plat/environment/linux/environment.h>
#include <coffee/core/string_casting.h>

namespace Coffee{
namespace Environment{
namespace Linux{

/* More paths to inspect:
 *
 * Desktop details:
 * - /sys/class/dmi/id/board_name
 * - /sys/class/dmi/id/board_vendor
 * - /sys/class/dmi/id/chassis_type
 *
 * Networking:
 * - /sys/class/net/.../operstate
 *
 */

static CString get_lsb_release()
{
    CString version = CResources::Linux::LinuxFileFun::sys_read("/etc/lsb-release");
    cstring desc = StrFind(version.c_str(), "DISTRIB_DESCRIPTION");
    if(desc && (desc = Search::ChrFind(desc, '=') + 1)
            && (desc = Search::ChrFind(desc, '"') + 1))
    {
        cstring end = Search::ChrFind(desc, '"');
        if(end || (end = Search::ChrFind(desc, 0)))
        {
            CString desc_std(desc, end-desc);
            return desc_std;
        }
    }
    return {};
}

static CString get_kern_ver()
{
    utsname d;
    if(uname(&d)!=0)
        return "?";
    else
        return d.release;
}

CString get_kern_name()
{
    utsname d;
    if(uname(&d)!=0)
        return "?";
    else
        return d.sysname;
}

CString get_kern_arch()
{
    utsname d;
    if(uname(&d)!=0)
        return "?";
    else
        return d.machine;
}

CString LinuxSysInfo::GetSystemVersion()
{
    CString tmp = get_lsb_release();
    if(tmp.size() <= 0)
        tmp = get_kern_ver();
    return tmp;
}

CString LinuxSysInfo::CPUInfoString(bool force)
{
    /* We don't want to read it tons of times */
    if(!cached_cpuinfo_string.empty()&&!force)
        return cached_cpuinfo_string;

    //        C_PERFWARN(__FILE__,__LINE__,"Reading from /proc/cpuinfo!");

    CString data = CResources::Linux::LinuxFileFun::sys_read("/proc/cpuinfo");

    cached_cpuinfo_string = data;

    return data;
}

Vector<CString> LinuxSysInfo::CPUFlags()
{
    const cstring query_linaro = "Features";
    const cstring query = "flags";

    CPUInfoString();

    cstring res = StrFind(cached_cpuinfo_string.c_str(),query);

    if(!res)
        res = StrFind(cached_cpuinfo_string.c_str(),query_linaro);

    if(!res)
        return {};

    res = StrFind(res,":")+1;
    cstring end = StrFind(res,"\n");

    CString result;
    result.insert(0,res,end-res);
    StrUtil::trim(result);

    Vector<CString> flags;
    cstring ptr = &result[0];

    while(ptr)
    {
        if(ptr!=&result[0])
            ptr++;
        cstring sp = StrFind(ptr," ");
        if(!sp)
            sp=&result[result.size()-1];
        CString out;
        out.insert(0,ptr,sp-ptr);
        flags.push_back(out);
        ptr = StrFind(ptr," ");
    }

    return flags;
}

uint32 LinuxSysInfo::CpuCount()
{
    const cstring query = "physical id";

    CPUInfoString();

    cstring src = cached_cpuinfo_string.c_str();

    uint32 count = 0;
    cstring res = StrFind(src,query);
    while(res)
    {
        res = StrFind(res,":")+1;
        cstring end = StrFind(res,"\n");

        CString result;
        result.insert(0,res,end-res);
        StrUtil::trim(result);

        uint32 c = cast_string<uint32>(result)+1;
        count = CMath::max(count,c);

        src = end;
        res = StrFind(src,query);
    }

    return count ? count : 1;
}

uint32 LinuxSysInfo::CoreCount()
{
    const cstring query = "cpu cores";

    CPUInfoString();

    cstring res = StrFind(cached_cpuinfo_string.c_str(),query);

    if(!res)
        return ThreadCount();

    res = StrFind(res,":")+1;
    cstring end = StrFind(res,"\n");

    CString result;
    result.insert(0,res,end-res);
    StrUtil::trim(result);

    uint32 cores = cast_string<uint32>(result);

    return cores ? cores : 1;
}

HWDeviceInfo LinuxSysInfo::Processor()
{
    CString mk,md,fw;

    const cstring mk_query = "vendor_id";
    const cstring md_query = "model name";
    const cstring fw_query = "microcode";

    const cstring mk_query_linaro = "Hardware";
    const cstring fw_query_linaro = "Revision";

    CPUInfoString();

    cstring mk_str = StrFind(cached_cpuinfo_string.c_str(),mk_query);
    if(!mk_str)
        mk_str = StrFind(cached_cpuinfo_string.c_str(),mk_query_linaro);
    cstring md_str = StrFind(cached_cpuinfo_string.c_str(),md_query);
    cstring fw_str = StrFind(cached_cpuinfo_string.c_str(),fw_query);
    if(!fw_str)
        fw_str = StrFind(cached_cpuinfo_string.c_str(),fw_query_linaro);

    if(mk_str)
    {
        mk_str = StrFind(mk_str,":")+1;
        cstring mk_end = StrFind(mk_str,"\n");
        mk.insert(0,mk_str,mk_end-mk_str);
        StrUtil::trim(mk);
    }

    if(md_str)
    {
        md_str = StrFind(md_str,":")+1;
        cstring md_end = StrFind(md_str,"\n");
        md.insert(0,md_str,md_end-md_str);
        StrUtil::trim(md);
    }

    if(fw_str)
    {
        fw_str = StrFind(fw_str,":")+1;
        cstring fw_end = StrFind(fw_str,"\n");
        fw.insert(0,fw_str,fw_end-fw_str);
        StrUtil::trim(fw);
    }

    return HWDeviceInfo(mk,md,fw);
}

bigscalar LinuxSysInfo::ProcessorFrequency()
{
    const CString query = "cpu MHz";

    CPUInfoString();

    cstring res = StrFind(cached_cpuinfo_string.c_str(),query.c_str());

    if(!res)
        return 0.0;

    res = StrFind(res,":")+1;
    cstring end = StrFind(res,"\n");

    CString result;
    result.insert(0,res,end-res);
    StrUtil::trim(result);

    return CMath::floor(cast_string<bigscalar>(result))/1000;
}

bool LinuxSysInfo::HasFPU()
{
#if defined(COFFEE_MAEMO)
    const cstring query = "vfpv3";
#else
    const cstring query = "fpu";
#endif

    CPUInfoString();

    cstring res = StrFind(cached_cpuinfo_string.c_str(),query);

    if(!res)
        return false;

    res = StrFind(res,":")+1;
    cstring end = StrFind(res,"\n");

    CString result;
    result.insert(0,res,end-res);
    StrUtil::trim(result);

    return StrCmp(result.c_str(),"yes");
}

bool LinuxSysInfo::HasFPUExceptions()
{
    const cstring query = "fpu_exception";

    CPUInfoString();

    cstring res = StrFind(cached_cpuinfo_string.c_str(),query);

    if(!res)
        return false;

    res = StrFind(res,":")+1;
    cstring end = StrFind(res,"\n");

    CString result;
    result.insert(0,res,end-res);
    StrUtil::trim(result);

    return StrCmp(result.c_str(),"yes");
}

uint64 LinuxSysInfo::ProcessorCacheSize()
{
    const cstring query = "cache size";

    CPUInfoString();

    cstring res = StrFind(cached_cpuinfo_string.c_str(),query);

    if(!res)
        return 0;

    res = StrFind(res,":")+1;
    cstring end = StrFind(res,"\n");

    CString result;
    result.insert(0,res,end-res);
    StrUtil::trim(result);

    szptr e = result.find(" ");
    result.erase(e,result.size()-e);

    return cast_string<uint64>(result);
}

bool LinuxSysInfo::HasHyperThreading()
{
    Vector<CString> flags = CPUFlags();

    for(const CString& flag : flags)
    {
        if(StrCmp(flag.c_str(),"ht")||StrCmp(flag.c_str(),"htt"))
            return true;
    }
    return false;
}

HWDeviceInfo LinuxSysInfo::DeviceName()
{
    static const cstring prod_ver = "/sys/class/dmi/id/product_version";
    static const cstring prod_name = "/sys/class/dmi/id/product_name";

    static const cstring str_gen = "Generic";
    static const cstring str_lin = "Linux";

    /* Assumes the following format for lsb-release:
     *
     * DISTRIB_ID=Ubuntu
     * DISTRIB_RELEASE=16.04
     * DISTRIB_CODENAME=xenial
     * DISTRIB_DESCRIPTION="Ubuntu 16.04.1 LTS"
     *
     */

    cstring manf = str_gen;
    cstring prod = str_lin;

    CString manufac = CResources::Linux::LinuxFileFun::sys_read("/sys/class/dmi/id/sys_vendor");
    cstring prod_src = prod_name;
    if(manufac == "LENOVO")
        prod_src = prod_ver;
    CString product = CResources::Linux::LinuxFileFun::sys_read(prod_ver);

    if(manufac.size() > 0)
        manf = manufac.c_str();
    if(product.size() > 0)
        prod = product.c_str();

    return HWDeviceInfo(manf, prod, get_kern_name() + (" " + get_kern_ver()));
}

using namespace CResources;

PowerInfoDef::Temp LinuxPowerInfo::CpuTemperature()
{
    static const constexpr cstring thermal_class = "/sys/class/thermal";
    Temp out = {};

    DirFun::DirList lst;
    CString tmp,tmp2;
    if(DirFun::Ls(thermal_class,lst))
    {
        for(auto e : lst)
            if(e.name != "." && e.name != "..")
            {
                tmp = Env_::ConcatPath(thermal_class,e.name.c_str());
                tmp2 = Env_::ConcatPath(tmp.c_str(),"temp");

                if(FileFun::Exists(tmp2.c_str()))
                {
                    CString temp = FileFun::sys_read(tmp2.c_str());
                    out.current = cast_string<scalar>(temp) / 1000;
                    break;
                }
            }
    }

    return out;
}

}
}
}
