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

    glAttachShader(programId,vertShader);
    glAttachShader(programId,fragShader);

    glLinkProgram(programId);

    glDetachShader(programId,vertShader);
    glDetachShader(programId,fragShader);

    GLint status;
    glGetProgramiv(programId,GL_LINK_STATUS,&status);
    if(status == GL_FALSE){
        GLint loglen;
        glGetProgramiv(programId,1000,&loglen);
        char* log = new char[loglen+1];
        glGetProgramInfoLog(programId,loglen,NULL,log);
        qDebug() << "Failed to compile shader program: \nLog:"+QString(log);
        delete log;
        glDeleteProgram(programId);
        return -1;
    }
    return 0;
}

int ShaderContainer::compileShader(QString shaderFile,int shaderType){
    int handle = glCreateShader(shaderType);

    const char* code = FileHandler::getStringFromFile(shaderFile).toStdString().c_str();
    glShaderSource(handle,1,(const GLchar**)&code,NULL);

    glCompileShader(handle);

    GLint status;
    glGetShaderiv(handle,GL_COMPILE_STATUS,&status);
    if(status == GL_FALSE){
        GLint loglen;
        glGetShaderiv(handle,1000,&loglen);
        char* log = new char[loglen+1];
        glGetShaderInfoLog(handle,loglen,NULL,log);
        qDebug() << "Failed to create shader: "+shaderFile
                    +"\nLog:"+log;
        delete log;
        glDeleteShader(handle);
        return -1;
    }
    return handle;
}
