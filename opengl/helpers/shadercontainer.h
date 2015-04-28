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
    int compileShader(QString shaderFile,int shaderType);
    int getProgramId();
    void unload();

    int getUniformLocation(QString name){
        int handle = glGetUniformLocation(getProgramId(),name.toStdString().c_str());
        if(handle>0)
            uniforms.insert(name,handle);
        return handle;
    }
    int getAttributeLocation(QString name){
        int handle = glGetAttribLocation(getProgramId(),name.toStdString().c_str());
        if(handle>0)
            attributes.insert(name,handle);
        return handle;
    }
    void setUniform(QString name,glm::vec3 val){
        if(uniforms.keys().contains(name))
            glUniform3f(uniforms.value(name),val.x,val.y,val.z);
        else
            throw std::runtime_error("failed to set uniform: "+name.toStdString());
    }
    void setUniform(QString name,glm::vec2 val){
        if(uniforms.keys().contains(name))
            glUniform2f(uniforms.value(name),val.x,val.y);
        else
            throw std::runtime_error("failed to set uniform: "+name.toStdString());
    }
    void setUniform(QString name,GLfloat val){
        if(uniforms.keys().contains(name))
            glUniform1f(uniforms.value(name),val);
        else
            throw std::runtime_error("failed to set uniform: "+name.toStdString());
    }

signals:

public slots:

private:
    int programId = 0;

    QHash<QString,int> attributes;
    QHash<QString,int> uniforms;
};

#endif // SHADERCONTAINER_H
