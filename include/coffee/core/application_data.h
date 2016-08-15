#pragma once

#include "types/tdef/stltypes.h"
#include "types/tdef/integertypes.h"

namespace Coffee{

typedef struct application_data_t
{
    static CString organization_name;
    static CString application_name;

    static uint32 version_code;
} ApplicationData_t;

}
