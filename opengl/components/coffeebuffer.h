#ifndef COFFEEBUFFER_H
#define COFFEEBUFFER_H

#include "general/common.h"

//Wrapping for OpenGL buffers, will tell us all about size and etc.

class CoffeeBuffer : public QObject
{
    Q_PROPERTY(uint size READ size WRITE setSize NOTIFY sizeChanged)

    Q_OBJECT
public:
    CoffeeBuffer(QObject *parent);

    uint size() const;

signals:
    void sizeChanged(uint size);

public slots:
    void allocBuffer();
    void setSize(uint size);

private:
    QVector<char> m_bufferData; //Will be kept here, not handed to driver
    GLuint m_handle;
};

#endif // COFFEEBUFFER_H
