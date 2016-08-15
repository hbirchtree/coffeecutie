#include <coffee/core/profiler/profiling-export.h>

#include <coffee/core/CProfiling>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/CXmlParser>

#include <coffee/core/coffee_mem_macros.h>
#include <coffee/core/coffee.h>

#include <coffee/core/plat/plat_file.h>

#include <coffee/core/plat/plat_quirks_toggling.h>

namespace Coffee{
namespace Profiling{

void PrintProfilerData()
{
#ifndef NDEBUG
    cBasicPrint("Profiling information:");

    LinkList<uint64> base_time; /* Stack of time values */
    LinkList<uint64> curr_timeline; /* Progression of time within a context */
    for(Profiler::DataPoint const& p : *Profiler::datapoints)
    {
        if(p.tp==Profiler::DataPoint::Profile)
        {
            uint64 ts = (p.ts-base_time.front())-curr_timeline.front();
            cBasicPrint("Time: {0}, label: {1}",
                        ts,
                        p.name);

            curr_timeline.front() = p.ts-base_time.front();
        }
        else if(p.tp==Profiler::DataPoint::Push)
        {
            base_time.push_front(p.ts);
            curr_timeline.push_front(0);
            cBasicPrint("Enter scope: {0}",p.name,p.ts);
        }
        else if(p.tp==Profiler::DataPoint::Pop)
        {
            base_time.pop_front();
            curr_timeline.pop_front();
            cBasicPrint("Exit scope: {0}",p.name,p.ts);
        }
    }
#endif
}

void ExportProfilerData(cstring out, int32 argc, cstring_w *argv)
{
    XML::Document doc;

    XML::Element* root = doc.NewElement("profile");
    doc.InsertFirstChild(root);

    /* Save environment variables */
    {
        XML::Element* envdata = doc.NewElement("environment");
        root->InsertEndChild(envdata);

        Env::Variables vars = Env::Environment();

        XML::Element* p;
        /* TODO: Make variable whitelist instead of including all of them, for privacy */
        for(Env::VarPair const& v : vars)
        {
            p = doc.NewElement("v");
            p->SetAttribute("name",v.first.c_str());
            p->SetText(v.second.c_str());
            envdata->InsertEndChild(p);
        }
    }
    cVerbose(4,"Writing environment data");

    /* Store data about runtime */
    {
        XML::Element* rundata = doc.NewElement("runtime");
        root->InsertEndChild(rundata);

        /* Launch commandline */
        XML::Element* argument;
        for(int32 i=0;i<argc;i++)
        {
            argument = doc.NewElement("argument");
            rundata->InsertEndChild(argument);

            argument->SetText(argv[i]);
        }

        /* Current working directory,
         * most likely application directory
         *  or home directory */
		CString sys_string = PlatformData::SystemDisplayString();

        CString cwd = Env::CurrentDir();
        rundata->SetAttribute("cwd",cwd.c_str());

        rundata->SetAttribute("version",CoffeeBuildString);
        rundata->SetAttribute("compiler",CoffeeCompilerString);
        rundata->SetAttribute("system", sys_string.c_str());
        rundata->SetAttribute("architecture",CoffeeArchString);
    }
    cVerbose(4,"Writing runtime data");

    /* Store system information */
    {
        XML::Element* sysdata = doc.NewElement("sysinfo");
        root->InsertEndChild(sysdata);

        CString tmp = Convert::inttostring((int32)SysInfo::NetStatus());
        sysdata->SetAttribute("sys.net",tmp.c_str());

        tmp = Convert::uintltostring(SysInfo::MemTotal());
        sysdata->SetAttribute("sys.memory",tmp.c_str());

        {
            HWDeviceInfo pinfo = SysInfo::Processor();

            tmp = pinfo.manufacturer;
            sysdata->SetAttribute("proc.manufacturer",tmp.c_str());

            tmp = pinfo.model;
            sysdata->SetAttribute("proc.model",tmp.c_str());

            tmp = pinfo.firmware;
            sysdata->SetAttribute("proc.firmware",tmp.c_str());
        }

        tmp = Convert::uinttostring(SysInfo::ProcessorCacheSize());
        sysdata->SetAttribute("proc.cache",tmp.c_str());

        tmp = Convert::scalartostring(SysInfo::ProcessorFrequency());
        sysdata->SetAttribute("proc.freq",tmp.c_str());

        {
            tmp = Convert::uinttostring(SysInfo::CpuCount());
            sysdata->SetAttribute("proc.count",tmp.c_str());

            tmp = Convert::uinttostring(SysInfo::CoreCount());
            sysdata->SetAttribute("proc.cores",tmp.c_str());

            tmp = Convert::uinttostring(SysInfo::ThreadCount());
            sysdata->SetAttribute("proc.threads",tmp.c_str());
        }

        tmp = Convert::booltostring(SysInfo::HasHyperThreading());
        sysdata->SetAttribute("proc.hyperthread",tmp.c_str());
    }
    cVerbose(4,"Writing system data");

    /* Store extra data gathered by program, is parsed as JSON if possible */
    {
        XML::Element* extradata = doc.NewElement("extra");
        root->InsertEndChild(extradata);

        XML::Element* e;
        for(Profiler::ExtraPair const& p : *Profiler::extra_data)
        {
            e = doc.NewElement(p.key.c_str());
            extradata->InsertEndChild(e);

            e->SetText(p.value.c_str());
        }
    }
    cVerbose(4,"Writing extra data");

    /* Only runs in debug mode! */
    if(Profiler::Enabled){
#ifndef NDEBUG
        /* Store list of threads we've bumped into or labeled */
        {
            XML::Element* threaddata = doc.NewElement("threads");
            root->InsertEndChild(threaddata);

            XML::Element* e;
            for(Profiler::ThreadItem const& p : *Profiler::threadnames)
            {
                e = doc.NewElement("thread");
                threaddata->InsertEndChild(e);

                CString id = StrUtil::pointerify(p.first);
                e->SetAttribute("id",id.c_str());
                e->SetAttribute("name",p.second.c_str());
            }
        }

        /* Store datapoints */
        {
            XML::Element* curr_r = doc.NewElement("datapoints");
            root->InsertEndChild(curr_r);

            XML::Element* curr = curr_r;

            /* Store information about runtime */
            CString st = Convert::uinttostring(*Profiler::start_time);

            /* Create a nice date string */
            CString date = Time::FormattedCurrentTime("%Y%m%dT%H%M%S");
            date = cStringFormat("{0}+{1}",date,Time::Microsecond());

            curr_r->SetAttribute("start1",st.c_str());
            curr_r->SetAttribute("start2",date.c_str());

            /* Some parsing information */
            LinkList<Timestamp> base;
            LinkList<Timestamp> lt;

            base.push_front(*Profiler::start_time);
            lt.push_front(0);

            Profiler::datapoints->sort();

            /* Finally, smash data points into XML format */
            for(Profiler::DataPoint const& p : *Profiler::datapoints)
            {
                switch(p.tp)
                {
                case Profiler::DataPoint::Profile:
                {
                    uint64 ts = p.ts-base.front()- lt.front();

                    XML::Element* n = doc.NewElement("dpoint");

                    CString tsf = Convert::uinttostring(ts);
                    CString tid = StrUtil::pointerify(p.thread.hash());

                    n->SetAttribute("ts",tsf.c_str());
                    n->SetAttribute("label",p.name.c_str());
                    n->SetAttribute("thread",tid.c_str());

                    curr->InsertEndChild(n);

                    lt.front() = p.ts-base.front();
                    break;
                }
                case Profiler::DataPoint::Push:
                {
                    XML::Element* n = doc.NewElement("context");

                    CString tsf = Convert::uinttostring(p.ts-base.front());
                    CString tid = StrUtil::pointerify(p.thread.hash());

                    n->SetAttribute("ts",tsf.c_str());
                    n->SetAttribute("label",p.name.c_str());
                    n->SetAttribute("thread",tid.c_str());

                    curr->InsertEndChild(n);
                    curr = n;

                    base.push_front(p.ts);
                    lt.push_front(0);
                    break;
                }
                case Profiler::DataPoint::Pop:
                {
                    curr = curr->Parent()->ToElement();
                    base.pop_front();
                    lt.pop_front();
                    break;
                }
                }
                if(curr == nullptr)
                    curr = curr_r;
            }
        }
#endif
    }
    cVerbose(4,"Writing profiler data");

    if(!PlatformData::IsMobile())
    {
        auto file = CResources::CFILEFun::Open(out,ResourceAccess::WriteOnly|ResourceAccess::Discard);

        if (!file)
        {
            cWarning("Failed to save timing profile");
            return;
        }

        /* Because fuck dangling file handles */
#if defined(COFFEE_USE_EXCEPTIONS)
        try{
#endif
            doc.SaveFile(file->handle,false);
#if defined(COFFEE_USE_EXCEPTIONS)
        }catch(std::exception)
        {
        }
#endif

        CResources::CFILEFun::Close(file);
    }else{
#if defined(COFFEE_ANDROID)
        const constexpr cstring mobile_logtemplate = "/data/local/tmp/{0}-profile.xml";
#else
        const constexpr cstring mobile_logtemplate = "/tmp/{0}-profile.xml";
#endif

        cVerbose(5,"Creating tinyxml2 printer");
        tinyxml2::XMLPrinter printer;
        doc.Print(&printer);
        cVerbose(5,"Printed tinyxml2 document");
        CString log_name = cStringFormat(
                    mobile_logtemplate,
                    CoffeeApplicationData::application_name);
        cVerbose(5,"Creating filename");
        CResources::Resource out(log_name.c_str(),true);
        out.data = (c_ptr)printer.CStr();
        out.size = (szptr)printer.CStrSize();
        cVerbose(5,"Retrieving data pointers");
        CResources::FileCommit(out);
        cVerbose(5,"Wrote file");
    }
}

void ExitRoutine(int32 argc, cstring_w *argv, bool silent)
{
    /* Verify if we should export profiler data */
    {
        const constexpr cstring disable_flag = "COFFEE_NO_PROFILER_EXPORT";
        if(!(Env::ExistsVar(disable_flag) && Env::GetVar(disable_flag) == "1"))
        {
            CString log_name = Env::ExecutableName();
            log_name = Env::BaseName(log_name.c_str());
            log_name = CStrReplace(log_name,".exe","");
            log_name = cStringFormat("{0}-profile.xml",log_name);
            if(!silent)
                cDebug("Saving profiler data to: {0}",log_name);

            Profiling::ExportProfilerData(log_name.c_str(),argc,argv);
        }
    }
    /* ... and always destroy the profiler */
    Profiler::DestroyProfiler();
}

}
}
