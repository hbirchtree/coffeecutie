#pragma once

#include <coffee/core/plat/memory/string_ops.h>
#include <coffee/core/plat/memory/stlstring_ops.h>
#include <coffee/core/CDebug>
#include <coffee/core/CProfiling>
#include "http_types.h"

#include <iterator>

namespace Coffee{
namespace HTTP{

using Host = CString;

FORCEDINLINE
void SendProperty(CString& req_s, CString const& p, CString const& v)
{
    CString chunk = cStringFormat("{0}: {1}\r\n",p,v);
    req_s += chunk;
}

template <typename T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& vec)
{
    std::copy( vec.begin(), vec.end(), std::ostream_iterator<T>( os ) );
    return os;
}

template<typename StrmT>
void GenerateRequest(StrmT& req_s, Host const& host, Request const& r)
{
    DProfContext __m("Generating HTTP request");

    CString header;

    header += cStringFormat("{0} {1} {2}\r\n",
                            r.reqtype,r.resource,r.version);

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

    req_s << header;

    Profiler::DeepProfile("Creating header data");

    if(r.payload.size())
        req_s << r.payload;

    Profiler::DeepProfile("Transmitting data");
}

template<typename StrmT>
bool ExtractResponse(StrmT& stream, Response* response)
{
    DProfContext __m("Extracting HTTP response");

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

    std::getline(stream, response->message);
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

    auto& payload = response->payload;

    auto contentLenIt = response->header.find("Content-Length");
    if(contentLenIt != response->header.end())
        payload.reserve(cast_string<u32>(contentLenIt->second));

    Profiler::DeepProfile("Reading header data");

    std::array<char, 4096> payloadBuffer;
    while(stream.read(payloadBuffer.data(), payloadBuffer.size()))
        payload.insert(
                    payload.end(),
                    payloadBuffer.begin(),
                    payloadBuffer.end());

    auto extraSpace = payload.capacity()
            - payload.size();
    payload.resize(payload.capacity());

    if(stream.gcount() <= extraSpace)
        MemCpy(&payload.data()[payload.size() - extraSpace],
               payloadBuffer.data(),
               stream.gcount());

    Profiler::DeepProfile("Reading payload");

    return true;
}

}
}
