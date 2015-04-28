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
        if(handle>=0)
            uniforms.insert(name,handle);
        return handle;
    }
    void getUniformLocations(QList<QString> names){
        for(QString name : names)
            getUniformLocation(name);
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
        else if(verbosity>1)
            qDebug() << this->objectName() << "Failed to set uniform: " << name;
    }
    void setUniform(QString name,glm::vec4 val){
        if(uniforms.keys().contains(name))
            glUniform4f(uniforms.value(name),val.x,val.y,val.z,val.w);
        else if(verbosity>1)
            qDebug() << this->objectName() << "Failed to set uniform: " << name;
    }
    void setUniformRgb(QString name,glm::vec3 val){
        if(uniforms.keys().contains(name))
            glUniform3f(uniforms.value(name),val.r,val.g,val.b);
        else if(verbosity>1)
            qDebug() << this->objectName() << "Failed to set uniform: " << name;
    }
    void setUniformRgba(QString name,glm::vec4 val){
        if(uniforms.keys().contains(name))
            glUniform4f(uniforms.value(name),val.r,val.g,val.b,val.a);
        else if(verbosity>1)
            qDebug() << this->objectName() << "Failed to set uniform: " << name;
    }
    void setUniform(QString name,glm::vec2 val){
        if(uniforms.keys().contains(name))
            glUniform2f(uniforms.value(name),val.x,val.y);
        else if(verbosity>1)
            qDebug() << this->objectName() << "Failed to set uniform: " << name;
    }
    void setUniform(QString name,GLfloat val){
        if(uniforms.keys().contains(name))
            glUniform1f(uniforms.value(name),val);
        else if(verbosity>1)
            qDebug() << this->objectName() << "Failed to set uniform: " << name;
    }
    void setUniform(QString name, int val){
        if(uniforms.keys().contains(name))
            glUniform1i(uniforms.value(name),val);
        else if(verbosity>1)
            qDebug() << this->objectName() << "Failed to set uniform: " << name;
    }

    void setUniform(QString name,glm::mat4 val){
        if(uniforms.keys().contains(name))
            glUniformMatrix4fv(uniforms.value(name),1,GL_FALSE,glm::value_ptr(val));
        else if(verbosity>1)
            qDebug() << this->objectName() << "Failed to set uniform: " << name;
    }

signals:

public slots:

private:
    int programId = 0;
    int verbosity = 0;

    QHash<QString,int> attributes;
    QHash<QString,int> uniforms;
};

#endif // SHADERCONTAINER_H
