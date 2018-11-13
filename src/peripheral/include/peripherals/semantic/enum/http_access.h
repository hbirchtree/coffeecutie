#pragma once

#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>

#undef None
#undef DELETE

namespace semantic {

enum class HTTPAccess
{
    None = 0x0,

    Secure = 0x1, /* Whether to use SSL */

    GET    = 0x2, /* Use GET request */
    POST   = 0x4, /* Use POST request */
    PUT    = 0x8, /* Use PUT request */
    UPDATE = 0x40,
    DELETE = 0x80,
    PATCH  = 0x100,
    HEAD   = 0x200,

    NoRedirect = 0x10, /*!< Do not follow redirect locations */
    NoVerify   = 0x20, /*!< When using SSL, do not verify certificates */

    DefaultAccess = GET | Secure,
    DefaultPOST   = POST | Secure,

    SecurityMask = Secure,
    RequestMask  = GET | POST | PUT | UPDATE | DELETE | PATCH | HEAD,
};
C_FLAGS(HTTPAccess, libc_types::u32);

} // namespace semantic
