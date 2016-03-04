#ifndef COFFEE_UNIT_TESTS_MEMORY_OPS_H
#define COFFEE_UNIT_TESTS_MEMORY_OPS_H

#include <coffee/core/CBase>
#include <coffee/core/CTypes>
#include "../plat/plat_memory.h"

namespace CoffeeTests{

using namespace Coffee;

void run_memory_tests(bool silent = true)
{
    constexpr cstring final_string = "test\\\\file\\\\name";
    CString test_string = CStrReplace("test/file/name","/","\\\\");
    if(!silent)
        cDebug("String tests:\n %s ?= %s",test_string,final_string);
    CASSERT(StrCmp(test_string.c_str(),final_string));
}

}

#endif
