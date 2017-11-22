#pragma once

#include <coffee/core/plat/memory/string_ops.h>
#include <coffee/core/plat/memory/stlstring_ops.h>
#include <coffee/core/CDebug>
#include <coffee/core/CProfiling>
#include "http_types.h"

namespace Coffee{
namespace HTTP{

using Host = CString;

FORCEDINLINE
void SendProperty(CString& req_s, CString const& p, CString const& v)
{
    CString chunk = cStringFormat("{0}: {1}\r\n",p,v);
    req_s += chunk;
}

template<typename StrmT>
void GenerateRequest(StrmT& req_s, Host const& host, Request const& r)
{
    ProfContext __m("Generating HTTP request");

    CString header;

    header += cStringFormat("{0} {1} {2}\r\n",r.reqtype,r.resource,r.version);

    SendProperty(header,"Host",host);

    for(Pair<CString,CString> const& p : r.header)
    {
        SendProperty(header,p.first,p.second);
    }

    if(r.mimeType.size())
    {
        SendProperty(header,"Content-Type",r.mimeType);
    }

    if(r.reqtype == "GET")
        SendProperty(header,"Connection","close");


    if(r.payload.size())
        SendProperty(header,"Content-Length",Convert::uintltostring(r.payload.size()));

    header += "\r\n";

    Profiler::Profile("Creating header data");

    if(r.payload.size())
        header += r.payload;

    Profiler::Profile("Creating payload data");
    Profiler::Profile("Dud");

    req_s << header;

    Profiler::Profile("Transmitting data");
}

template<typename StrmT>
bool ExtractResponse(StrmT& stream, Response* response)
{
    ProfContext __m("Extracting HTTP response");

    CString tmp;
    CString t1,t2;

    stream >> response->version;
    stream >> t1;

    {
        bool code_ok = false;
        response->code = Mem::Convert::strtouint(t1.c_str(),10,&code_ok);
        if(!code_ok)
            response->code = 0;
    }

    std::getline(stream,response->message);
    Mem::StrUtil::trim(response->message);

    /* In this case, it's not even an HTTP response */
    if(response->version.substr(0,5)!="HTTP/")
        return false;

    while(std::getline(stream,tmp) && tmp!="\r")
    {
        int64 idx = Search::ChrFind(tmp.c_str(),':')-tmp.c_str();
        cstring end = Search::ChrFind(tmp.c_str(),'\r');
        if(idx > 0)
        {
            t1 = tmp.substr(0,idx);
            t2 = tmp.substr(idx+1,tmp.size());
            if(end)
                t2.resize(t2.size()-1);
            StrUtil::trim(t2);
            response->header[t1] = t2;
        }
    }

    auto contentLenIt = response->header.find("Content-Length");
    if(contentLenIt != response->header.end())
        response->payload.reserve(cast_string<u32>(contentLenIt->second));

    Profiler::Profile("Reading header data");

    response->payload = CString(std::istreambuf_iterator<char>(stream), {});

//    while(std::getline(stream,tmp)&&tmp!="\r\n")
//        response->payload.append(tmp);

    Profiler::Profile("Reading payload");

    return true;
}

}
}
