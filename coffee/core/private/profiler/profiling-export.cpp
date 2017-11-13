#include <coffee/core/profiler/profiling-export.h>

#include <coffee/core/base/files/url.h>

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
#ifdef COFFEE_LOWFAT
    return;
#endif

#ifndef NDEBUG
    cBasicPrint("Profiling information:");

    LinkList<uint64> base_time; /* Stack of time values */
    LinkList<uint64> curr_timeline; /* Progression of time within a context */
    for(Profiler::DataPoint const& p : *Profiler::DataPoints())
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

STATICINLINE XML::Element* AddChildWithText(
        XML::Document& doc, XML::Element* e,
        cstring tagname, CString text)
{
    auto newElement = doc.NewElement(tagname);
    e->InsertEndChild(newElement);

    if(text.size() > 0)
        newElement->SetText(text.c_str());

    return newElement;
}

STATICINLINE void ElementSetAttr(
        XML::Element* e, cstring attrname, CString text)
{
    e->SetAttribute(attrname, text.c_str());
}

STATICINLINE void DescribeNetwork(XML::Document& doc, XML::Element* sysdata)
{
    auto net = AddChildWithText(doc, sysdata, "networking", {});

    AddChildWithText(doc, net, "state", cast_pod(SysInfo::NetStatus()));
}

STATICINLINE void DescribeMemory(XML::Document& doc, XML::Element* sysdata)
{
    auto mem = AddChildWithText(doc, sysdata, "memory", {});

    AddChildWithText(doc, mem, "bank", cast_pod(SysInfo::MemTotal()));
}

STATICINLINE void DescribeProcessor(XML::Document& doc, XML::Element* sysdata)
{
    XML::Element* procdata = doc.NewElement("processors");
    sysdata->InsertEndChild(procdata);

    HWDeviceInfo pinfo = SysInfo::Processor();

    XML::Element* proc_detail = doc.NewElement("processor");
    procdata->InsertEndChild(proc_detail);

    AddChildWithText(doc, proc_detail, "manufacturer", pinfo.manufacturer);
    AddChildWithText(doc, proc_detail, "model", pinfo.model);
    AddChildWithText(doc, proc_detail, "firmware", pinfo.firmware);

    auto cache = AddChildWithText(doc, proc_detail, "cache", {});

    AddChildWithText(doc, cache, "l1",
                     cast_pod(SysInfo::ProcessorCacheSize()));

    AddChildWithText(doc, proc_detail, "frequencies",
                     cast_pod(SysInfo::ProcessorFrequency()));

    auto hier = AddChildWithText(doc, proc_detail, "hierarchy", {});

    ElementSetAttr(hier, "cores", cast_pod(SysInfo::CoreCount()));
    ElementSetAttr(hier, "threads", cast_pod(SysInfo::ThreadCount()));

    if(SysInfo::HasHyperThreading())
        AddChildWithText(doc, proc_detail, "hyperthreading", {});
}

