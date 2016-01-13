#include "library.h"

#include <coffee/core/CBase>
#include <plat/cmemory.h>

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
        void* ptr = Coffee::Alloc(1024*1024*1024);
        Coffee::CFree(ptr);

        cDebug("Byte!");
    }
};

extern "C" {

TestClass* CoffeeLoader();

}

TestClass* CoffeeLoader(){
    return new TestImplementation();
}
