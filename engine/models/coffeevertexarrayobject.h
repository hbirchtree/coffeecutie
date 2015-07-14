#ifndef COFFEEVERTEXARRAYOBJECT_H
#define COFFEEVERTEXARRAYOBJECT_H

#include "general/common.h"

class CoffeeBuffer;

class CoffeeVertexArrayObject : public QObject
{
    Q_OBJECT
public:
    CoffeeVertexArrayObject(QObject *parent);

    GLuint handle();

    void bindArray();
    void unbindArray();

    void freeArray();
    void allocArray();

    bool isValidArray() const;

signals:

public slots:
    void giveHandle(GLuint handle);
    void addAttribute(CoffeeBuffer* buffer,
                      GLuint index,
                      GLenum type,
                      GLboolean normalized,
                      GLuint size,
                      GLuint stride,
                      GLvoid* pointer = 0);
    void addAttributeDivided(CoffeeBuffer* buffer,
                      GLuint index,
                      GLenum type,
                      GLboolean normalized,
                      GLuint size,
                      GLuint stride,
                      GLuint divisor,
                      GLvoid* pointer = 0);

private:
    bool isArrayBound();

    GLuint m_handle = 0;
    QVector<CoffeeBuffer*> m_assocBuffers;
};

#endif // COFFEEVERTEXARRAYOBJECT_H
