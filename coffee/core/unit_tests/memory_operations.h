#ifndef COFFEE_UNIT_TESTS_MEMORY_OPS_H
#define COFFEE_UNIT_TESTS_MEMORY_OPS_H

#include "coffee/core/CBase"
#include "coffee/core/CTypes"
#include "coffee/core/plat/cmemory.h"

namespace CoffeeTests{

using namespace Coffee;

void run_memory_tests(bool silent = true)
{
    constexpr cstring final_string = "test\\\\file\\\\name";
    cstring_w test_string = c_str_replace("test/file/name","/","\\\\");
    if(!silent)
        cDebug("String tests:\n %s ?= %s",test_string,final_string);
    CASSERT(c_strcmp(test_string,final_string));
    c_free(test_string);
}

}

#endif
