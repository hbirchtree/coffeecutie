#include "../../platform_detect.h"

#ifdef COFFEE_LINUX
#ifndef COFFEE_CORE_PLAT_ENV_SYSINFO_H
#define COFFEE_CORE_PLAT_ENV_SYSINFO_H

#include "../sysinfo_def.h"
#include <sys/sysinfo.h>
#include <sys/procfs.h>
#include <unistd.h>

namespace Coffee{

struct LinuxSysInfo : SysInfoDef
{
    STATICINLINE CString CPUInfoString(bool force = false)
    {
        /* We don't want to read it tons of times */
        if(!cached_cpuinfo_string.empty()&&!force)
            return cached_cpuinfo_string;

        C_PERFWARN(__FILE__,__LINE__,"Reading from /proc/cpuinfo!");

        FILE* cpuinfo = fopen("/proc/cpuinfo","r");
        char* arg = 0;
        size_t size = 0;
        CString data;

        while(getdelim(&arg,&size,0,cpuinfo)!=-1)
            data.append(arg);

        fclose(cpuinfo);

        cached_cpuinfo_string = data;

        return data;
    }

    STATICINLINE std::vector<CString> CPUFlags()
    {
        const CString query = "flags";

        CPUInfoString();

        cstring res = CStrFind(cached_cpuinfo_string.c_str(),query.c_str());

        res = CStrFind(res,":")+1;
        cstring end = CStrFind(res,"\n");

        CString result;
        result.insert(0,res,end-res);
        StrUtil::trim(result);

        std::vector<CString> flags;
        cstring ptr = &result[0];

        while(ptr)
        {
            if(ptr!=&result[0])
                ptr++;
            cstring sp = CStrFind(ptr," ");
            if(!sp)
                sp=&result[result.size()-1];
            CString out;
            out.insert(0,ptr,sp-ptr);
            flags.push_back(out);
            ptr = CStrFind(ptr," ");
        }

        return flags;
    }

    STATICINLINE uint32 CpuCount()
    {
        return sysconf(_SC_NPROCESSORS_ONLN);
    }
    STATICINLINE uint32 CoreCount()
    {
        return sysconf(_SC_NPROCESSORS_ONLN);
    }
    STATICINLINE bool MemVirtualAvailable()
    {
        return true;
    }
    STATICINLINE int64 MemTotal()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.totalram*inf.mem_unit;
    }
    STATICINLINE int64 MemAvailable()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.freeram*inf.mem_unit;
    }
    STATICINLINE int64 SwapTotal()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.totalswap*inf.mem_unit;
    }
    STATICINLINE int64 SwapAvailable()
    {
        struct sysinfo inf;
        sysinfo(&inf);
        return inf.freeswap*inf.mem_unit;
    }

    STATICINLINE HWDeviceInfo Processor()
    {
        CString mk,md,fw;

        const CString mk_query = "vendor_id";
        const CString md_query = "model name";
        const CString fw_query = "microcode";

        CPUInfoString();

        cstring mk_str = CStrFind(cached_cpuinfo_string.c_str(),mk_query.c_str())+mk_query.size();
        cstring md_str = CStrFind(cached_cpuinfo_string.c_str(),md_query.c_str())+md_query.size();
        cstring fw_str = CStrFind(cached_cpuinfo_string.c_str(),fw_query.c_str())+fw_query.size();

        mk_str = CStrFind(mk_str,":")+1;
        md_str = CStrFind(md_str,":")+1;
        fw_str = CStrFind(fw_str,":")+1;

        cstring mk_end = CStrFind(mk_str,"\n");
        cstring md_end = CStrFind(md_str,"\n");
        cstring fw_end = CStrFind(fw_str,"\n");

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

        cstring res = CStrFind(cached_cpuinfo_string.c_str(),query.c_str());

        res = CStrFind(res,":")+1;
        cstring end = CStrFind(res,"\n");

        CString result;
        result.insert(0,res,end-res);
        StrUtil::trim(result);

        return CMath::floor(Convert::strtoscalar(result.c_str()))/1000;
    }

    STATICINLINE bool HasFPU()
    {
        const CString query = "fpu";

        CPUInfoString();

        cstring res = CStrFind(cached_cpuinfo_string.c_str(),query.c_str());

        res = CStrFind(res,":")+1;
        cstring end = CStrFind(res,"\n");

        CString result;
        result.insert(0,res,end-res);
        StrUtil::trim(result);

        return CStrCmp(result.c_str(),"yes");
    }

    STATICINLINE int64 ProcessorCacheSize()
    {

    }

    STATICINLINE bool HasHyperThreading()
    {
        std::vector<CString> flags = CPUFlags();

        for(const CString& flag : flags)
        {
            if(CStrCmp(flag.c_str(),"ht")||CStrCmp(flag.c_str(),"htt"))
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

using SysInfo = LinuxSysInfo;

}

#endif
#endif