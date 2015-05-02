#include "floatbuffer.h"
#include <vector>

FloatBuffer::FloatBuffer(int size){
    data.reserve(size);
}

FloatBuffer* FloatBuffer::createArray(int size){
    return (new FloatBuffer(size));
}

FloatBuffer* FloatBuffer::put(GLfloat *input)
{
    int insize = sizeof(input)/sizeof(GLfloat);
    if(insize+pointer>=getSize())
        throw std::runtime_error("err: glfloat buffer overflow");

    for(int i=0;i<insize;i++)
        put(input[i]);

    return this;
}

FloatBuffer* FloatBuffer::put(GLfloat in)
{
    if(pointer>=getSize())
        throw std::runtime_error("err: glfloat buffer overflow");
    data.insert(data.begin()+pointer,in);
    pointer++;

    return this;
}

void FloatBuffer::reset()
{
    pointer = 0;
}

GLfloat* FloatBuffer::get()
{
    return data.data();
}

int FloatBuffer::getSize()
{
    return data.capacity();
}

QString FloatBuffer::toString()
{
    return CoffeeVertex::floatBufferToString(data.data(),0,getSize());
}
