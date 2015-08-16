#ifndef COFFEEUNIFORMBLOCK_H
#define COFFEEUNIFORMBLOCK_H

#include "general/common.h"
#include <QBuffer>

class Vector3Value;
class Vector2Value;
class CoffeeBuffer;
class VectorData;
class CoffeeShader;

class CoffeeUniformValue
{
public:
    CoffeeUniformValue(CoffeeShader* parent);

    uint32_t arrayStride = 0;

    int32_t blockOffset = -1;
    int32_t blockIndex = -1;

    uint32_t uniformSize = 0;
    QString uniformName;
    GLenum uniformType;

    uint32_t location = 0;

    void setUniform(const void* data, uint16_t size);
private:
    CoffeeShader* m_shader;
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
