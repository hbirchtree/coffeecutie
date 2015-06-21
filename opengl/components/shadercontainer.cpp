#include "shadercontainer.h"

#include "general/filehandler.h"
#include "general/shadervariant.h"

ShaderContainer::ShaderContainer(QObject *parent) : QObject(parent)
{
}

ShaderContainer::~ShaderContainer()
{

}

bool ShaderContainer::buildProgram(QString vertShaderFile, QString fragShaderFile, QString geomShaderFile)
{
    if(isAllocated()){
        addAllocation();
        return true;
    }

    createProgram();

    setVertexShader(vertShaderFile);
    setFragmentShader(fragShaderFile);
    setGeometryShader(geomShaderFile);

    compileShaders();

    if(!linkProgram())
        return false;

    return true;
}

bool ShaderContainer::buildProgram(QString vertShaderFile,QString fragShaderFile){
    return buildProgram(vertShaderFile,fragShaderFile,QString());
}

bool ShaderContainer::buildProgram()
{
    return buildProgram(vertexShader(),fragmentShader(),geometryShader());
}

void ShaderContainer::compileShaders()
{
    std::string src = FileHandler::getStringFromFile(vertexShader()).toStdString();
    const char* code = src.c_str();
    addShader(code,vertexShader(),GL_VERTEX_SHADER);

    if(!fragmentShader().isEmpty()){
        src = FileHandler::getStringFromFile(fragmentShader()).toStdString();
        code = src.c_str();
        addShader(code,fragmentShader(),GL_FRAGMENT_SHADER);
    }

    if(!geometryShader().isEmpty()){
        src = FileHandler::getStringFromFile(geometryShader()).toStdString();
        code = src.c_str();
        addShader(code,geometryShader(),GL_GEOMETRY_SHADER);
    }
}

void ShaderContainer::createProgram()
{
    programId = glCreateProgram();
}

bool ShaderContainer::addShader(const char *data, QString id, const GLenum &shaderType)
{
    GLuint shader = compileShaderSource(data,id,shaderType);
    if(shader>0)
        shaders.append(shader);
    else
        return false;
    return true;
}

bool ShaderContainer::linkProgram()
{
    for(GLuint shader : shaders)
        glAttachShader(programId,shader);

    glLinkProgram(programId);

    GLint status;
    glGetProgramiv(programId,GL_LINK_STATUS,&status);
    if(status == 0&&verbosity>0){
        GLint loglen;
        GLchar log[1000];
        glGetProgramInfoLog(programId,sizeof(log),&loglen,log);
        qDebug() << "Failed to link shader program: \nLog:"+QString(log);
        glDeleteProgram(programId);
        return false;
    }

    for(int shader : shaders){
        glDetachShader(programId,shader);
        glDeleteShader(shader);
    }

    GLint c;
    glGetProgramiv(programId,GL_ACTIVE_UNIFORMS,&c);
    qDebug() << "Shader info:" << c << "uniforms";
    for(int i=0;i<c;i++){
        GLchar o[64];
        int sz;
        GLenum t;
        glGetActiveUniform(programId,i,64,nullptr,&sz,&t,o);
        int handle = glGetUniformLocation(programId,o);
        uniforms.insert(QString(o),handle);
        uniforms_t.insert(QString(o),t);
    }
    glGetProgramiv(programId,GL_ACTIVE_ATTRIBUTES,&c);
    for(int i=0;i<c;i++){
        GLchar o[64];
        int sz;
        GLenum t;
        glGetActiveAttrib(programId,i,64,nullptr,&sz,&t,o);
        int handle = glGetAttribLocation(programId,o);
        attributes.insert(QString(o),handle);
        attributes_t.insert(QString(o),t);
    }

    addAllocation();
    return true;
}

GLuint ShaderContainer::compileShader(QString shaderFile, const GLenum &shaderType){
    std::string src = FileHandler::getStringFromFile(shaderFile).toStdString();
    const char* code = src.c_str();
    return compileShaderSource(code,shaderFile,shaderType);
}

