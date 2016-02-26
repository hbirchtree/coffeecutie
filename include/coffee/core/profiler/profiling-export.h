#pragma once

#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/CXmlParser>

#include "../coffee_mem_macros.h"

#include "../plat/file/file_def.h"

namespace Coffee{
namespace Profiling{

FORCEDINLINE void PrintProfilerData()
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
			StrUtil::spacepad(ts,10),
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

FORCEDINLINE void ExportProfilerData(cstring out, int32 argc = 0, cstring_w* argv = nullptr)
{
    XML::Document doc;

    {
        XML::Element* envdata = doc.NewElement("environment");
        doc.InsertFirstChild(envdata);

        Env::Variables vars = Env::Environment();

        XML::Element* p;
        for(Env::VarPair const& v : vars)
        {
            p = doc.NewElement("v");
            p->SetAttribute("name",v.first.c_str());
            p->SetAttribute("value",v.second.c_str());
            envdata->InsertEndChild(p);
        }

    }

    XML::Element* curr = doc.NewElement("profile");
    doc.InsertFirstChild(curr);

    /**/
    CString st = cStringFormat("{0}",*Profiler::start_time);

    /* Create a nice date string */
    CString date = Time::FormattedCurrentTime("%Y%m%dT%H%M%S");
    date = cStringFormat("{0}+{1}",date,Time::Microsecond());

    CString cmdline;
    for(int32 i=0;i<argc;i++)
    {
        cmdline.append(argv[i]);
        if(i<argc-1)
            cmdline.append(" ");
    }

    CString cwd = Env::CurrentDir();

    curr->SetAttribute("start1",st.c_str());
    curr->SetAttribute("start2",date.c_str());
    curr->SetAttribute("command",cmdline.c_str());
    curr->SetAttribute("cwd",cwd.c_str());


    LinkList<Timestamp> base;
    LinkList<Timestamp> lt;

    base.push_front(*Profiler::start_time);

    for(Profiler::DataPoint const& p : *Profiler::datapoints)
    {
        switch(p.tp)
        {
        case Profiler::DataPoint::Profile:
        {
            uint64 ts = (p.ts-base.front()-lt.front());

            XML::Element* n = doc.NewElement("dpoint");

            CString tsf = cStringFormat("{0}",ts);
            CString tid = cStringFormat(
                        "0x{0}",StrUtil::hexify(p.thread.hash()));

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

            CString tsf = cStringFormat("{0}",p.ts-base.front());
            CString tid = cStringFormat(
                        "0x{0}",StrUtil::hexify(p.thread.hash()));

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
            break;
        }
        }
    }

    auto file = CResources::FileFun::Open(out,ResourceAccess::WriteOnly);

    /* Because fuck dangling file handles */
    try{
        doc.SaveFile(file->handle,false);
    }catch(std::exception)
    {
    }

    CResources::FileFun::Close(file);
}

}
}
