#include "coffeebuffer.h"

CoffeeBuffer::CoffeeBuffer(QObject *parent, BufferStorageMask flags, GLenum bufferType) : QObject(parent)
{
    m_bufferType = bufferType;
    m_dataFlags = flags;
}

CoffeeBuffer::CoffeeBuffer(QObject *parent, BufferStorageMask flags, GLenum bufferType, GLuint handle) :
    CoffeeBuffer(parent,flags,bufferType)
{
    giveHandle(handle);
}

uint CoffeeBuffer::size() const
{
    return m_bufferData.size();
}

GLuint CoffeeBuffer::handle() const
{
    return m_handle;
}

void CoffeeBuffer::applyBufferStorage()
{
    bindBuffer();
    if(m_dataFlags==GL_DYNAMIC_STORAGE_BIT)
        qDebug() << "Doh!";
    glBufferStorage(m_bufferType,0,nullptr,m_dataFlags);
    unbindBuffer();
}

void CoffeeBuffer::allocBuffer()
{
    if(m_handle!=0)
        return;
    glGenBuffers(1,&m_handle);
    if(!isValidBuffer()){
        qWarning("Oh no! Our buffer is bad!");
        m_handle = 0;
    }
}

void CoffeeBuffer::freeBuffer()
{
    glDeleteBuffers(1,&m_handle);
    m_handle = 0;
}

void CoffeeBuffer::bindBuffer()
{
    if(!isValidBuffer()){
        qWarning("Invalid buffer! Unable to bind buffer");
    }else
        glBindBuffer(m_bufferType,m_handle);
}

void CoffeeBuffer::unbindBuffer()
{
    if(!isValidBuffer()){
        qWarning("Invalid buffer! Unable to bind buffer");
    }else
        glBindBuffer(m_bufferType,0);
}

void CoffeeBuffer::commitData(const void* data, GLsizeiptr size)
{
    bindBuffer();
    glBufferStorage(m_bufferType,size,data,m_dataFlags);
}

void CoffeeBuffer::commitSubData(const void *data, GLintptr offset, GLsizeiptr size)
{
    bindBuffer();
    glBufferSubData(m_bufferType,offset,size,data);
}

QVector<quint8> CoffeeBuffer::getSubData(GLintptr offset, GLsizeiptr size)
{
    QVector<unsigned char> d_out;
    d_out.resize(size);

    bindBuffer();
    glGetBufferSubData(m_bufferType,offset,size,d_out.data());
    unbindBuffer();

    return d_out;
}

void CoffeeBuffer::giveHandle(GLuint handle)
{
    m_handle = handle;
    if(!isValidBuffer()){
        qWarning("Oh no! Our buffer is bad!");
    }
}

void CoffeeBuffer::setSize(uint size)
{
    if((uint)m_bufferData.size() == size)
        return;

    m_bufferData.resize(size);
    emit sizeChanged(size);
}

bool CoffeeBuffer::isValidBuffer() const
{
    return (m_handle!=0);
}

bool CoffeeBuffer::isBufferBound() const
{
    GLint bound = 0;
    glGetIntegerv(GL_BUFFER_BINDING,&bound);
    qDebug() << m_handle << bound;
    return m_handle==(GLuint)bound&&isValidBuffer();
}