GLuint ShaderContainer::compileShaderSource(const char *data, QString id, const GLenum &shaderType)
{
    GLuint handle = glCreateShader(shaderType);

    glShaderSource(handle,1,&data,NULL);

    glCompileShader(handle);

    GLint status;
    glGetShaderiv(handle,GL_COMPILE_STATUS,&status);
    if(status == 0&&verbosity>0){
        GLint loglen;
        GLchar log[1000];
        glGetShaderInfoLog(handle,sizeof(log),&loglen,log);
        qDebug() << "Failed to compile shader: "+id
                    +"\nLog: "+QString::fromLocal8Bit(log);
        glDeleteShader(handle);
        return 0;
    }
    return handle;
}

int ShaderContainer::getProgramId()
{
    return programId;
}

void ShaderContainer::unload()
{
    removeAllocation();
    if(isAllocated())
        return;
    glDeleteProgram(programId);
    uniforms.clear();
    attributes.clear();
    programId = 0;
}

int ShaderContainer::getUniformLocation(QString name){
    if(uniforms.contains(name))
        return uniforms.value(name);
    return -1;
}

const QHash<QString, GLenum> ShaderContainer::getAttributes()
{
    return attributes_t;
}

const QHash<QString, GLenum> ShaderContainer::getUniforms()
{
    return uniforms_t;
}

int ShaderContainer::getAttributeLocation(QString name){
    if(attributes.contains(name))
        return attributes.value(name);
    return -1;
    int handle = glGetAttribLocation(getProgramId(),name.toStdString().c_str());
    return handle;
}

void ShaderContainer::setUniform(QString name,const glm::vec3& val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT_VEC3)
        glUniform3f(uniforms.value(name),val.x,val.y,val.z);
    else if(verbosity>1)
        qDebug() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, const glm::vec4 &val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT_VEC4)
        glUniform4f(uniforms.value(name),val.x,val.y,val.z,val.w);
    else if(verbosity>1)
        qDebug() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, const glm::vec2 &val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT_VEC2)
        glUniform2f(uniforms.value(name),val.x,val.y);
    else if(verbosity>1)
        qDebug() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, GLfloat val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT)
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

void ShaderContainer::setUniform(QString name, const glm::mat3 &val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT_MAT3)
        glUniformMatrix3fv(uniforms.value(name),1,GL_FALSE,glm::value_ptr(val));
    else if(verbosity>1)
        qDebug() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, const glm::mat4 &val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT_MAT4)
        glUniformMatrix4fv(uniforms.value(name),1,GL_FALSE,glm::value_ptr(val));
    else if(verbosity>1)
        qDebug() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, const ShaderVariant* val){
    if(uniforms.keys().contains(name)){
        switch(val->getType()){
        case ShaderVariant::ShaderVec2:
            setUniform(name,(*val->getVec2())());
            break;
        case ShaderVariant::ShaderVec3:
            setUniform(name,(*val->getVec3())());
            break;
        case ShaderVariant::ShaderVec4:
            setUniform(name,(*val->getVec4())());
            break;
        case ShaderVariant::ShaderMat3:
            setUniform(name,(*val->getMat3())());
            break;
        case ShaderVariant::ShaderMat4:
            setUniform(name,(*val->getMat4())());
            break;
        case ShaderVariant::ShaderDub:
            setUniform(name,(float)(*val->getDouble())());
            break;
        default:
            break;
        }
    }
}

QVariantMap ShaderContainer::getUniformsMap()
{
    QVariantMap r;
    for(QString k : uniforms.keys()){
        r.insert(k,QString("%1").arg(glbinding::Meta::getString(uniforms_t.value(k)).c_str()));
    }
    return r;
}

QVariantMap ShaderContainer::getAttributesMap()
{
    QVariantMap r;
    for(QString k : attributes.keys()){
        r.insert(k,QString("l:%1,t:%2")
                 .arg(attributes.value(k))
                 .arg(glbinding::Meta::getString(attributes_t.value(k)).c_str()));
    }
    return r;
}

QString ShaderContainer::fragmentShader() const
{
    return fragShaderFile;
}

QString ShaderContainer::vertexShader() const
{
    return vertShaderFile;
}

QString ShaderContainer::geometryShader() const
{
    return m_geometryShader;
}

void ShaderContainer::setFragmentShader(const QString &sh)
{
    this->fragShaderFile = sh;
}

void ShaderContainer::setVertexShader(const QString &sh)
{
    this->vertShaderFile = sh;
}

void ShaderContainer::setGeometryShader(const QString &geometryShader)
{
    m_geometryShader = geometryShader;
}
