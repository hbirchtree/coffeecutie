#pragma once

#include <coffee/core/plat/memory/string_ops.h>
#include <coffee/core/plat/memory/stlstring_ops.h>
#include "http_types.h"

namespace Coffee{
namespace HTTP{

using Host = CString;

template<typename StrmT>
/*!
 * \brief Generic HTTP header data insertion, re-usable
 * \param stream
 * \param extra
 */
void GenerateRequestHeader(StrmT& stream,
                           Request const& extra)
{
    for(std::pair<CString,CString> const& v : extra.values)
    {
        stream << v.first << ": " << v.second << "\r\n";
    }
}

template<typename StrmT>
void GenerateRequest(StrmT& stream, Host const& host,
                     CString const& request,
                     Request const& extra)
{
    CString ver_string = "1.0";
    if(extra.version.size()>0)
        ver_string = extra.version;

    /* Generate common HTTP structure */
    stream << "GET " << request << " HTTP/" << ver_string << "\r\n";
    stream << "HOST: " << host << "\r\n";
    /* Add extra data if applicable, can be reused if in a batch */
    GenerateRequestHeader(stream,extra);
    /* Choose what to do with the socket afterwards */
    stream << "Connection: CLOSE\r\n\r\n";
}

template<typename StrmT>
bool ExtractResponse(StrmT& stream, Response* response)
{
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

    while(std::getline(stream,tmp)&&tmp!="\r")
    {
        szptr idx = tmp.find(":",0,tmp.size());
        if(idx > 0)
        {
            t1 = tmp.substr(0,idx-1);
            t2 = tmp.substr(idx+1,tmp.size());
            response->values[t1] = t2;
        }
    }

    while(std::getline(stream,tmp)&&tmp!="\r\n")
        response->payload.append(tmp);

    return true;
}

}
}
