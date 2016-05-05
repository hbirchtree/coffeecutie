#include <coffee/asio/rest-client.h>

#include <coffee/asio/http_parsing.h>

namespace Coffee{
namespace CASIO{


RestClientImpl::RestResponse RestClientImpl::RestRequest(Host h, Request const& r)
{
    /* For HTTP/S we use socket streams, because they are simple
         *  and neat to work with. Their complexity behind
         *  the scenes is no matter. */
    if(r.transp==HTTP)
        return RestRequestHTTP(h,r);
    else if(r.transp==HTTPS)
        return RestRequestHTTPS(h,r);
    else
        return RestResponse();
}

RestClientImpl::RestResponse RestClientImpl::RestRequestHTTP(Host h, Request const& r)
{
    ProfContext _m("ASIO request");

    CString port;
    switch(r.port)
    {
    case 0:
        port = "http";
        break;
    default:
        port = Convert::uintstostring(r.port);
        break;
    }

    asio::ip::tcp::iostream s;
    s.connect(h,port);

    Profiler::Profile("Connect");

    HTTP::GenerateRequest(s,h,r);

    Profiler::Profile("Request generation");

    RestResponse resp;

    if(!HTTP::ExtractResponse(s,&resp))
        return RestResponse();
    else
    {
        Profiler::Profile("Response gathering");
        return resp;
    }
}

RestClientImpl::RestResponse RestClientImpl::RestRequestHTTPS(Host h, Request const& r)
{
    asio::ssl::stream<asio::ip::tcp::socket> socket(t_context->service,
                                                    t_context->sslctxt);

    CString port;
    switch(r.port)
    {
    case 0:
        port = "https";
        break;
    default:
        port = Convert::uintstostring(r.port);
        break;
    }

    asio::ip::tcp::resolver::query q(h,port);
    auto it = t_context->resolver.resolve(q);

    asio::connect(socket.next_layer(),it);

    socket.handshake(asio::ssl::stream_base::client);

    asio::streambuf trans;
    std::ostream os(&trans);
    HTTP::GenerateRequest(os,h,r);
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

    if(!HTTP::ExtractResponse(is,&resp))
        return RestResponse();
    else
        return resp;
}

CString RestClientImpl::GetContentType(const RestResponse &resp)
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

}
}
