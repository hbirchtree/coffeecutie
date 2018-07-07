#include <coffee/core/profiler/profiling-export.h>

#include <coffee/core/base/files/url.h>

#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/CJSONParser>
#include <coffee/core/CProfiling>
#include <coffee/core/CXmlParser>

#include <coffee/core/coffee.h>
#include <coffee/core/coffee_mem_macros.h>

#include <coffee/core/plat/plat_file.h>
#include <coffee/core/plat/plat_quirks_toggling.h>

#ifndef COFFEE_LOWFAT

namespace Coffee {
namespace Profiling {

static constexpr szptr MAX_NUMBER_OF_ENTRIES = 1024 * 1024;

/*!
 * \brief The DataPointGenerator iterates over all DataPoints in all threads
 */
struct DataPointGenerator
{
    DataPointGenerator()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        /* Sort the thread list to make it deterministic */
        using TPair = Pair<ThreadId::Hash, CString>;

        Vector<TPair> names;
        names.insert(
            names.end(),
            State::GetProfilerStore()->threadnames.begin(),
            State::GetProfilerStore()->threadnames.end());

        std::sort(
            names.begin(), names.end(), [](TPair const& v1, TPair const& v2) {
                return v1.second < v2.second;
            });

        std::transform(
            names.begin(),
            names.end(),
            std::back_inserter(m_threadHashes),
            [](TPair const& v) { return v.first; });
#endif
    }

    struct iterator : Iterator<ForwardIteratorTag, DataPoint>
    {
        static const constexpr szptr npos = C_CAST<szptr>(-1);

        iterator(DataPointGenerator& gen) :
            generator(&gen), current_index(0), current_thread(0)
        {
            m_iterator = intern_data()->datapoints.begin();
            find_next();
        }
        iterator(DataPointGenerator& gen, int) :
            generator(&gen), current_index(npos), current_thread(npos)
        {
        }

        iterator& operator++()
        {
            current_index++;
            if(m_iterator != intern_data()->datapoints.end())
                m_iterator++;
            find_next();
            return *this;
        }

        bool operator==(iterator const& other) const
        {
            return other.current_index == current_index &&
                   other.current_thread == current_thread &&
                   (other.generator) == (generator);
        }

        bool operator!=(iterator const& other) const
        {
            return !(*this == other);
        }

        DataPoint const& operator*() const
        {
            return *m_iterator;
//            return intern_data()->datapoints.at(current_index);
        }

      private:
        LinkList<DataPoint>::iterator m_iterator;

        void find_next()
        {
            while(current_thread < generator->m_threadHashes.size() &&
                  current_index >= intern_data()->datapoints.size())
            {
                current_thread++;
                current_index = 0;
                if(current_thread < generator->m_threadHashes.size())
                    m_iterator = intern_data()->datapoints.begin();
            }
            if(current_thread >= generator->m_threadHashes.size())
            {
                current_thread = npos;
                current_index  = npos;
            }
        }

        ThreadData* intern_data() const
        {
#if !defined(COFFEE_DISABLE_PROFILER)
            auto key = generator->m_threadHashes.at(current_thread);
            return State::GetProfilerStore()->thread_refs[key].get();
#else
            throw implementation_error("profiler not available");
#endif
        }

        DataPointGenerator* generator;
        szptr               current_index;
        szptr               current_thread;
    };

    szptr size()
    {
        szptr counter = 0;

#if !defined(COFFEE_DISABLE_PROFILER)
        for(auto const& hash : m_threadHashes)
            counter += State::GetProfilerStore()
                           ->thread_refs[hash]
                           .get()
                           ->datapoints.size();
#endif

        return counter;
    }

    /*!
     * \brief As a safety measure, we disable saving the trace
     *  after 1 million entries. This was proven useful in the wild.
     * \return
     */
    iterator begin()
    {
        if(size() < MAX_NUMBER_OF_ENTRIES)
            return iterator(*this);
        else
        {
            cWarning("Trace events too large, all events discarded!");
            return end();
        }
    }

