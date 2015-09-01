#include "coffeeuniformblock.h"

#include "opengl/components/coffeebuffer.h"
#include "engine/scripting/qscriptvectorvalue.h"

#include "opengl/components/shadercontainer.h"

CoffeeUniformBlock::CoffeeUniformBlock(QObject *parent, uint64_t bufferSize) : QObject(parent)
{
    m_buffer = new CoffeeBuffer(this,GL_DYNAMIC_STORAGE_BIT,GL_UNIFORM_BUFFER);
    m_buffer->setSize(bufferSize);
    m_data = new QByteArray();
    m_data->resize(bufferSize);
}

CoffeeUniformBlock::CoffeeUniformBlock(QObject *parent, CoffeeUniformBlock* source, uint32_t offset, uint32_t size) : QObject(parent)
{
    //This means we are using a part of another CoffeeUniformBlock object for storage, thus no m_buffer object is available.
    connect(this,&CoffeeUniformBlock::dataRangeUpdated,
            [=](const void* data, uint32_t _offset, uint32_t _size){
        if(_offset+_size>size){
            qWarning("Attempted to write outside permitted area! Will not proceed!");
            return;
        }
        source->setDataRange(data,_offset+offset,_size);
    });
}

QString CoffeeUniformBlock::name() const
{
    return m_name;
}

QVector<CoffeeUniformValue *> CoffeeUniformBlock::getUniforms()
{
    return m_uniforms;
}

QByteArray *CoffeeUniformBlock::getData()
{
    return m_data;
}

void *CoffeeUniformBlock::getDataRange(uint32_t offset, uint32_t size)
{
    void* d = malloc(size);
    if(offset+size>(uint32_t)m_data->size())
        return d;
    memcpy(d,&m_data->data()[offset],size);
    return d;
}

CoffeeUniformBlock *CoffeeUniformBlock::getChild(uint32_t offset, uint32_t size)
{
    if(!m_data||offset+size>uint32_t(m_data->size()))
        return nullptr;
    return new CoffeeUniformBlock(this,this,offset,size);
}

void CoffeeUniformBlock::copyUniforms(CoffeeUniformBlock *src)
{
    for(CoffeeUniformValue* v : src->getUniforms())
        if(!m_uniforms.contains(v)&&v)
            m_uniforms.append(v);
}

void CoffeeUniformBlock::setUniformData(const QString uniformName,VectorData* uniformData)
{
    for(CoffeeUniformValue* v : m_uniforms)
        if(v->uniformName==uniformName){
            setDataRange(uniformData->getVectorData(),
                         v->blockOffset,
                         uniformData->getVectorDataSize());
            {
                Vector3Value *vec = dynamic_cast<Vector3Value*>(uniformData);
                if(v){
                    connect(vec,&Vector3Value::valueChanged,[=](){
                        setDataRange(uniformData->getVectorData(),
                                     v->blockOffset,
                                     uniformData->getVectorDataSize());
                    });
                }
            }
        }
}

void CoffeeUniformBlock::setDataRange(const void* data, uint32_t offset, uint32_t size)
{
    if(m_data){
        if(m_data->size()<int(offset+size))
            return;
        memcpy(&m_data->data()[offset],data,size);
        updateData(offset,size);
    }else{
        //We forward it to the parent of the block
        dataRangeUpdated(data,offset,size);
    }
}

void CoffeeUniformBlock::updateData(uint32_t offset, uint32_t size)
{
    Q_UNUSED(offset)
    Q_UNUSED(size)
    //updates a certain range of the buffer in GL memory
}

void CoffeeUniformBlock::addUniform(CoffeeUniformValue *val)
{
    if(!m_uniforms.contains(val))
        m_uniforms.append(val);
    else
        qWarning("Uniform already exists!");
}

void CoffeeUniformBlock::setName(const QString &name)
{
    m_name = name;
}


CoffeeUniformValue::CoffeeUniformValue(CoffeeShader *parent)
{
    m_shader = parent;
}

void CoffeeUniformValue::setUniform(const void *data, uint16_t size)
{
    m_shader->setUniformRaw(location,data,size);
}
