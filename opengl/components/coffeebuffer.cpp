#include "coffeebuffer.h"

CoffeeBuffer::CoffeeBuffer(QObject *parent) : QObject(parent)
{

}

uint CoffeeBuffer::size() const
{
    return m_bufferData.size();
}

void CoffeeBuffer::allocBuffer()
{

}

void CoffeeBuffer::setSize(uint size)
{
    if ((uint)m_bufferData.size() == size)
        return;

    m_bufferData.resize(size);
    emit sizeChanged(size);
}

