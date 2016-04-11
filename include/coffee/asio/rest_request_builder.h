#pragma once

#include <iostream>
#include "asio_data.h"
#include "rest-client.h"

namespace Coffee{
namespace CASIO{

template<typename T> FORCEDINLINE
/*!
 * \brief Generate an HTTP REST request, few details
 * \param req_s
 * \param host
 * \param request
 */
void GenerateRestRequest(T& req_s, CString host, CString request)
{
    req_s << "GET " << request << " HTTP/1.0\r\n";
    req_s << "HOST: " << host << "\r\n";
    req_s << "Accept: */*\r\n";
    req_s << "CONNECTION: close\r\n\r\n";
}

template<typename SockType, typename TargetType> FORCEDINLINE
/*!
 * \brief Extract an HTTP response from a TCP socket
 * \param s
 * \param resp
 * \return
 */
bool ExtractResponse(SockType& s, TargetType& resp)
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
