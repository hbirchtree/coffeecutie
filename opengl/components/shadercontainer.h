#ifndef SHADERCONTAINER_H
#define SHADERCONTAINER_H

#include "general/common.h"
#include "general/filehandler.h"
#include "general/shadervariant.h"
class ShaderContainer : public QObject
{
    Q_PROPERTY(QString fragmentShader READ fragmentShader WRITE setFragmentShader)
    Q_PROPERTY(QString vertexShader READ vertexShader WRITE setVertexShader)
    Q_PROPERTY(QString geometryShader READ geometryShader WRITE setGeometryShader)
    Q_PROPERTY(int programId READ getProgramId)

    Q_OBJECT
public:
    explicit ShaderContainer(QObject *parent = 0);
    ~ShaderContainer();

    //Full-process functions
    int buildProgram(QString vertShaderFile, QString fragShaderFile, QString geomShaderFile);
    int buildProgram(QString vertShaderFile,QString fragShaderFile);
    int buildProgram();

    //Partial
    void createProgram();
    bool addShader(const char* data, QString id, const GLenum& shaderType);
    bool linkProgram();

    int compileShader(QString shaderFile, const GLenum &shaderType);
    int compileShaderSource(const char* data, QString id, const GLenum& shaderType);
    int getProgramId();
    void unload();

    int getUniformLocation(QString name);
    void getUniformLocations(QList<QString> names);
    int getAttributeLocation(QString name);
    void setUniform(QString name, const glm::vec3 &val);
    void setUniform(QString name,const glm::vec4& val);
    void setUniformRgb(QString name,const glm::vec3& val);
    void setUniformRgba(QString name,const glm::vec4& val);
    void setUniform(QString name,const glm::vec2& val);
    void setUniform(QString name,GLfloat val);
    void setUniform(QString name,int val);

    void setUniform(QString name, const glm::mat3 &val);
    void setUniform(QString name, const glm::mat4 &val);

    void setUniform(QString name, const ShaderVariant *val);

    QString fragmentShader() const;
    QString vertexShader() const;
    QString geometryShader() const;

public slots:
    void setFragmentShader(const QString& sh);
    void setVertexShader(const QString& sh);
    void setGeometryShader(const QString &geometryShader);

private:
    QString fragShaderFile;
    QString vertShaderFile;
    QString geomShaderFile;

    QVector<int> shaders;

    int programId = 0;
    int verbosity = 1;

    QHash<QString,int> attributes;
    QHash<QString,int> uniforms;
    QString m_geometryShader;
};

#endif // SHADERCONTAINER_H
