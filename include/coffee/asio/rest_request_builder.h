#pragma once

#include <iostream>
#include "asio_data.h"
#include "rest-client.h"

namespace Coffee{
namespace CASIO{

template<typename T>
FORCEDINLINE void GenerateRestRequest(T& req_s, CString host, CString request)
{
    req_s << "GET " << request << " HTTP/1.0\r\n";
    req_s << "HOST: " << host << "\r\n";
    req_s << "Accept: */*\r\n";
    req_s << "CONNECTION: close\r\n\r\n";
}

template<typename SockType, typename TargetType>
FORCEDINLINE bool ExtractResponse(SockType& s, TargetType& resp)
{
    s >> resp.version;
    s >> resp.status;

    std::getline(s,resp.message);
    StrUtil::trim(resp.message);

    if(resp.version.substr(0,5) != "HTTP/")
        return false;

    CString tmp;
    while(std::getline(s,tmp) && tmp != "\r")
        resp.header.append(tmp);

    while(std::getline(s,tmp) && tmp != "\r\n")
        resp.payload.append(tmp);

    return true;
}

}
}