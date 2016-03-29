#pragma once

#include "types/tdef/stltypes.h"
#include "types/tdef/integertypes.h"

namespace Coffee{

typedef struct application_data_t
{

    CString organization_name;
    CString application_name;

    uint32 version_code;

} ApplicationData_t;

}