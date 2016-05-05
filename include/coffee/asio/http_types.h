#pragma once

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/tdef/stltypes.h>

namespace Coffee{
namespace HTTP{

enum Transport
{
    Trans_Undef,
    Trans_HTTP,
    Trans_HTTPS,
};

using PortNum = uint16;

template<typename StrType>
using ValueMapping = Map<StrType,StrType>;

template<typename StrType>
struct _http_request
{
    uint32 transp;
    PortNum port;

    StrType reqtype;
    StrType resource;

    StrType version;
    StrType mimeType;
    ValueMapping<StrType> values;
    StrType payload;
};

template<typename StrType>
struct _http_response
{
    StrType header; /*!< OBSOLETE: Will be removed soon! */

    StrType reqtype;
    StrType version;
    StrType mimeType;
    StrType message;
    ValueMapping<StrType> values;
    StrType payload;

    union
    {
        uint32 code;
        uint32 status;
    };
};

template<typename T>
void InitializeRequest(_http_request<T>& r)
{
    r.transp = Trans_HTTP;
    r.port = 0;
    r.reqtype = "GET";
    r.version = "HTTP/1.0";
    r.mimeType = "text/plain";
    r.values.insert({"Accept","*/*"});
}

using Request = _http_request<CString>;
using Response = _http_response<CString>;

}
}
