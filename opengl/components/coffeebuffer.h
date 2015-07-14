#ifndef COFFEEBUFFER_H
#define COFFEEBUFFER_H

#include "general/common.h"

//Wrapping for OpenGL buffers, will tell us all about size and etc.

class CoffeeBuffer : public QObject
{
    Q_PROPERTY(uint size READ size WRITE setSize NOTIFY sizeChanged)

    Q_OBJECT
public:
    CoffeeBuffer(QObject *parent, BufferStorageMask flags, GLenum bufferType);
    CoffeeBuffer(QObject *parent, BufferStorageMask flags, GLenum bufferType, GLuint handle);

    uint size() const;

    GLuint handle() const;

    void applyBufferStorage(); //Allows us to update the storage flags, clears the buffer

    void allocBuffer();
    void freeBuffer();

    void bindBuffer();
    void unbindBuffer();

    void commitData(const void *data, GLsizeiptr size);
    void commitSubData(const void* data, GLintptr offset, GLsizeiptr size);

    QVector<quint8> getSubData(GLintptr offset, GLsizeiptr size);

signals:
    void sizeChanged(uint size);

public slots:
    void giveHandle(GLuint handle);
    void setSize(uint size);

private:
    bool isValidBuffer() const;
    bool isBufferBound() const;

    QVector<quint8> m_bufferData; //Our local storage, might be used by OpenGL if flags allow it
    GLuint m_handle = 0;
    GLenum m_bufferType;
    BufferStorageMask m_dataFlags;
};

#endif // COFFEEBUFFER_H
