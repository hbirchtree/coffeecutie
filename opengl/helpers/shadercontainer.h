#ifndef SHADERCONTAINER_H
#define SHADERCONTAINER_H

#include "general/common.h"
#include "general/filehandler.h"

class ShaderContainer : public QObject
{
    Q_OBJECT
public:
    explicit ShaderContainer(QObject *parent = 0);
    ~ShaderContainer();

    int buildProgram(QString verShaderFile,QString fragShaderFile);
    int compileShader(QString shaderFile, const GLenum &shaderType);
    int getProgramId();
    void unload();

    int getUniformLocation(QString name);
    void getUniformLocations(QList<QString> names);
    int getAttributeLocation(QString name);
    void setUniform(QString name,glm::vec3 val);
    void setUniform(QString name,glm::vec4 val);
    void setUniformRgb(QString name,glm::vec3 val);
    void setUniformRgba(QString name,glm::vec4 val);
    void setUniform(QString name,glm::vec2 val);
    void setUniform(QString name,GLfloat val);
    void setUniform(QString name, int val);

    void setUniform(QString name,glm::mat4 val);

signals:

public slots:

private:
    int programId = 0;
    int verbosity = 0;

    QHash<QString,int> attributes;
    QHash<QString,int> uniforms;
};

#endif // SHADERCONTAINER_H
