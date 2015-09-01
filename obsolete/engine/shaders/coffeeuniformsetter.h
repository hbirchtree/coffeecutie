#ifndef COFFEEUNIFORMSETTER_H
#define COFFEEUNIFORMSETTER_H

#include "general/common.h"

class VectorData;
class CoffeeTexture;
class CoffeeShader;

class ScalarValue;
class Vector2Value;
class Vector3Value;
class Matrix4Value;
class CoffeeUniformValue;

class ShaderMapping {
public:
    QString uniform;
    VectorData* data;
    CoffeeUniformValue* value = nullptr;
    bool constant = false;
};
class TextureMapping {
public:
    bool operator==(const TextureMapping& val);
    QPointer<CoffeeTexture> texture;
    QString samplerName;

    bool loaded = false;
};

class CoffeeUniformSetter
{
public:
    void applyUniforms();

    void bindTextures();
    void unbindTextures();

    void setUniform(const QString &uniformName, VectorData* data);

    void setTexture(const QString &samplerName, CoffeeTexture *texture);

    void clearUniforms();
    void clearTextures();

    const QVector<ShaderMapping*> getUniforms() const;
    const QVector<TextureMapping*> getTextures() const;

    virtual void load() = 0;
    virtual void unload() = 0;

    CoffeeShader* _shader_obj();
    void setShader(CoffeeShader* shader);

protected:
    CoffeeUniformSetter();
    QVector<ShaderMapping*> uniforms;
    QVector<TextureMapping*> textures;
    QPointer<CoffeeShader> m_shader;
};

#endif // COFFEEUNIFORMSETTER_H
