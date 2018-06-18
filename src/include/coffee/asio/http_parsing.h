#pragma once

#include "http_types.h"
#include <coffee/core/CProfiling>
#include <coffee/core/plat/memory/stlstring_ops.h>
#include <coffee/core/plat/memory/string_ops.h>
#include <coffee/core/string_casting.h>

#include <iterator>

namespace Coffee {
namespace HTTP {

using Host = CString;

FORCEDINLINE
void SendProperty(CString& req_s, CString const& p, CString const& v)
{
    DProfContext _("Sending property");
    CString      chunk = p + ": " + v + "\r\n";
    req_s += chunk;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
    std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(os));
    return os;
}

template<
    typename StrmT,
    typename implements<std::ostream, StrmT>::type* = nullptr>
void GenerateRequest(StrmT& req_s, Host const& host, Request const& r)
{
    DProfContext __m("Generating HTTP request");

    CString header;

    header += r.reqtype + " " + r.resource + " " + r.version + "\r\n";

    SendProperty(header, "Host", host);

    for(Pair<CString, CString> const& p : r.header)
    {
        SendProperty(header, p.first, p.second);
    }

    if(r.mimeType.size())
    {
        SendProperty(header, "Content-Type", r.mimeType);
    }

    if(r.reqtype == "GET")
        SendProperty(header, "Connection", "close");

    if(r.payload.size())
        SendProperty(
            header, "Content-Length", cast_pod(r.payload.size()));

    header += "\r\n";

    {
        DProfContext _("Sending header chunk");
        req_s << header;
    }

    Profiler::DeepProfile("Creating header data");

    if(r.payload.size())
        req_s << r.payload;

    Profiler::DeepProfile("Transmitting data");
}

template<
    typename StrmT,
    typename implements<std::istream, StrmT>::type* = nullptr>
bool ExtractResponse(StrmT& stream, Response* response)
{
    DProfContext __m("Extracting HTTP response");

    CString tmp;
    CString t1, t2;

    {
        DProfContext _("Reading response code");
        stream >> response->version;
        stream >> t1;

        bool code_ok   = false;
        response->code = Mem::Convert::strtouint(t1.c_str(), 10, &code_ok);
        if(!code_ok)
            response->code = 0;

        std::getline(stream, response->message);
        Mem::StrUtil::trim(response->message);
    }

    /* In this case, it's not even an HTTP response */
    if(response->version.substr(0, 5) != "HTTP/")
        return false;
    {
        DProfContext _("Reading header fields");
        while(std::getline(stream, tmp) && tmp != "\r")
        {
            auto    idx = Search::ChrFind(tmp.c_str(), ':') - tmp.c_str();
            cstring end = Search::ChrFind(tmp.c_str(), '\r');
            if(idx > 0)
            {
                t1 = tmp.substr(0, C_FCAST<szptr>(idx));
                t2 = tmp.substr(C_FCAST<szptr>(idx + 1), tmp.size());
                if(end)
                    t2.resize(t2.size() - 1);
                StrUtil::trim(t2);
                response->header[t1] = t2;
            }
        }
    }

    auto& payload = response->payload;

    auto contentLenIt = response->header.find("Content-Length");
    if(contentLenIt != response->header.end())
        payload.reserve(cast_string<u32>(contentLenIt->second));

    {
        DProfContext           _("Reading payload");
        std::array<char, 4096> payloadBuffer;
        while(stream.read(payloadBuffer.data(), payloadBuffer.size()))
            payload.insert(
                payload.end(), payloadBuffer.begin(), payloadBuffer.end());

        auto extraSpace = payload.capacity() - payload.size();

        if(stream.gcount() <= extraSpace)
            std::copy(
                payloadBuffer.begin(),
                payloadBuffer.begin() + stream.gcount(),
                std::back_insert_iterator<Vector<byte_t>>(payload));
        //            MemCpy(
        //                &payload.data()[payload.size() - extraSpace],
        //                payloadBuffer.data(),
        //                stream.gcount());
    }

    return true;
}

} // namespace HTTP
} // namespace Coffee
