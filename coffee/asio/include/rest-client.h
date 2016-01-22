#ifndef COFFEE_ASIO_REST_CLIENT_H
#define COFFEE_ASIO_REST_CLIENT_H

#include <coffee/core/CBase>
#include <coffee/core/CTypes>
#include <coffee_strings.h>
#include <asio.hpp>
#include <iostream>

namespace Coffee{
namespace CASIO{

struct RestClientImpl{
    using Host = CString;
    using Request = CString;

    enum Protocol
    {
	HTTP  = 0,
	HTTPS = 1,
    };

    struct RestContext
    {
	RestContext():
	    resolver(service)
	{
	}

	asio::io_service service;
	asio::ip::tcp::resolver resolver;
    };

    struct RestResponse
    {
	uint32 status;
	CString version;
	CString mimetype;
	CString host;
	CString message;
	CString header;
	CString payload;
    };

    static inline C_FORCE_INLINE void InitService()
    {
	t_context = new RestContext();
    }

    static inline C_FORCE_INLINE RestContext* GetContext()
    {
	RestContext* t = t_context;
	t_context = nullptr;
	return t;
    }

    static inline C_FORCE_INLINE bool MakeCurrent(RestContext* c)
    {
	if(t_context)
	{
	    cLog(__FILE__,__LINE__,CFStrings::ASIO_Library_Name,
		 CFStrings::ASIO_Library_MakeCurrent_Error);
	    return false;
	}
	t_context = c;
	return true;
    }

    static inline C_FORCE_INLINE std::future<RestResponse> RestRequestAsync(
	    Protocol p, Host h, Request r)
    {
	std::function<RestResponse(Protocol,Host,Request)> fun = RestRequest;
	return CThreading::RunAsync(fun,p,h,r);
    }

    static inline C_FORCE_INLINE RestResponse RestRequest(Protocol p, Host h, Request r)
    {
	CString protocol = "";
	switch(p)
	{
	case HTTP:
	    protocol = "http";
	    break;
	case HTTPS:
	    protocol = "https";
	    break;
        }

	asio::ip::tcp::resolver::query q(h,protocol);

	auto it = t_context->resolver.resolve(q);

	asio::ip::tcp::socket s(t_context->service);
	asio::connect(s,it);

	asio::streambuf req;
	std::ostream req_s(&req);
	req_s << "GET " << r << " HTTP/1.0\r\n";
	req_s << "HOST: " << h << "\r\n";
	req_s << "CONNECTION: close\r\n\r\n";

	asio::write(s,req);

	asio::streambuf res;
	asio::read_until(s,res,"\r\n");

	std::istream res_s(&res);

	std::string http_ver;
	uint32 stat;

	res_s >> http_ver;
	res_s >> stat;

	RestResponse resp;
	resp.status = stat;
	resp.version = http_ver;

	std::string message;

	std::getline(res_s,message);

	resp.message = message;

	if(!res_s||http_ver.substr(0,5)!="HTTP/")
	    return resp;

	asio::read_until(s,res,"\r\n\r\n");

	std::string header;
	while(std::getline(res_s,header)&&header!="\r")
        {
            cDebug("Header: {0}",header);
        }

        std::ostringstream ss;
	if(res.size()>0)
        {
            ss << &res;
            resp.payload.append(ss.str());
        }

        try{
            while(asio::read(s,res,asio::transfer_at_least(1)))
	    {
                ss << &res;
                resp.payload.append(ss.str());
	    }
	}
	catch(std::system_error)
	{
	}

	return resp;
    }

private:
    static thread_local RestContext* t_context;
};

thread_local RestClientImpl::RestContext* RestClientImpl::t_context = nullptr;

}

using RestClient = CASIO::RestClientImpl;

}

#endif