    iterator end()
    {
        return iterator(*this, 0);
    }

  private:
    Vector<ThreadId::Hash> m_threadHashes;
};

static Vector<DataPoint> GetSortedDataPoints()
{
    DataPointGenerator gen;
    Vector<DataPoint>  points;
    points.insert(points.begin(), gen.begin(), gen.end());

    //    std::sort(points.begin(), points.end());

    return points;
}

static CString AnonymizePath(CString const& p)
{
    return CStrReplace(
        CStrReplace(p, Env::GetUserHome().internUrl, "~"),
        Env::ExistsVar("USER") ? Env::GetVar("USER") : "",
        "user");
}

static CString AnonymizePath(Url const& p)
{
    return AnonymizePath(p.internUrl);
}

void PrintProfilerData()
{
#ifdef COFFEE_LOWFAT
    return;
#endif

#ifndef NDEBUG
    cBasicPrint("Profiling information:");

    LinkList<uint64> base_time;     /* Stack of time values */
    LinkList<uint64> curr_timeline; /* Progression of time within a context */
    for(Profiling::DataPoint const& p : DataPointGenerator())
    {
        if(p.tp == Profiling::DataPoint::Profile)
        {
            uint64 ts = (p.ts - base_time.front()) - curr_timeline.front();
            cBasicPrint("Time: {0}, label: {1}", ts, p.name);

            curr_timeline.front() = p.ts - base_time.front();
        } else if(p.tp == Profiling::DataPoint::Push)
        {
            base_time.push_front(p.ts);
            curr_timeline.push_front(0);
            cBasicPrint("Enter scope: {0}", p.name, p.ts);
        } else if(p.tp == Profiling::DataPoint::Pop)
        {
            base_time.pop_front();
            curr_timeline.pop_front();
            cBasicPrint("Exit scope: {0}", p.name, p.ts);
        }
    }
#endif
}

namespace XML_Stuff {

STATICINLINE XML::Element* AddChildWithText(
    XML::Document& doc, XML::Element* e, cstring tagname, CString text)
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

    AddChildWithText(doc, cache, "l1", cast_pod(SysInfo::ProcessorCacheSize()));

    AddChildWithText(
        doc,
        proc_detail,
        "frequencies",
        cast_pod(SysInfo::ProcessorFrequency()));

    auto hier = AddChildWithText(doc, proc_detail, "hierarchy", {});

    ElementSetAttr(hier, "cores", cast_pod(SysInfo::CoreCount()));
    ElementSetAttr(hier, "threads", cast_pod(SysInfo::ThreadCount()));

    if(SysInfo::HasHyperThreading())
        AddChildWithText(doc, proc_detail, "hyperthreading", {});
}

} // namespace XML_Stuff

