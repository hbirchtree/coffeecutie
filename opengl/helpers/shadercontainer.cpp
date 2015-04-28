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
    if(status == GL_FALSE){
        GLint loglen;
        glGetProgramiv(programId,1000,&loglen);
        char* log = new char[loglen+1];
        glGetProgramInfoLog(programId,loglen,NULL,log);
        qDebug() << "Failed to link shader program: \nLog:"+QString(log);
        delete log;
        glDeleteProgram(programId);
        return -1;
    }

    glDetachShader(programId,vertShader);
    glDetachShader(programId,fragShader);

    return 0;
}

int ShaderContainer::compileShader(QString shaderFile,int shaderType){
    int handle = glCreateShader(shaderType);

    std::string src = FileHandler::getStringFromFile(shaderFile).toStdString();
    const char* code = src.c_str();
    glShaderSource(handle,1,&code,NULL);

    glCompileShader(handle);

    GLint status;
    glGetShaderiv(handle,GL_COMPILE_STATUS,&status);
    if(status == GL_FALSE){
        GLint loglen;
        glGetShaderiv(handle,1000,&loglen);
        if(loglen<0){
            qDebug() << "Catastrophic error when compiling shader: Negative log length";
            return -1;
        }
        char* log = new char[loglen+1];
        glGetShaderInfoLog(handle,loglen,NULL,log);
        qDebug() << "Failed to compile shader: "+shaderFile
                    +"\nLog: "+QString::fromLocal8Bit(log);
        delete log;
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
