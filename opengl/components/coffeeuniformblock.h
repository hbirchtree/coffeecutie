#ifndef COFFEEUNIFORMBLOCK_H
#define COFFEEUNIFORMBLOCK_H

#include "general/common.h"

class Vector3Value;
class Vector2Value;
class CoffeeBuffer;
class ShaderVariant;

class CoffeeUniformValue
{
public:
    uint16_t arrayStride = 0;

    int16_t blockOffset = -1;
    int16_t blockIndex = -1;

    uint16_t uniformSize = 0;
    QString uniformName;
    GLenum uniformType;
};

class CoffeeUniformBlock : public QObject
{
    Q_PROPERTY(QString name READ name WRITE setName)

    Q_OBJECT
public:
    CoffeeUniformBlock(QObject *parent, uint64_t bufferSize);

    Q_INVOKABLE static uint16_t systemFloatSize();
    Q_INVOKABLE static uint16_t systemVec3Size();
    Q_INVOKABLE static uint16_t systemVec4Size();
    Q_INVOKABLE static uint16_t systemVec2Size();
    Q_INVOKABLE static uint16_t systemMat4Size();
    Q_INVOKABLE static uint16_t systemMat3Size();
    Q_INVOKABLE static uint16_t systemUintSize();

    QString name() const;

signals:

public slots:
    void addUniform(CoffeeUniformValue* val);
    void setName(const QString &name);

private:
    CoffeeBuffer* m_buffer;
    QVector<CoffeeUniformValue*> m_uniforms;
    QString m_name;
};

#endif // COFFEEUNIFORMBLOCK_H
