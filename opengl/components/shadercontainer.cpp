#include "shadercontainer.h"

#include "general/shadervariant.h"
#include <QMetaEnum>
#include "engine/data/coffeeresource.h"

ShaderContainer::ShaderContainer(QObject *parent) : QObject(parent)
{
}

ShaderContainer::~ShaderContainer()
{

}

bool ShaderContainer::buildProgram(CoffeeResource *vertShaderFile,
                                   CoffeeResource *fragShaderFile,
                                   CoffeeResource *geomShaderFile)
{
    if(isAllocated()){
        addAllocation();
        return true;
    }

    createProgram();

    this->vertShaderFile = vertShaderFile;
    this->fragShaderFile = fragShaderFile;
    m_geometryShader = geomShaderFile;

    compileShaders();

    if(!linkProgram())
        return false;

    return true;
}

bool ShaderContainer::buildProgram(CoffeeResource *vertShaderFile,
                                   CoffeeResource *fragShaderFile){
    return buildProgram(vertShaderFile,fragShaderFile,nullptr);
}

bool ShaderContainer::buildProgram()
{
    return buildProgram(vertShaderFile,fragShaderFile,m_geometryShader);
}

void ShaderContainer::compileShaders()
{
    std::string src = vertShaderFile->data()->toStdString();
    const char* code = src.c_str();
    addShader(code,vertexShader(),GL_VERTEX_SHADER);

    if(!fragmentShader().isEmpty()){
        src = fragShaderFile->data()->toStdString();
        code = src.c_str();
        addShader(code,fragmentShader(),GL_FRAGMENT_SHADER);
    }

    if(!geometryShader().isEmpty()){
        src = m_geometryShader->data()->toStdString();
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
        qFatal("Failed to link shader program: \nLog: %s",log);
        glDeleteProgram(programId);
        return false;
    }

    for(int shader : shaders){
        glDetachShader(programId,shader);
        glDeleteShader(shader);
    }

    GLint c;
    glGetProgramiv(programId,GL_ACTIVE_UNIFORMS,&c);
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

    qDebug("Shader info: name=%s,uniforms=%i, attributes=%i",
           objectName().toStdString().c_str(), uniforms.size(),
           attributes.size());

    addAllocation();
    return true;
}

GLuint ShaderContainer::compileShader(CoffeeResource* shader, const GLenum &shaderType){
    std::string src = shader->data()->toStdString();
    const char* code = src.c_str();
    return compileShaderSource(code,shader->source(),shaderType);
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
        qFatal("Failed to compile shader: %s\nLog: %s",id.toStdString().c_str(),log);
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

void ShaderContainer::setVerbosity(uint verbosity)
{
    this->verbosity = verbosity;
}
void ShaderContainer::setGeometryShader(CoffeeResource *geometryShader)
{
    m_geometryShader = geometryShader;
}

uint ShaderContainer::getVerbosity() const
{
    return verbosity;
}

void ShaderContainer::setVertexShader(CoffeeResource *value)
{
    vertShaderFile = value;
}

void ShaderContainer::setFragmentShader(CoffeeResource *value)
{
    fragShaderFile = value;
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
        qWarning() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, const glm::vec4 &val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT_VEC4)
        glUniform4f(uniforms.value(name),val.x,val.y,val.z,val.w);
    else if(verbosity>1)
        qWarning() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, const glm::vec2 &val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT_VEC2)
        glUniform2f(uniforms.value(name),val.x,val.y);
    else if(verbosity>1)
        qWarning() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, GLfloat val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT)
        glUniform1f(uniforms.value(name),val);
    else if(verbosity>1)
        qWarning() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, int val){
    if(uniforms.keys().contains(name))
        glUniform1i(uniforms.value(name),val);
    else if(verbosity>1)
        qWarning() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, const glm::mat3 &val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT_MAT3)
        glUniformMatrix3fv(uniforms.value(name),1,GL_FALSE,glm::value_ptr(val));
    else if(verbosity>1)
        qWarning() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, const glm::mat4 &val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT_MAT4)
        glUniformMatrix4fv(uniforms.value(name),1,GL_FALSE,glm::value_ptr(val));
    else if(verbosity>1)
        qWarning() << this->objectName() << "Failed to set uniform: " << name;
}

void ShaderContainer::setUniform(QString name, const ShaderVariant* val){
    //According to Callgrind, this is the time-consuming part of rendering.
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
            qWarning() << this->objectName() << "Unhandled data-type of" <<
                          ShaderVariant::staticMetaObject.className() <<
                          ":" <<
                          ShaderVariant::staticMetaObject.
                          enumerator(
                              ShaderVariant::staticMetaObject.
                              indexOfEnumerator("ShaderVariantType")
                          ).valueToKey(val->getType());
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
    if(fragShaderFile)
        return fragShaderFile->source();
    return "";
}

QString ShaderContainer::vertexShader() const
{
    if(vertShaderFile)
        return vertShaderFile->source();
    return "";
}

QString ShaderContainer::geometryShader() const
{
    if(m_geometryShader)
        return m_geometryShader->source();
    return "";
}
