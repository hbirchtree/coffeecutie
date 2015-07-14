#include "coffeevertexarrayobject.h"

CoffeeVertexArrayObject::CoffeeVertexArrayObject(QObject *parent) : QObject(parent)
{

}

GLuint CoffeeVertexArrayObject::handle()
{
    return m_handle;
}

void CoffeeVertexArrayObject::bindArray()
{
    if(isValidArray()){
        glBindVertexArray(m_handle);
    }else{
        qWarning("Cannot bind invalid array!");
    }
}

void CoffeeVertexArrayObject::unbindArray()
{
    glBindVertexArray(0);
}

void CoffeeVertexArrayObject::freeArray()
{
    if(isValidArray())
        glDeleteVertexArrays(1,&m_handle);
}

void CoffeeVertexArrayObject::allocArray()
{
    glGenVertexArrays(1,&m_handle);
}

bool CoffeeVertexArrayObject::isValidArray() const
{
    return glIsVertexArray(m_handle)==GL_TRUE||
            m_handle!=0;
}

void CoffeeVertexArrayObject::giveHandle(GLuint handle)
{
    if(glIsVertexArray(handle)==GL_TRUE){
        m_handle = handle;
    }else{
        qWarning("Given vertex array handle was invalid!");
    }
}

void CoffeeVertexArrayObject::addAttribute(CoffeeBuffer *buffer, GLuint index, GLenum type, GLboolean normalized, GLuint size, GLuint stride, GLvoid *pointer)
{
    if(!isArrayBound())
        bindArray();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index,size,type,normalized,stride,pointer);
    if(buffer&&!m_assocBuffers.contains(buffer)){ //We might find a use for this later
        m_assocBuffers.append(buffer);
    }
}

void CoffeeVertexArrayObject::addAttributeDivided(CoffeeBuffer *buffer, GLuint index, GLenum type, GLboolean normalized, GLuint size, GLuint stride, GLuint divisor, GLvoid *pointer)
{
    addAttribute(buffer,index,type,normalized,size,stride,pointer);
    glVertexAttribDivisor(index,divisor);
}

bool CoffeeVertexArrayObject::isArrayBound()
{
    GLint bound = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING,&bound);
    return bound==m_handle&&isValidArray();
}
