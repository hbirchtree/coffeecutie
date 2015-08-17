#include "shadercontainer.h"

#include "opengl/components/coffeeuniformblock.h"
#include "engine/scripting/qscriptvectorvalue.h"
#include <QMetaEnum>
#include "engine/data/coffeeresource.h"

CoffeeShader::CoffeeShader(QObject *parent) : QObject(parent)
{
}

CoffeeShader::~CoffeeShader()
{

}

bool CoffeeShader::buildProgram(CoffeeResource *vertShaderFile,
                                   CoffeeResource *fragShaderFile,
                                   CoffeeResource *geomShaderFile)
{
    if(isAllocated()){
        addAllocation();
        return true;
    }

    createProgram();

    this->m_vertexShader = vertShaderFile;
    this->m_fragmentShader = fragShaderFile;
    m_geometryShader = geomShaderFile;

    compileShaders();

    if(!linkProgram())
        return false;

    return true;
}

bool CoffeeShader::buildProgram(CoffeeResource *vertShaderFile,
                                   CoffeeResource *fragShaderFile){
    return buildProgram(vertShaderFile,fragShaderFile,nullptr);
}

bool CoffeeShader::buildProgram()
{
    return buildProgram(m_vertexShader,m_fragmentShader,m_geometryShader);
}

void CoffeeShader::compileShaders()
{
    std::string src = m_vertexShader->data()->toStdString();
    const char* code = src.c_str();
    addShader(code,vertexShader(),GL_VERTEX_SHADER);

    if(!fragmentShader().isEmpty()){
        src = m_fragmentShader->data()->toStdString();
        code = src.c_str();
        addShader(code,fragmentShader(),GL_FRAGMENT_SHADER);
    }

    if(!geometryShader().isEmpty()){
        src = m_geometryShader->data()->toStdString();
        code = src.c_str();
        addShader(code,geometryShader(),GL_GEOMETRY_SHADER);
    }
}

void CoffeeShader::createProgram()
{
    programId = glCreateProgram();
}

bool CoffeeShader::addShader(const char *data, const QString &id, const GLenum &shaderType)
{
    GLuint shader = compileShaderSource(data,id,shaderType);
    if(shader>0)
        shaders.append(shader);
    else
        return false;
    return true;
}

bool CoffeeShader::linkProgram()
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


    getProgramAttributes();
    getProgramUniforms();

//    qDebug("Shader info: name=%s,uniforms=%i, uniform blocks=%i, attributes=%i",
//           objectName().toStdString().c_str(),
//           uniforms.size(),
//           uniformBlocks.size(),
//           attributes.size());

    addAllocation();
    return true;
}

GLuint CoffeeShader::compileShader(CoffeeResource* shader, const GLenum &shaderType){
    std::string src = shader->data()->toStdString();
    const char* code = src.c_str();
    return compileShaderSource(code,shader->source(),shaderType);
}

GLuint CoffeeShader::compileShaderSource(const char *data, const QString &id, const GLenum &shaderType)
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

int CoffeeShader::getProgramId()
{
    return programId;
}

void CoffeeShader::unload()
{
    removeAllocation();
    if(isAllocated())
        return;
    glDeleteProgram(programId);
    uniforms.clear();
    attributes.clear();
    programId = 0;
}

void CoffeeShader::setVerbosity(uint verbosity)
{
    this->verbosity = verbosity;
}
void CoffeeShader::setGeometryShader(CoffeeResource *geometryShader)
{
    m_geometryShader = geometryShader;
}

uint CoffeeShader::getVerbosity() const
{
    return verbosity;
}

CoffeeUniformBlock *CoffeeShader::getUniformBlock(const QString &name)
{
    for(CoffeeUniformBlock* b : m_uniformBlocks)
        if(b->name()==name)
            return b;
    return nullptr;
}

CoffeeUniformValue *CoffeeShader::getUniformValue(const QString &name)
{
    for(CoffeeUniformValue* b : m_uniformValues)
        if(b->uniformName==name)
            return b;
    return nullptr;
}

void CoffeeShader::getProgramAttributes()
{
    GLint c;
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
}

