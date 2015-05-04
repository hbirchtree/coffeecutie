#include "shadercontainer.h"

ShaderContainer::ShaderContainer(QObject *parent) : QObject(parent)
{

}

ShaderContainer::~ShaderContainer()
{

}

int ShaderContainer::buildProgram(QString vertShaderFile,QString fragShaderFile){
    programId = glCreateProgram();

    int vertShader = compileShader(vertShaderFile,GL_VERTEX_SHADER);
    int fragShader = compileShader(fragShaderFile,GL_FRAGMENT_SHADER);

    if(vertShader==-1||fragShader==-1)
        return -1;

    GLint status;

    glAttachShader(programId,vertShader);
    glAttachShader(programId,fragShader);

    glLinkProgram(programId);

    glGetProgramiv(programId,GL_LINK_STATUS,&status);
    if(status == 0&&verbosity>0){
        GLint loglen;
        GLchar log[1000];
        glGetProgramInfoLog(programId,sizeof(log),&loglen,log);
        qDebug() << "Failed to link shader program: \nLog:"+QString(log);
        glDeleteProgram(programId);
        return -1;
    }

    glDetachShader(programId,vertShader);
    glDetachShader(programId,fragShader);

    return 0;
}

int ShaderContainer::compileShader(QString shaderFile, const GLenum &shaderType){
    int handle = glCreateShader(shaderType);

    std::string src = FileHandler::getStringFromFile(shaderFile).toStdString();
    const char* code = src.c_str();
    glShaderSource(handle,1,&code,NULL);

    glCompileShader(handle);

    GLint status;
    glGetShaderiv(handle,GL_COMPILE_STATUS,&status);
    if(status == 0&&verbosity>0){
        GLint loglen;
        GLchar log[1000];
        glGetShaderInfoLog(handle,sizeof(log),&loglen,log);
        qDebug() << "Failed to compile shader: "+shaderFile
                    +"\nLog: "+QString::fromLocal8Bit(log);
        glDeleteShader(handle);
        return -1;
    }
    return handle;
}

int ShaderContainer::getProgramId()
{
    return programId;
}

void ShaderContainer::unload()
{
    glDeleteProgram(programId);
    programId = 0;
}

int ShaderContainer::getUniformLocation(QString name){
    if(uniforms.contains(name))
        return uniforms.value(name);
    int handle = glGetUniformLocation(getProgramId(),name.toStdString().c_str());
    if(handle>=0)
        uniforms.insert(name,handle);
    else if(verbosity>1)
        qDebug() << this->objectName() << "Failed to get uniform: " << name;
    return handle;
}

void ShaderContainer::getUniformLocations(QList<QString> names){
    for(QString name : names)
        getUniformLocation(name);
}

int ShaderContainer::getAttributeLocation(QString name){
    if(attributes.contains(name))
        return attributes.value(name);
    int handle = glGetAttribLocation(getProgramId(),name.toStdString().c_str());
    if(handle>=0)
        attributes.insert(name,handle);
    else if(verbosity>1){
        qDebug() << this->objectName() << "Failed to get attribute: " << name;
        return -1;
    }
    return handle;
}

void ShaderContainer::setUniform(QString name, glm::vec3 val){
    if(uniforms.keys().contains(name))
        glUniform3f(uniforms.value(name),val.x,val.y,val.z);
    else if(verbosity>1)
        qDebug() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, glm::vec4 val){
    if(uniforms.keys().contains(name))
        glUniform4f(uniforms.value(name),val.x,val.y,val.z,val.w);
    else if(verbosity>1)
        qDebug() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniformRgb(QString name, glm::vec3 val){
    if(uniforms.keys().contains(name))
        glUniform3f(uniforms.value(name),val.r,val.g,val.b);
    else if(verbosity>1)
        qDebug() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniformRgba(QString name, glm::vec4 val){
    if(uniforms.keys().contains(name))
        glUniform4f(uniforms.value(name),val.r,val.g,val.b,val.a);
    else if(verbosity>1)
        qDebug() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, glm::vec2 val){
    if(uniforms.keys().contains(name))
        glUniform2f(uniforms.value(name),val.x,val.y);
    else if(verbosity>1)
        qDebug() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, GLfloat val){
    if(uniforms.keys().contains(name))
        glUniform1f(uniforms.value(name),val);
    else if(verbosity>1)
        qDebug() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, int val){
    if(uniforms.keys().contains(name))
        glUniform1i(uniforms.value(name),val);
    else if(verbosity>1)
        qDebug() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, glm::mat4 val){
    if(uniforms.keys().contains(name))
        glUniformMatrix4fv(uniforms.value(name),1,GL_FALSE,glm::value_ptr(val));
    else if(verbosity>1)
        qDebug() << this->objectName() << "Failed to set uniform: " << name;
}
