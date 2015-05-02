#ifndef FLOATBUFFER_H
#define FLOATBUFFER_H

#include "general/common.h"
#include "opengl/rendering/coffeevertex.h"
class FloatBuffer
{
public:
    static FloatBuffer* createArray(int size);
    FloatBuffer* allocate(int size);
    FloatBuffer* put(GLfloat* input);
    FloatBuffer* put(GLfloat in);
    void reset();
    GLfloat* get();
    int getSize();
    QString toString();

private:
    FloatBuffer(int size);
    int pointer = 0;
    std::vector<GLfloat> data;
};

#endif // FLOATBUFFER_H
