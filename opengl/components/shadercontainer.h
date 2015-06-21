#ifndef SHADERCONTAINER_H
#define SHADERCONTAINER_H

#include "general/common.h"
#include "general/data/coffeegameasset.h"

class ShaderVariant;

class ShaderContainer : public QObject, public CoffeeGameAsset
{
    Q_PROPERTY(QString fragmentShader READ fragmentShader WRITE setFragmentShader)
    Q_PROPERTY(QString vertexShader READ vertexShader WRITE setVertexShader)
    Q_PROPERTY(QString geometryShader READ geometryShader WRITE setGeometryShader)
    Q_PROPERTY(QVariantMap uniforms READ getUniformsMap)
    Q_PROPERTY(QVariantMap attributes READ getAttributesMap)
    Q_PROPERTY(int programId READ getProgramId)

    Q_OBJECT
public:
    ShaderContainer(QObject *parent);
    ~ShaderContainer();

    //Full-process functions
    Q_INVOKABLE bool buildProgram(QString vertShaderFile, QString fragShaderFile, QString geomShaderFile);
    Q_INVOKABLE bool buildProgram(QString vertShaderFile,QString fragShaderFile);
    Q_INVOKABLE bool buildProgram();

    void compileShaders();

    //Partial
    void createProgram();
    bool addShader(const char* data, QString id, const GLenum& shaderType);
    bool linkProgram();

    GLuint compileShader(QString shaderFile, const GLenum &shaderType);
    GLuint compileShaderSource(const char* data, QString id, const GLenum& shaderType);
    int getProgramId();

    //We keep these available if a user wants to work with raw gl* operations
    int getAttributeLocation(QString name);
    int getUniformLocation(QString name);

    const QHash<QString,GLenum> getAttributes();
    const QHash<QString,GLenum> getUniforms();

    QVariantMap getUniformsMap();
    QVariantMap getAttributesMap();

    QString fragmentShader() const;
    QString vertexShader() const;
    QString geometryShader() const;

public slots:
    void setFragmentShader(const QString& sh);
    void setVertexShader(const QString& sh);
    void setGeometryShader(const QString &geometryShader);

    void setUniform(QString name, const glm::vec3 &val);
    void setUniform(QString name,const glm::vec4& val);
    void setUniform(QString name,const glm::vec2& val);
    void setUniform(QString name,GLfloat val);
    void setUniform(QString name,int val);

    void setUniform(QString name, const glm::mat3 &val);
    void setUniform(QString name, const glm::mat4 &val);

    void setUniform(QString name, const ShaderVariant *val);

    void unload();

private:

    QString fragShaderFile;
    QString vertShaderFile;

    QVector<GLuint> shaders;

    int programId = 0;
    int verbosity = 1;

    QHash<QString,int> attributes;
    QHash<QString,int> uniforms;
    QHash<QString,GLenum> attributes_t;
    QHash<QString,GLenum> uniforms_t;
    QString m_geometryShader;
};

#endif // SHADERCONTAINER_H
