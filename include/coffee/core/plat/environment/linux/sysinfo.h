#include "../../platform_detect.h"

#if defined(COFFEE_LINUX)
#ifndef COFFEE_CORE_PLAT_ENV_SYSINFO_H
#define COFFEE_CORE_PLAT_ENV_SYSINFO_H

#include "../../../coffee_message_macros.h"
#include "../sysinfo_def.h"
#include "../sdlpowerinfo.h"

#include <sys/sysinfo.h>
#include <unistd.h>

namespace Coffee{
namespace Environment{
namespace Linux{
struct LinuxSysInfo : SysInfoDef
{
    STATICINLINE CString CPUInfoString(bool force = false)
    {
        /* We don't want to read it tons of times */
        if(!cached_cpuinfo_string.empty()&&!force)
            return cached_cpuinfo_string;

//        C_PERFWARN(__FILE__,__LINE__,"Reading from /proc/cpuinfo!");

        FILE* cpuinfo = fopen("/proc/cpuinfo","r");
        char* arg = 0;
        size_t size = 0;
        CString data;

        while(getdelim(&arg,&size,0,cpuinfo)!=-1)
            data.append(arg);

        CFree(arg);

        fclose(cpuinfo);

        cached_cpuinfo_string = data;

        return data;
    }

    STATICINLINE Vector<CString> CPUFlags()
    {
        const cstring query = "flags";

        CPUInfoString();

        cstring res = StrFind(cached_cpuinfo_string.c_str(),query);

        res = StrFind(res,":")+1;
        cstring end = StrFind(res,"\n");

        CString result;
        result.insert(0,res,end-res);
        StrUtil::trim(result);

        std::vector<CString> flags;
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

    STATICINLINE uint32 CpuCount()
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

            uint32 c = Convert::strtoint(result.c_str())+1;
            count = CMath::max(count,c);

            src = end;
            res = StrFind(src,query);
        }

        return count;
    }
    STATICINLINE uint32 CoreCount()
    {
        const cstring query = "cpu cores";

        CPUInfoString();

        cstring res = StrFind(cached_cpuinfo_string.c_str(),query);

        res = StrFind(res,":")+1;
        cstring end = StrFind(res,"\n");

        CString result;
        result.insert(0,res,end-res);
        StrUtil::trim(result);

        return Convert::strtoll(result.c_str());
    }
    STATICINLINE bool MemVirtualAvailable()
    {
        return true;
    }
    STATICINLINE uint64 MemTotal()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.totalram*inf.mem_unit;
    }
    STATICINLINE uint64 MemAvailable()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.freeram*inf.mem_unit;
    }
    STATICINLINE uint64 SwapTotal()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.totalswap*inf.mem_unit;
    }
    STATICINLINE uint64 SwapAvailable()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.freeswap*inf.mem_unit;
    }

    STATICINLINE HWDeviceInfo Processor()
    {
        CString mk,md,fw;

        const cstring mk_query = "vendor_id";
        const cstring md_query = "model name";
        const cstring fw_query = "microcode";

        CPUInfoString();

        cstring mk_str = StrFind(cached_cpuinfo_string.c_str(),mk_query);
        cstring md_str = StrFind(cached_cpuinfo_string.c_str(),md_query);
        cstring fw_str = StrFind(cached_cpuinfo_string.c_str(),fw_query);

        mk_str = StrFind(mk_str,":")+1;
        md_str = StrFind(md_str,":")+1;
        fw_str = StrFind(fw_str,":")+1;

        cstring mk_end = StrFind(mk_str,"\n");
        cstring md_end = StrFind(md_str,"\n");
        cstring fw_end = StrFind(fw_str,"\n");

        mk.insert(0,mk_str,mk_end-mk_str);
        md.insert(0,md_str,md_end-md_str);
        fw.insert(0,fw_str,fw_end-fw_str);

        StrUtil::trim(mk);
        StrUtil::trim(md);
        StrUtil::trim(fw);

        return HWDeviceInfo(mk,md,fw);
    }

    STATICINLINE bigscalar ProcessorFrequency()
    {
        const CString query = "cpu MHz";

        CPUInfoString();

        cstring res = StrFind(cached_cpuinfo_string.c_str(),query.c_str());

        res = StrFind(res,":")+1;
        cstring end = StrFind(res,"\n");

        CString result;
        result.insert(0,res,end-res);
        StrUtil::trim(result);

        return CMath::floor(Convert::strtoscalar(result.c_str()))/1000;
    }

    STATICINLINE bool HasFPU()
    {
        const cstring query = "fpu";

        CPUInfoString();

        cstring res = StrFind(cached_cpuinfo_string.c_str(),query);

        res = StrFind(res,":")+1;
        cstring end = StrFind(res,"\n");

        CString result;
        result.insert(0,res,end-res);
        StrUtil::trim(result);

        return StrCmp(result.c_str(),"yes");
    }

    STATICINLINE bool HasFPUExceptions()
    {
        const cstring query = "fpu_exception";

        CPUInfoString();

        cstring res = StrFind(cached_cpuinfo_string.c_str(),query);

        res = StrFind(res,":")+1;
        cstring end = StrFind(res,"\n");

        CString result;
        result.insert(0,res,end-res);
        StrUtil::trim(result);

        return StrCmp(result.c_str(),"yes");
    }

    STATICINLINE uint64 ProcessorCacheSize()
    {
        const cstring query = "cache size";

        CPUInfoString();

        cstring res = StrFind(cached_cpuinfo_string.c_str(),query);

        res = StrFind(res,":")+1;
        cstring end = StrFind(res,"\n");

        CString result;
        result.insert(0,res,end-res);
        StrUtil::trim(result);

        szptr e = result.find(" ");
        result.erase(e,result.size()-e);

        return Convert::strtoll(result.c_str());
    }

    STATICINLINE bool HasHyperThreading()
    {
        std::vector<CString> flags = CPUFlags();

        for(const CString& flag : flags)
        {
            if(StrCmp(flag.c_str(),"ht")||StrCmp(flag.c_str(),"htt"))
                return true;
        }
        return false;
    }

    STATICINLINE NetStatusFlags NetStatus()
    {
        return NetStatLocalOnly;
    }
private:
    thread_local static CString cached_cpuinfo_string;
};

using LinuxPowerInfo = _SDLPowerInfo;

}
}

using PowerInfo = Environment::Linux::LinuxPowerInfo;
using SysInfo = Environment::Linux::LinuxSysInfo;

}

#endif
#endif
