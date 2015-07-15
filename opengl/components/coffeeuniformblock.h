#ifndef COFFEEUNIFORMBLOCK_H
#define COFFEEUNIFORMBLOCK_H

#include "general/common.h"
#include <QBuffer>

class Vector3Value;
class Vector2Value;
class CoffeeBuffer;
class VectorData;

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
    CoffeeUniformBlock(QObject *parent,
                       uint64_t bufferSize);
    CoffeeUniformBlock(QObject *parent,
                       CoffeeUniformBlock *source,
                       uint32_t offset,
                       uint32_t size);

    Q_INVOKABLE static uint16_t systemFloatSize();
    Q_INVOKABLE static uint16_t systemVec3Size();
    Q_INVOKABLE static uint16_t systemVec4Size();
    Q_INVOKABLE static uint16_t systemVec2Size();
    Q_INVOKABLE static uint16_t systemMat4Size();
    Q_INVOKABLE static uint16_t systemMat3Size();
    Q_INVOKABLE static uint16_t systemUintSize();

    QString name() const;

    QVector<CoffeeUniformValue*> getUniforms();

    QByteArray* getData();
    void* getDataRange(uint32_t offset, uint32_t size);

    CoffeeUniformBlock* getChild(uint32_t offset, uint32_t size);
    void copyUniforms(CoffeeUniformBlock* src);

    void setUniformData(const QString uniformName, VectorData *uniformData);

signals:
    void dataRangeUpdated(const void *data, uint32_t offset, uint32_t size);

    void dataUpdated();

public slots:
    void setDataRange(const void *data, uint32_t offset, uint32_t size);

    void updateData(uint32_t offset, uint32_t size);

    void addUniform(CoffeeUniformValue* val);
    void setName(const QString &name);

private:
    QByteArray *m_data = nullptr;

    CoffeeBuffer* m_buffer = nullptr;
    QVector<CoffeeUniformValue*> m_uniforms;
    QString m_name;
};

#endif // COFFEEUNIFORMBLOCK_H
