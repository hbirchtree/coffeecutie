#include "library.h"

#include <coffee/core/CBase>
#include <coffee/core/plat/cmemory.h>

#include <stdio.h>

using namespace Coffee;
using namespace CDebugPrint;

class TestImplementation : public TestClass
{
public:
    void printHello()
    {
        cDebug("Hello there!");

        cWarning("Okay, I'll try calling a Coffee function now.");
        void* ptr = Coffee::c_alloc(1024*1024*1024);
        Coffee::c_free(ptr);

        cDebug("Byte!");
    }
};

extern "C" {

TestClass* CoffeeLoader();

}

TestClass* CoffeeLoader(){
    return new TestImplementation();
}
