#include <coffee/asio/rest-client.h>

#include <coffee/asio/rest_request_builder.h>

namespace Coffee{
namespace CASIO{


RestClientImpl::RestResponse RestClientImpl::RestRequest(RestClientImpl::Protocol p, RestClientImpl::Host h, RestClientImpl::Request r)
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

RestClientImpl::RestResponse RestClientImpl::RestRequestHTTP(RestClientImpl::Host h, RestClientImpl::Request const& r)
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

CString RestClientImpl::GetContentType(const RestClientImpl::RestResponse &resp)
{
    CString out;
    /* TODO: Externalize this string
         *  and use compile-time string processing from C++14 */
    CString query = "content-type:";

    /* We need to somewhat sanitize our input.
         *  We convert carriage return to newlines, for instance. */
    CString data = Search::CStrReplace(resp.header,"\r","\n");
    data = StrUtil::lower(data);

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

RestClientImpl::RestResponse RestClientImpl::RestRequestHTTPS(RestClientImpl::Host h, RestClientImpl::Request r)
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

}
}
