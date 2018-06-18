#ifndef COFFEE_ASIO_REST_CLIENT_H
#define COFFEE_ASIO_REST_CLIENT_H

#include "asio_data.h"
#include "http_types.h"
#include <coffee/core/base/threading/job_system.h>

namespace Coffee{
namespace ASIO{

struct RestClientImpl : ASIO_Client
{
    using Request = HTTP::_http_request<CString>;
    using Response = HTTP::Response;

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
    Future<Response> RestRequestAsync(
            AsioContext_internal context, Host const& h, Request const& r)
    {
        Function<Response()> fun = [=]()
        {
            Response res = RestRequest(context, h, r);
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
    static Response RestRequest(AsioContext_internal c, Host h, Request const& r);

    static Response RestRequestHTTP(AsioContext_internal c, Host h, Request const&r);

    static Response RestRequestHTTPS(AsioContext_internal c, Host h, Request const& r);

    /*!
     * \brief Retrieve the MIME-type of the REST response, used for strict checking
     * \return String representing the format
     */
    static CString GetContentType(Response const& resp);
};


}

using REST = ASIO::RestClientImpl;

}

#endif
