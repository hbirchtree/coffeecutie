#include "library.h"

#include <coffee/core/CBase>

#include <stdio.h>

using namespace Coffee;
using namespace DebugFun;

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

COFFAPI TestClass* CoffeeLoader();

TestClass* CoffeeLoader(){
    return new TestImplementation();
}
