#ifndef COFFEE_ASIO_REST_CLIENT_H
#define COFFEE_ASIO_REST_CLIENT_H

#include "rest_request_builder.h"

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
        RestResponse():
            status(0)
        {
        }

	uint32 status;
	CString version;
	CString mimetype;
	CString host;
	CString message;
	CString header;
	CString payload;
    };

    STATICINLINE
    /*!
     * \brief Make a REST request in an asynchronous manner,
     *  will run as task in the background
     * \param context
     * \param p
     * \param h
     * \param r
     * \return
     */
    std::future<RestResponse> RestRequestAsync(
            AsioContext context, Protocol const& p, Host const& h, Request const& r)
    {
        Threads::Function<RestResponse()> fun = [context,p,h,r]()
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
	/* For HTTP/S we use socket streams, because they are simple
	 *  and neat to work with. Their complexity behind
	 *  the scenes is no matter. */
        if(p==HTTP)
            return RestRequestHTTP(h,r);
	else if(p==HTTPS)
            return RestRequestHTTPS(h,r);
	else
	    return RestResponse();
    }

    STATICINLINE RestResponse RestRequestHTTP(Host h, Request r)
    {
        asio::ip::tcp::iostream s;
        s.connect(h,"http");

        GenerateRestRequest(s,h,r);

        RestResponse resp;

        if(!ExtractResponse(s,resp))
            return RestResponse();
        else
            return resp;
    }

    STATICINLINE RestResponse RestRequestHTTPS(Host h, Request r)
    {
        asio::ssl::stream<asio::ip::tcp::socket> socket(t_context->service,
                                                        t_context->sslctxt);

        asio::ip::tcp::resolver::query q(h,"https");
        auto it = t_context->resolver.resolve(q);

        asio::connect(socket.next_layer(),it);

        socket.handshake(asio::ssl::stream_base::client);

        asio::streambuf trans;
        std::ostream os(&trans);
        GenerateRestRequest(os,h,r);
        os.flush();
        asio::write(socket,trans);

        asio::streambuf recp;
        std::istream is(&recp);
        try{
            asio::read(socket,recp);
        }catch(std::system_error)
        {
        }

        RestResponse resp;

        if(!ExtractResponse(is,resp))
            return RestResponse();
        else
            return resp;
    }

    /*!
     * \brief Retrieve the MIME-type of the REST response, used for strict checking
     * \return String representing the format
     */
    STATICINLINE CString GetContentType(RestResponse const& resp)
    {
        CString out;
	/* TODO: Externalize this string
	 *  and use compile-time string processing from C++14 */
	CString query = "content-type:";

	/* We need to somewhat sanitize our input.
	 *  We convert carriage return to newlines, for instance. */
	CString data = Search::CStrReplace(resp.header,"\r","\n");
	data = StrUtil::lower(data);

	cDebug("Data: {0}",data);

	cstring b = StrFind(data.c_str(),query.c_str());
        cstring e = nullptr;
        if(b)
        {
            b+=query.size();
	    e = StrFind(b,"\n");
            if(!e)
                return out;
	    out.insert(0,b,e-b);
	    StrUtil::trim(out);
        }
        return out;
    }

};


}

using REST = CASIO::RestClientImpl;

}

#endif