void ExportProfilerData(CString& target)
{
#ifdef COFFEE_LOWFAT
    return;
#endif

    auto app_args = GetInitArgs().arguments();

    XML::Document doc;

    XML::Element* root = doc.NewElement("profile");
    doc.InsertFirstChild(root);

    /* Save environment variables */
#if defined(COFFEE_INTERNAL_BUILD)
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
    cVerbose(8,"Writing environment data");
#endif

    /* Store data about runtime */
    {
        auto rundata = AddChildWithText(doc, root, "runtime", {});

        auto args = AddChildWithText(doc, rundata, "arguments", {});

        /* Launch commandline */
        XML::Element* argument;
        for(auto arg : app_args)
        {
            argument = doc.NewElement("argument");
            args->InsertEndChild(argument);

            argument->SetText(arg);
        }

        auto AddElement = [&](cstring tagname, CString text)
        {
            AddChildWithText(doc,  rundata, tagname, text);
        };

        /* Current working directory,
         * most likely application directory
         *  or home directory */
		CString sys_string = PlatformData::SystemDisplayString();
        CString device_string = cStringFormat("{0}", SysInfo::DeviceName());

        AddElement("cwd", Env::CurrentDir());

        AddElement("system", sys_string.c_str());
        AddElement("device", device_string.c_str());
    }
    cVerbose(8,"Writing runtime data");

    {
        auto bdata = AddChildWithText(doc, root, "build", {});

        AddChildWithText(doc, bdata, "version", CoffeeBuildString);
        AddChildWithText(doc, bdata, "compiler", CoffeeCompilerString);
        AddChildWithText(doc, bdata, "architecture", CoffeeArchString);
    }
    cVerbose(8, "Writing build data");

    /* Store system information */
    {
        XML::Element* sysdata = doc.NewElement("sysinfo");
        root->InsertEndChild(sysdata);

        DescribeMemory(doc, sysdata);
        DescribeNetwork(doc, sysdata);
        DescribeProcessor(doc, sysdata);
    }
    cVerbose(8,"Writing system data");

    /* Store extra data gathered by program, is parsed as JSON if possible */
    if(Profiler::ExtraInfo())
    {
        XML::Element* extradata = doc.NewElement("extra");
        root->InsertEndChild(extradata);

        XML::Element* e;
        for(Profiler::ExtraPair const& p : *Profiler::ExtraInfo())
        {
            e = doc.NewElement(p.key.c_str());
            extradata->InsertEndChild(e);

            e->SetText(p.value.c_str());
        }
    }
    cVerbose(8,"Writing extra data");

    /* Only runs in debug mode! */
    if(Profiler::Enabled()){
#ifndef NDEBUG
        /* Store list of threads we've bumped into or labeled */
        {
            XML::Element* threaddata = doc.NewElement("threads");
            root->InsertEndChild(threaddata);

            XML::Element* e;
            for(Profiler::ThreadItem const& p : *Profiler::ThreadNames())
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
            CString st = Convert::uintltostring(Profiler::StartTime());

            /* Create a nice date string */
            CString date = Time::FormattedCurrentTime("%Y%m%dT%H%M%S");
            date = cStringFormat("{0}+{1}",date,Time::Microsecond());

            curr_r->SetAttribute("start1",st.c_str());
            curr_r->SetAttribute("start2",date.c_str());

            /* Some parsing information */
            LinkList<Timestamp> base;
            LinkList<Timestamp> lt;

            base.push_front(Profiler::StartTime());
            lt.push_front(0);

            Profiler::DataPoints()->sort();

            /* Finally, smash data points into XML format */
            for(Profiler::DataPoint const& p : *Profiler::DataPoints())
            {
                switch(p.tp)
                {
                case Profiler::DataPoint::Profile:
                {
                    uint64 ts = p.ts-base.front()- lt.front();

                    XML::Element* n = doc.NewElement("dpoint");

                    CString tsf = Convert::uintltostring(ts);
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

                    CString tsf = Convert::uintltostring(p.ts-base.front());
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
    cVerbose(8,"Writing profiler data");

    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);

    target.insert(0, printer.CStr(), C_CAST<szptr>(printer.CStrSize()));
}

void ExportStringToFile(const CString &data, const Url &outfile)
{
#ifdef COFFEE_LOWFAT
    return;
#endif

//#if defined(COFFEE_ANDROID)
//    const constexpr cstring logtemplate = "/data/local/tmp/{0}_profile.xml";
//#elif defined(COFFEE_RASPBERRYPI) || defined(COFFEE_MAEMO) || defined(COFFEE_APPLE)
//    const constexpr cstring logtemplate = "/tmp/{0}_profile.xml";
//#else
//    cstring logtemplate = outfile;
//#endif

//    CString log_name = cStringFormat(
//                logtemplate,
//                ApplicationData().application_name);

    cVerbose(6,"Creating filename");
    CResources::Resource out(outfile);
    out.data = C_CCAST<c_ptr>(C_FCAST<c_cptr>(data.c_str()));
    /* -1 because we don't want the null-terminator */
    out.size = C_CAST<szptr>(data.size() - 1);
    cVerbose(6,"Retrieving data pointers");
    if(!CResources::FileCommit(out, false, ResourceAccess::Discard))
        cWarning("Failed to export string to file");
    cVerbose(6,"Wrote file");
}

void ExitRoutine()
{
#ifdef COFFEE_LOWFAT
    return;
#endif
    /* Verify if we should export profiler data */
    {
        const constexpr cstring disable_flag = "COFFEE_NO_PROFILER_EXPORT";
        if(!(Env::ExistsVar(disable_flag) && Env::GetVar(disable_flag) == "1"))
        {
            auto log_name = (Path{Env::ExecutableName()}
                    .fileBasename()
                    .removeExt() + "-profile")
                    .addExtension("xml");

            auto log_url = MkUrl(log_name.internUrl.c_str(),
                                 ResourceAccess::SpecifyStorage
                                 |ResourceAccess::TemporaryFile);


            CString target_log;
            Profiling::ExportProfilerData(target_log);
            Profiling::ExportStringToFile(target_log, log_url);

            cVerbose(6, "Saved profiler data to: {0}",
                     FileFun::CanonicalName(log_url));
        }
    }
    /* ... and always destroy the profiler */
    Profiler::DestroyProfiler();
}

}
}