void ExportProfilerData(CString& target)
{
#ifdef COFFEE_LOWFAT
    return;
#endif

    using namespace XML_Stuff;

    auto const& app_args = GetInitArgs().arguments();

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
        /* TODO: Make variable whitelist instead of including all of them, for
         * privacy */
        for(Env::VarPair const& v : vars)
        {
            p = doc.NewElement("v");
            p->SetAttribute("name", v.first.c_str());
            p->SetText(v.second.c_str());
            envdata->InsertEndChild(p);
        }
    }
    cVerbose(8, "Writing environment data");
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

        auto AddElement = [&](cstring tagname, CString text) {
            AddChildWithText(doc, rundata, tagname, text);
        };

        /* Current working directory,
         * most likely application directory
         *  or home directory */
        CString sys_string    = PlatformData::SystemDisplayString();
        CString device_string = cStringFormat("{0}", SysInfo::DeviceName());

        AddElement("cwd", Env::CurrentDir().internUrl);

        AddElement("system", sys_string.c_str());
        AddElement("device", device_string.c_str());
    }
    cVerbose(8, "Writing runtime data");

    {
        auto bdata = AddChildWithText(doc, root, "build", {});

        auto const& buildi = State::GetBuildInfo();

        AddChildWithText(doc, bdata, "version", buildi.build_version);
        AddChildWithText(doc, bdata, "compiler", buildi.compiler);
        AddChildWithText(doc, bdata, "architecture", buildi.architecture);
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
    cVerbose(8, "Writing system data");

    /* Store extra data gathered by program, is parsed as JSON if possible */
    if(Profiler::ExtraInfo())
    {
        XML::Element* extradata = doc.NewElement("extra");
        root->InsertEndChild(extradata);

        XML::Element* e;
        for(Profiling::ExtraPair const& p : *Profiler::ExtraInfo())
        {
            e = doc.NewElement(p.key.c_str());
            extradata->InsertEndChild(e);

            e->SetText(p.value.c_str());
        }
    }
    cVerbose(8, "Writing extra data");

    /* Only runs in debug mode! */
    if(Profiler::HasData())
    {
#ifndef NDEBUG
        /* Store list of threads we've bumped into or labeled */
        {
            XML::Element* threaddata = doc.NewElement("threads");
            root->InsertEndChild(threaddata);

            XML::Element* e;
            for(Profiling::ThreadItem const& p : *Profiler::ThreadNames())
            {
                e = doc.NewElement("thread");
                threaddata->InsertEndChild(e);

                CString id = StrUtil::pointerify(p.first);
                e->SetAttribute("id", id.c_str());
                e->SetAttribute("name", p.second.c_str());
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
            date         = cStringFormat("{0}+{1}", date, Time::Microsecond());

            curr_r->SetAttribute("start1", st.c_str());
            curr_r->SetAttribute("start2", date.c_str());

            /* Some parsing information */
            LinkList<Timestamp> base;
            LinkList<Timestamp> lt;

            base.push_front(Profiler::StartTime());
            lt.push_front(0);

            /* Finally, smash data points into XML format */
            for(Profiling::DataPoint const& p : GetSortedDataPoints())
            {
                switch(p.tp)
                {
                case Profiling::DataPoint::Profile:
                {
                    uint64 ts = p.ts - base.front() - lt.front();

                    XML::Element* n = doc.NewElement("dpoint");

                    CString tsf = Convert::uintltostring(ts);
                    CString tid = StrUtil::pointerify(p.thread);

                    n->SetAttribute("ts", tsf.c_str());
                    n->SetAttribute("label", p.name.c_str());
                    n->SetAttribute("thread", tid.c_str());

                    curr->InsertEndChild(n);

                    lt.front() = p.ts - base.front();
                    break;
                }
                case Profiling::DataPoint::Push:
                {
                    XML::Element* n = doc.NewElement("context");

                    CString tsf = Convert::uintltostring(p.ts - base.front());
                    CString tid = StrUtil::pointerify(p.thread);

                    n->SetAttribute("ts", tsf.c_str());
                    n->SetAttribute("label", p.name.c_str());
                    n->SetAttribute("thread", tid.c_str());

                    curr->InsertEndChild(n);
                    curr = n;

                    base.push_front(p.ts);
                    lt.push_front(0);
                    break;
                }
                case Profiling::DataPoint::Pop:
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
    cVerbose(8, "Writing profiler data");

    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);

    target.insert(0, printer.CStr(), C_CAST<szptr>(printer.CStrSize()));
}

namespace CT_Stuff {

STATICINLINE JSON::Value FromString(
    CString const& s, JSON::Document::AllocatorType& alloc)
{
    JSON::Value j;
    j.SetString(s.c_str(), alloc);

    return j;
}

STATICINLINE void PutEvents(
    JSON::Value& target, JSON::Document::AllocatorType& alloc)
{
    if(!Profiler::HasData())
        return;

    /* Some parsing information */
    auto start = Profiler::StartTime();

    for(Profiling::DataPoint const& p : GetSortedDataPoints())
    {
        JSON::Value o;
        o.SetObject();

        CString tid = StrUtil::pointerify(p.thread);

        if((*Profiler::ThreadNames())[p.thread].size())
            tid = (*Profiler::ThreadNames())[p.thread];

        auto catVal  = FromString(p.component, alloc);
        auto tidVal  = FromString(tid, alloc);
        auto nameVal = FromString(p.name, alloc);

        o.AddMember("ts", JSON::Value(p.ts - start), alloc);
        o.AddMember("name", nameVal, alloc);
        o.AddMember("pid", JSON::Value(1), alloc);
        o.AddMember("tid", tidVal, alloc);
        o.AddMember("cat", catVal, alloc);

        switch(p.tp)
        {
        case Profiling::DataPoint::Profile:
        {
            if(feval(p.at & Profiling::DataPoint::Hot))
                o.AddMember("ph", "P", alloc);
            else
                o.AddMember("ph", "i", alloc);

            o.AddMember("s", "t", alloc);
            break;
        }
        case Profiling::DataPoint::Push:
        {
            o.AddMember("ph", "B", alloc);
            break;
        }
        case Profiling::DataPoint::Pop:
        {
            o.AddMember("ph", "E", alloc);
            break;
        }
        }

        target.PushBack(o, alloc);
    }
}

STATICINLINE void PutArgs(
    JSON::Value& target, JSON::Document::AllocatorType& alloc)
{
    auto args = GetInitArgs().originalArguments();

    for(auto const& arg : args)
    {
        target.PushBack(FromString(AnonymizePath(arg), alloc), alloc);
    }
}

STATICINLINE void PutExtraData(
    JSON::Value& target, JSON::Document::AllocatorType& alloc)
{
    if(!Profiler::ExtraInfo())
        return;

    for(auto info : *Profiler::ExtraInfo())
    {
        target.AddMember(
            FromString(info.key, alloc), FromString(info.value, alloc), alloc);
    }
}

STATICINLINE void PutRuntimeInfo(
    JSON::Value& target, JSON::Document::AllocatorType& alloc)
{
    auto const& buildi = State::GetBuildInfo();

    target.AddMember(
        "build.version", FromString(buildi.build_version, alloc), alloc);
    target.AddMember(
        "build.compiler", FromString(buildi.compiler, alloc), alloc);
    target.AddMember(
        "build.architecture", FromString(buildi.architecture, alloc), alloc);

    target.AddMember(
        "runtime.system",
        FromString(PlatformData::SystemDisplayString(), alloc),
        alloc);
    target.AddMember(
        "device",
        FromString(Strings::to_string(SysInfo::DeviceName()), alloc),
        alloc);

    target.AddMember("device.dpi", PlatformData::DeviceDPI(), alloc);
    target.AddMember("device.type", PlatformData::DeviceVariant(), alloc);
    target.AddMember("device.platform", PlatformData::PlatformVariant(), alloc);
    target.AddMember("device.debug", PlatformData::IsDebug(), alloc);

    target.AddMember(
        "device.version",
        FromString(Strings::to_string(SysInfo::GetSystemVersion()), alloc),
        alloc);
    target.AddMember(
        "device.motherboard",
        FromString(Strings::to_string(SysInfo::Motherboard()), alloc),
        alloc);
    target.AddMember(
        "device.chassis",
        FromString(Strings::to_string(SysInfo::Chassis()), alloc),
        alloc);

#if defined(COFFEE_INTERNAL_BUILD)
    target.AddMember(
        "device.hostname",
        FromString(Strings::to_string(SysInfo::HostName()), alloc),
        alloc);
#endif

    auto pinfo = SysInfo::Processor();
    target.AddMember(
        "processor.manufacturer", FromString(pinfo.manufacturer, alloc), alloc);
    target.AddMember("processor.model", FromString(pinfo.model, alloc), alloc);
    target.AddMember(
        "processor.firmware", FromString(pinfo.firmware, alloc), alloc);

    auto freqs = SysInfo::ProcessorFrequencies();
    if(freqs.size())
    {
        JSON::Value freq_j;
        freq_j.SetArray();
        for(auto f : freqs)
            freq_j.PushBack(f, alloc);
        target.AddMember("processor.frequency", freq_j, alloc);
    } else
        target.AddMember(
            "processor.frequency", SysInfo::ProcessorFrequency(), alloc);

    target.AddMember("processor.cores", SysInfo::CoreCount(), alloc);
    target.AddMember("processor.threads", SysInfo::ThreadCount(), alloc);

    target.AddMember(
        "processor.hyperthreading", SysInfo::HasHyperThreading(), alloc);
    target.AddMember("processor.pae", SysInfo::HasPAE(), alloc);
    target.AddMember("processor.fpu", SysInfo::HasFPU(), alloc);

    target.AddMember("memory.bank", SysInfo::MemTotal(), alloc);
    target.AddMember("memory.virtual.total", SysInfo::SwapTotal(), alloc);
    target.AddMember(
        "memory.virtual.available", SysInfo::SwapAvailable(), alloc);

    auto cwd = Env::CurrentDir();

    target.AddMember(
        "runtime.cwd", FromString(AnonymizePath(cwd), alloc), alloc);
}

} // namespace CT_Stuff

void ExportChromeTracerData(CString& target)
{
    using namespace CT_Stuff;

    JSON::Document doc;
    doc.SetObject();

    JSON::Value events;
    events.SetArray();
    PutEvents(events, doc.GetAllocator());

    JSON::Value args;
    args.SetArray();
    PutArgs(args, doc.GetAllocator());

    JSON::Value extraData;
    extraData.SetObject();
    PutExtraData(extraData, doc.GetAllocator());

    auto appd = ApplicationData();
    doc.AddMember(
        "application.name",
        FromString(appd.application_name, doc.GetAllocator()),
        doc.GetAllocator());
    doc.AddMember(
        "application.org",
        FromString(appd.organization_name, doc.GetAllocator()),
        doc.GetAllocator());
    doc.AddMember(
        "application.version",
        ApplicationData().version_code,
        doc.GetAllocator());
    PutRuntimeInfo(doc, doc.GetAllocator());
    doc.AddMember("runtime.arguments", args, doc.GetAllocator());

    doc.AddMember("extra", extraData, doc.GetAllocator());

    doc.AddMember("traceEvents", events, doc.GetAllocator());

    target = JSON::Serialize(doc);
}

void ExportStringToFile(const CString& data, const Url& outfile)
{
#ifdef COFFEE_LOWFAT
    return;
#endif

    cVerbose(6, "Creating filename");
    CResources::Resource out(outfile);
    out.data = C_CCAST<c_ptr>(C_FCAST<c_cptr>(data.c_str()));
    /* -1 because we don't want the null-terminator */
    out.size = C_CAST<szptr>(data.size() - 1);
    cVerbose(6, "Retrieving data pointers");

    if(!CResources::FileCommit(out, ResourceAccess::Discard))
        cWarning("Failed to export string to file");

    cVerbose(6, "Wrote file");
}

void ExitRoutine()
{
#ifdef COFFEE_LOWFAT
    return;
#endif

    Profiler::DisableProfiler();

    file_error ec;

    /* Verify if we should export profiler data */
    {
        const constexpr cstring disable_flag = "COFFEE_NO_PROFILER_EXPORT";
        if(!(Env::ExistsVar(disable_flag) && Env::GetVar(disable_flag) == "1"))
        {
            auto log_name = (Path{Env::ExecutableName()}.fileBasename());

            auto log_url = MkUrl(
                "",
                ResourceAccess::SpecifyStorage | ResourceAccess::TemporaryFile);

            auto log_url2 =
                log_url +
                Path(log_name.internUrl + "-chrome").addExtension("json");

            log_url = log_url +
                      Path(log_name.internUrl + "-profile").addExtension("xml");

            CString target_log;

            CString target_chrome;
            Profiling::ExportChromeTracerData(target_chrome);

            Profiling::ExportProfilerData(target_log);

            Profiling::ExportStringToFile(target_log, log_url);
            Profiling::ExportStringToFile(target_chrome + " ", log_url2);

            cVerbose(
                6,
                "Saved profiler data to: {0}",
                FileFun::CanonicalName(log_url, ec));
        }
    }
}

} // namespace Profiling
} // namespace Coffee

#endif