void CoffeeShader::getProgramUniforms()
{
    GLint c;
    glGetProgramiv(programId,GL_ACTIVE_UNIFORMS,&c);
    for(int i=0;i<c;i++){
        m_uniformValues.append(getProgramUniform(i));
        GLchar o[64];
        int sz;
        GLenum t;
        glGetActiveUniform(programId,i,64,nullptr,&sz,&t,o);
        int handle = glGetUniformLocation(programId,o);
        uniforms.insert(QString(o),handle);
        uniforms_t.insert(QString(o),t);
    }

    glGetProgramiv(programId,GL_ACTIVE_UNIFORM_BLOCKS,&c);
    for(int i=0;i<c;i++){
        m_uniformBlocks.append(getProgramUniformBlock(i));
        GLchar o[64];
        int sz;
        glGetActiveUniformBlockName(programId,i,64,&sz,o);
        int handle = glGetUniformBlockIndex(programId,o);
        GLint size;
        glGetActiveUniformBlockiv(programId,i,GL_UNIFORM_BLOCK_DATA_SIZE,&size);
        uniformBlocks.insert(QString(o),handle);
    }
}

CoffeeUniformValue *CoffeeShader::getProgramUniform(GLuint index)
{
    CoffeeUniformValue* v = new CoffeeUniformValue(this);

    GLint t;

    GLchar o[64];
    glGetActiveUniformName(programId,index,64,nullptr,o);
    v->uniformName = QString(o);

    glGetActiveUniformsiv(programId,1,&index,GL_UNIFORM_TYPE,&t);
    v->uniformType = static_cast<GLenum>(t);

    glGetActiveUniformsiv(programId,1,&index,GL_UNIFORM_BLOCK_INDEX,&t);
    v->blockIndex = t;

    glGetActiveUniformsiv(programId,1,&index,GL_UNIFORM_OFFSET,&t);
    v->blockOffset = t;

    glGetActiveUniformsiv(programId,1,&index,GL_UNIFORM_SIZE,&t);
    v->uniformSize = t;

    glGetActiveUniformsiv(programId,1,&index,GL_UNIFORM_ARRAY_STRIDE,&t);
    v->arrayStride= t;

    v->location = glGetUniformLocation(programId,o);

    return v;
}

CoffeeUniformBlock *CoffeeShader::getProgramUniformBlock(GLuint index)
{
    GLchar o[64];
    glGetActiveUniformBlockName(programId,index,64,nullptr,o);

    GLint size;
    glGetActiveUniformBlockiv(programId,index,GL_UNIFORM_BLOCK_DATA_SIZE,&size);

    CoffeeUniformBlock* b = new CoffeeUniformBlock(0,size);
    b->setName(o);

    for(CoffeeUniformValue* v : m_uniformValues)
        if(v->blockIndex==(uint16_t)index)
            b->addUniform(v);

    return b;
}

void CoffeeShader::setVertexShader(CoffeeResource *value)
{
    m_vertexShader = value;
}

void CoffeeShader::setFragmentShader(CoffeeResource *value)
{
    m_fragmentShader = value;
}


int CoffeeShader::getUniformLocation(const QString &name){
    if(uniforms.contains(name))
        return uniforms.value(name);
    return -1;
}

QVector<CoffeeUniformValue *> CoffeeShader::getUniforms()
{
    return m_uniformValues;
}

QVector<CoffeeUniformBlock *> CoffeeShader::getUniformBlocks()
{
    return m_uniformBlocks;
}

int CoffeeShader::getAttributeLocation(const QString &name){
    if(attributes.contains(name))
        return attributes.value(name);
    return -1;

    //Dead code..?
    int handle = glGetAttribLocation(getProgramId(),name.toStdString().c_str());
    return handle;
}

void CoffeeShader::setUniform(const QString &name, const glm::vec3& val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT_VEC3)
        glUniform3f(uniforms.value(name),val.x,val.y,val.z);
    else if(verbosity>1)
        qWarning() << this->objectName() << "Failed to set uniform: " << name;
}

void CoffeeShader::setUniform(const QString &name, const glm::vec4 &val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT_VEC4)
        glUniform4f(uniforms.value(name),val.x,val.y,val.z,val.w);
    else if(verbosity>1)
        qWarning() << this->objectName() << "Failed to set uniform: " << name;
}

