#ifndef EXAMPLE_CODE_EXAMPLE_LIBRARY_FILE_H
#define EXAMPLE_CODE_EXAMPLE_LIBRARY_FILE_H

class TestClass
{
public:
    virtual ~TestClass()
    {
    }

    virtual void printHello() = 0;
};

#endif
