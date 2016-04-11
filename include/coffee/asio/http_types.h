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

template<typename CharType>
using ValueMapping = Map<CharType,CharType>;

template<typename CharType>
struct _http_request
{
    CharType version;
    ValueMapping<CharType> values;
    CharType payload;
};

template<typename CharType>
struct _http_response
{
    CharType header; /*!< OBSOLETE: Will be removed soon! */

    CharType version;
    CharType message;
    ValueMapping<CharType> values;
    CharType payload;

    union
    {
        uint32 code;
        uint32 status;
    };
};

using Request = _http_request<CString>;
using Response = _http_response<CString>;

}
}
