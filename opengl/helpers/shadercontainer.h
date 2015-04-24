#ifndef SHADERCONTAINER_H
#define SHADERCONTAINER_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QDebug>
#include <GL/glew.h>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/vec2.hpp"
#include "GLFW/glfw3.h"
#include "glext.h"
#include "glcorearb.h"
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

    void getUniformLocation(QString name){
        int handle = glGetUniformLocation(getProgramId(),name.toStdString().c_str());
        if(handle>0)
            uniforms.insert(name,handle);
    }
    void getAttributeLocation(QString name){
        int handle = glGetAttribLocation(getProgramId(),name.toStdString().c_str());
        if(handle>0)
            attributes.insert(name,handle);
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
