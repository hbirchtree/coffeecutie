#ifndef SHADERCONTAINER_H
#define SHADERCONTAINER_H

#include "general/common.h"
#include "general/data/coffeegameasset.h"

class ShaderVariant;
class VectorData;
class CoffeeResource;

class CoffeeUniformValue;
class CoffeeUniformBlock;

class CoffeeShader : public QObject, public CoffeeGameAsset
{
    Q_PROPERTY(QString fragmentShader READ fragmentShader)
    Q_PROPERTY(QString vertexShader READ vertexShader)
    Q_PROPERTY(QString geometryShader READ geometryShader)
    Q_PROPERTY(QVariantMap uniforms READ getUniformsMap)
    Q_PROPERTY(QVariantMap attributes READ getAttributesMap)
    Q_PROPERTY(uint verbosity READ getVerbosity WRITE setVerbosity)
    Q_PROPERTY(int programId READ getProgramId)

    Q_OBJECT
public:
    CoffeeShader(QObject *parent);
    ~CoffeeShader();

    //Full-process functions
    Q_INVOKABLE bool buildProgram(CoffeeResource *vertShaderFile, CoffeeResource *fragShaderFile, CoffeeResource *geomShaderFile);
    Q_INVOKABLE bool buildProgram(CoffeeResource *vertShaderFile, CoffeeResource *fragShaderFile);
    Q_INVOKABLE bool buildProgram();

    void compileShaders();

    //Partial
    void createProgram();
    bool addShader(const char* data, const QString &id, const GLenum& shaderType);
    bool linkProgram();

    GLuint compileShader(CoffeeResource *shader, const GLenum &shaderType);
    GLuint compileShaderSource(const char* data, const QString &id, const GLenum& shaderType);
    int getProgramId();

    //We keep these available if a user wants to work with raw gl* operations
    int getAttributeLocation(const QString &name);
    int getUniformLocation(const QString &name);

    const QHash<QString,GLenum> getAttributes();
    const QHash<QString,GLenum> getUniforms();

    QVariantMap getUniformsMap();
    QVariantMap getAttributesMap();

    QString fragmentShader() const;
    QString vertexShader() const;
    QString geometryShader() const;

    void setFragmentShader(CoffeeResource *value);
    void setVertexShader(CoffeeResource *value);
    void setGeometryShader(CoffeeResource *geometryShader);

    uint getVerbosity() const;

    Q_INVOKABLE CoffeeUniformBlock* getUniformBlock(const QString &name);
    Q_INVOKABLE CoffeeUniformValue* getUniformValue(const QString &name);

    void setUniform(const QString &name, const glm::vec3 &val);
    void setUniform(const QString &name, const glm::vec4& val);
    void setUniform(const QString &name, const glm::vec2& val);
    void setUniform(const QString &name, GLfloat val);
    void setUniform(const QString &name, int val);

    void setUniform(const QString &name, const glm::mat3 &val);
    void setUniform(const QString &name, const glm::mat4 &val);

    void setUniform(const QString &name, const ShaderVariant *val);
    void setUniform(const QString &name, VectorData *val);

    void bindUniformBufferRange(GLuint uboIndex, GLuint uboHandle, uint32_t offset, uint32_t size);

public slots:
    void load();
    void unload();

    void setVerbosity(uint verbosity);

private:
    void getProgramAttributes();
    void getProgramUniforms();
    CoffeeUniformValue* getProgramUniform(GLuint index);
    CoffeeUniformBlock* getProgramUniformBlock(GLuint index);

    CoffeeResource *fragShaderFile = nullptr;
    CoffeeResource *vertShaderFile = nullptr;
    CoffeeResource *m_geometryShader = nullptr;

    QVector<GLuint> shaders;

    GLuint programId = 0;
    uint verbosity = 1;

    QHash<QString,int> uniformBlocks;
    QHash<QString,int> attributes;
    QHash<QString,int> uniforms;
    QHash<QString,GLenum> attributes_t;
    QHash<QString,GLenum> uniforms_t;

    QVector<CoffeeUniformValue*> m_uniformValues;
    QVector<CoffeeUniformBlock*> m_uniformBlocks;
};

#endif // SHADERCONTAINER_H
