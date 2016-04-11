#ifndef COFFEE_ASIO_REST_CLIENT_H
#define COFFEE_ASIO_REST_CLIENT_H

#include "asio_data.h"
#include "http_types.h"

namespace Coffee{
namespace CASIO{

struct RestClientImpl : ASIO_Client
{
    using Host = CString;
    using Request = CString;

    using RestResponse = HTTP::Response;

    enum Protocol
    {
        HTTP = HTTP::Trans_HTTP,
        HTTPS = HTTP::Trans_HTTPS,
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
    Future<RestResponse> RestRequestAsync(
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

    /*!
     * \brief Basic function for making a REST request, will automatically decide between HTTP and HTTPS as requested
     * \param p
     * \param h
     * \param r
     * \return
     */
    static RestResponse RestRequest(Protocol p, Host h, Request r);

    static RestResponse RestRequestHTTP(Host h, const Request &r);

    static RestResponse RestRequestHTTPS(Host h, Request r);

    /*!
     * \brief Retrieve the MIME-type of the REST response, used for strict checking
     * \return String representing the format
     */
    static CString GetContentType(RestResponse const& resp);

};


}

using REST = CASIO::RestClientImpl;

}

#endif
