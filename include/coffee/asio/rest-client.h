#ifndef COFFEE_ASIO_REST_CLIENT_H
#define COFFEE_ASIO_REST_CLIENT_H

#include "asio_data.h"
#include <iostream>

namespace Coffee{
namespace CASIO{

struct RestClientImpl : ASIO_Client
{
    using Host = CString;
    using Request = CString;

    enum Protocol
    {
        Undefined = 0,
        HTTP  = 1,
        HTTPS = 2,
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

    STATICINLINE std::future<RestResponse> RestRequestAsync(
            AsioContext context, Protocol const& p, Host const& h, Request const& r)
    {
        std::function<RestResponse()> fun = [context,p,h,r]()
        {
            MakeCurrent(context);
	    RestResponse res = RestRequest(p,h,r);
	    GetContext();
	    return res;
        };
        return Threads::RunAsync(fun);
    }

    STATICINLINE RestResponse RestRequest(Protocol p, Host h, Request r)
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
        default:
            return RestResponse();
        }

	asio::ip::tcp::resolver::query q(h,protocol);

        auto it = t_context->resolver.resolve(q);

        asio::ip::tcp::socket s(t_context->service);
	asio::connect(s,it);

        /* Send the HTTP/S request */
	asio::streambuf req;
	std::ostream req_s(&req);
	req_s << "GET " << r << " HTTP/1.0\r\n";
	req_s << "HOST: " << h << "\r\n";
	req_s << "CONNECTION: close\r\n\r\n";

	asio::write(s,req);

        /* Get the response */
	asio::streambuf res;
	asio::read_until(s,res,"\r\n");

	std::istream res_s(&res);

	std::string http_ver;
	uint32 stat;

        /* Get version of HTTP as well as status code */
	res_s >> http_ver;
	res_s >> stat;

	RestResponse resp;
	resp.status = stat;
	resp.version = http_ver;

        /* Get HTTP message */
	std::string message;
	std::getline(res_s,message);
	resp.message = message;

	if(!res_s||http_ver.substr(0,5)!="HTTP/")
	    return resp;

	asio::read_until(s,res,"\r\n\r\n");

        /* Read the header */
	std::string header;
	while(std::getline(res_s,header)&&header!="\r")
        {
            resp.header.append(header+"\n");
        }

        /* Read full payload */
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
        /* We receive an exception on end of stream */
        catch(std::system_error)
        {
        }

	return resp;
    }

    /*!
     * \brief Retrieve the MIME-type of the REST response, used for strict checking
     * \return String representing the format
     */
    STATICINLINE CString GetContentType(RestResponse const& resp)
    {
        CString out;
        CString query = "Content-Type: ";

        cstring b = CStrFind(resp.header.c_str(),query.c_str());
        cstring e = nullptr;
        if(b)
        {
            b+=query.size();
            e = CStrFind(b,"\r\n");
            if(!e)
                return out;
            out.resize(e-b);
            CMemCpy(&out[0],b,e-b);
        }
        return out;
    }

};


}

using RestClient = CASIO::RestClientImpl;

}

#endif
