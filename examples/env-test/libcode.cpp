#include "library.h"
#include <stdio.h>

class TestImplementation : public TestClass
{
public:
    void printHello()
    {
        printf("Hello!\n");
    }
};

extern "C" {

TestClass* CoffeeLoader();

}

TestClass* CoffeeLoader(){
    return new TestImplementation();
}
