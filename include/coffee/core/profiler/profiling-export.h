#pragma once

#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/CXmlParser>

#include "../coffee_mem_macros.h"

namespace Coffee{
namespace Profiling{

FORCEDINLINE void PrintProfilerData()
{
#ifndef NDEBUG
    cBasicPrint("Profiling information:");

    LinkList<uint64> base;
    LinkList<uint64> lt;
    for(Profiler::DataPoint const& p : *Profiler::datapoints)
    {
	if(p.tp==Profiler::DataPoint::Profile)
	{
	    uint64 ts = (p.ts-base.front())-lt.front();
	    cBasicPrint("Time: {0}, label: {1}",
			StrUtil::spacepad(ts,10),
			p.name);

	    lt.front() = p.ts-base.front();
	}
	else if(p.tp==Profiler::DataPoint::Push)
	{
	    base.push_front(p.ts);
	    lt.push_front(0);
	    cBasicPrint("Enter scope: {0}",p.name,p.ts);
	}
	else if(p.tp==Profiler::DataPoint::Pop)
	{
	    base.pop_front();
	    lt.pop_front();
	    cBasicPrint("Exit scope: {0}",p.name,p.ts);
	}
    }
#endif
}

FORCEDINLINE void ExportProfilerData(cstring out)
{
    CResources::CResource outfile(out);

    XML::Document* doc = new XML::Document();


}

}
}