void CoffeeShader::setUniform(const QString &name, const glm::vec2 &val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT_VEC2)
        glUniform2f(uniforms.value(name),val.x,val.y);
    else if(verbosity>1)
        qWarning() << this->objectName() << "Failed to set uniform: " << name;
}

void CoffeeShader::setUniform(const QString &name, GLfloat val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT)
        glUniform1f(uniforms.value(name),val);
    else if(verbosity>1)
        qWarning() << this->objectName() << "Failed to set uniform: " << name;
}

void CoffeeShader::setUniform(const QString &name, int val){
    if(uniforms.keys().contains(name))
        glUniform1i(uniforms.value(name),val);
    else if(verbosity>1)
        qWarning() << this->objectName() << "Failed to set uniform: " << name;
}

void CoffeeShader::setUniform(const QString &name, const glm::mat3 &val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT_MAT3)
        glUniformMatrix3fv(uniforms.value(name),1,GL_FALSE,glm::value_ptr(val));
    else if(verbosity>1)
        qWarning() << this->objectName() << "Failed to set uniform: " << name;
}

void CoffeeShader::setUniform(const QString &name, const glm::mat4 &val){
    if(uniforms.keys().contains(name)&&uniforms_t.value(name)==GL_FLOAT_MAT4)
        glUniformMatrix4fv(uniforms.value(name),1,GL_FALSE,glm::value_ptr(val));
    else if(verbosity>1)
        qWarning() << this->objectName() << "Failed to set uniform: " << name;
}

void CoffeeShader::setUniform(const QString &name, VectorData *val)
{
    setUniformRaw(getUniformLocation(name),val->getVectorData(),val->getVectorDataSize());
}

void CoffeeShader::setUniformRaw(GLint location, const void *data, uint32_t size)
{
    switch(size){
    case sizeof(float):{
        glUniform1fv(location,
                     1,
                     reinterpret_cast<const GLfloat*>(data));
        break;
    }
    case sizeof(glm::vec2):{
        glUniform2fv(location,
                     1,
                     reinterpret_cast<const GLfloat*>(data));
        break;
    }
    case sizeof(glm::vec3):{
        glUniform3fv(location,
                     1,
                     reinterpret_cast<const GLfloat*>(data));
        break;
    }
    case sizeof(glm::vec4):{
        glUniform4fv(location,
                     1,
                     reinterpret_cast<const GLfloat*>(data));
        break;
    }
    case sizeof(glm::mat3):{
        glUniformMatrix3fv(location,
                     1,GL_FALSE,
                     reinterpret_cast<const GLfloat*>(data));
        break;
    }
    case sizeof(glm::mat4):{
        glUniformMatrix4fv(location,
                     1,GL_FALSE, // <-- Transpose flag!
                     reinterpret_cast<const GLfloat*>(data));
        break;
    }
    }
}

void CoffeeShader::bindUniformBufferRange(GLuint uboIndex, GLuint uboHandle, uint32_t offset, uint32_t size)
{
    glBindBuffer(GL_UNIFORM_BUFFER,uboHandle);
    glBindBufferRange(GL_UNIFORM_BUFFER,uboIndex,uboHandle,offset,size);
}

void CoffeeShader::useProgram()
{
    glUseProgram(programId);
}

void CoffeeShader::load()
{
    buildProgram();
}

QVariantMap CoffeeShader::getUniformsMap()
{
    QVariantMap r;
    for(QString k : uniforms.keys()){
        r.insert(k,QString("%1").arg(glbinding::Meta::getString(uniforms_t.value(k)).c_str()));
    }
    return r;
}

QVariantMap CoffeeShader::getAttributesMap()
{
    QVariantMap r;
    for(QString k : attributes.keys()){
        r.insert(k,QString("l:%1,t:%2")
                 .arg(attributes.value(k))
                 .arg(glbinding::Meta::getString(attributes_t.value(k)).c_str()));
    }
    return r;
}

QString CoffeeShader::fragmentShader() const
{
    if(m_fragmentShader)
        return m_fragmentShader->source();
    return "";
}

QString CoffeeShader::vertexShader() const
{
    if(m_vertexShader)
        return m_vertexShader->source();
    return "";
}

QString CoffeeShader::geometryShader() const
{
    if(m_geometryShader)
        return m_geometryShader->source();
    return "";
}
