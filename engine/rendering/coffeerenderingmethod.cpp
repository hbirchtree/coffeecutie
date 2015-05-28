#include "coffeerenderingmethod.h"

//CoffeeRenderingMethod::CoffeeRenderingMethod(QObject *parent) : QObject(parent)
//{
//    target = nullptr;
//    vaoDetails.dataSize = sizeof(GLfloat);
//}

//CoffeeRenderingMethod::CoffeeRenderingMethod(QObject* parent,RenderableObject *object) : CoffeeRenderingMethod(parent)
//{
//    target = object;
//}

//QPointer<CoffeeRenderingMethod> CoffeeRenderingMethod::createInstance(RenderableObject *target) const
//{
//    return new CoffeeRenderingMethod(this->parent(),target,this->getTextureMap(),this->getTextureUnitMap(),this->getUniformMap(),&vaoDetails);
//}

//CoffeeRenderingMethod::texturemap CoffeeRenderingMethod::getTextureMap() const
//{
//    return textureMapping;
//}

//CoffeeRenderingMethod::textureunitmap CoffeeRenderingMethod::getTextureUnitMap() const
//{
//    return tunitMapping;
//}

//CoffeeRenderingMethod::uniformmap CoffeeRenderingMethod::getUniformMap() const
//{
//    return shaderSettings;
//}

//int CoffeeRenderingMethod::countUniformMappings() const
//{
//    return shaderSettings.size();
//}

//int CoffeeRenderingMethod::countTextureMappings() const
//{
//    return textureMapping.size();
//}

//void CoffeeRenderingMethod::prepareData()
//{
//    if(target->isBaked())
//        return;
//    resolveShader();
////    loadAttributes();

//    allocTextures();
//    target->setBaked(true);
//}

//void CoffeeRenderingMethod::render()
//{
//    if(!target)
//        throw std::invalid_argument("CoffeeRenderingMethod object not renderable : this object is identified as a factory");
//    if(!target->isBaked())
//        prepareData();
//    glUseProgram(target->getShader()->getProgramId());
//    loadUniforms();
//    bindTextures();

//    glBindVertexArray(target->getVaoHandle());
//    glDrawArrays(GL_TRIANGLES,0,target->getVerticesCount());

//    unbindTextures();
//    glBindVertexArray(0);
//    glUseProgram(0);
//}

//void CoffeeRenderingMethod::cleanupData()
//{
//    if(!target)
//        throw std::invalid_argument("CoffeeRenderingMethod object not renderable : this object is identified as a factory");
//    target->unloadAssets();
//}

//void CoffeeRenderingMethod::addTextureMapping(QPointer<CoffeeTexture> texture, gl::GLenum textureUnit, QString samplerName){
//    try{
//    tunitMapping.at(textureUnit);
//    }catch(std::out_of_range err){
//        textureMapping.insert(
//                    std::pair< QPointer<CoffeeTexture>,gl::GLenum>(texture,textureUnit));
//        tunitMapping.insert(std::pair<gl::GLenum,QString>(textureUnit,samplerName));
//        return;
//    }
//    for(texturemap::iterator it = textureMapping.begin();it!=textureMapping.end();++it){
//        if(it->second==textureUnit)
//            it->first->unloadTexture();
//    }
//}

//void CoffeeRenderingMethod::addShaderUniform(QString uniform, QPointer<ShaderVariant> data)
//{
//    shaderSettings.insert(uniform,data);
//}

//void CoffeeRenderingMethod::addVertexAttribute(QString name, int size)
//{
//    VertexAttributeDescriptor* attr = new VertexAttributeDescriptor();
//    attr->size = size;
//    attr->name = name;
//    vaoDetails.descriptors.append(attr);
//}

//CoffeeRenderingMethod::CoffeeRenderingMethod(QObject *parent, RenderableObject *target, CoffeeRenderingMethod::texturemap tmap, CoffeeRenderingMethod::textureunitmap tunit, CoffeeRenderingMethod::uniformmap umap, const VAODescriptor* vao) : QObject(parent)
//{
//    this->target = target;
//    this->textureMapping = texturemap(tmap);
//    this->tunitMapping = textureunitmap(tunit);
//    this->shaderSettings = uniformmap(umap);
//    this->vaoDetails.dataSize = vao->dataSize;
//    this->vaoDetails.descriptors = QList<VertexAttributeDescriptor*>(vao->descriptors);
//}

//void CoffeeRenderingMethod::resolveShader()
//{
//    if(!target->getShader())
//        throw std::runtime_error("cannot work with null shader!");

//    target->getShader()->getUniformLocations(shaderSettings.keys());
//}

//void CoffeeRenderingMethod::loadUniforms()
//{
//    ShaderContainer* shader = target->getShader();
//    if(!shader)
//        return;
//    for(QString uniform : shaderSettings.keys()){
//        QPointer<ShaderVariant> data = shaderSettings.value(uniform);
//        if(!data)
//            return;
//        switch(data->getType()){
//        case ShaderVariant::ShaderDub:{
//            shader->setUniform(uniform,(float)data->getDouble()());
//            break;
//        }
//        case ShaderVariant::ShaderVec2:{
//            shader->setUniform(uniform,data->getVec2()());
//            break;
//        }
//        case ShaderVariant::ShaderVec3:{
//            shader->setUniform(uniform,data->getVec3()());
//            break;
//        }
//        case ShaderVariant::ShaderVec4:{
//            shader->setUniform(uniform,data->getVec4()());
//            break;
//        }
//        case ShaderVariant::ShaderMat4:{
//            shader->setUniform(uniform,data->getMat4()());
//            break;
//        }
//        default:
//            break;
//        }
//    }
//    for(texturemap::iterator it = textureMapping.begin();it!=textureMapping.end();++it){
//        shader->setUniform(tunitMapping.at(it->second),static_cast<int>(it->second)-static_cast<int>(GL_TEXTURE0));
//    }
//}

//void CoffeeRenderingMethod::loadAttributes()
//{
//    if(target->getVaoHandle()>0)
//        return;

//    for(VertexAttributeDescriptor* attr : vaoDetails.descriptors)
//        attr->location = target->getShader()->getAttributeLocation(attr->name);

//    GLuint vbo[2];
//    GLuint vao = 0;
//    GLenum drawMode = GL_STATIC_DRAW;
//    if(target->isStreamDraw())
//        drawMode = GL_STREAM_DRAW;
//    glGenBuffers(2,vbo);

//    glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);
//    NumberBuffer<GLfloat>* data = target->getVertexData();
//    glBufferData(GL_ARRAY_BUFFER,data->getSize()*sizeof(GLfloat),data->get(),drawMode);

////    NumberBuffer<uint>* indices = nullptr;
////    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbo[1]);
////    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices->getSize()*sizeof(unsigned int),indices->get(),drawMode);

//    glGenVertexArrays(1,&vao);
//    glBindVertexArray(vao);

//    GLint accumulated_stride = 0;
//    for(VertexAttributeDescriptor* attr : vaoDetails.descriptors){
//        glEnableVertexAttribArray(attr->location);
//        glVertexAttribPointer(attr->location,attr->size,attr->dataType,attr->normalized,vaoDetails.getStride(),(GLvoid*)(vaoDetails.dataSize*accumulated_stride));
//        accumulated_stride+=attr->size;
//    }

//    glBindBuffer(GL_ARRAY_BUFFER,0);
//    glBindVertexArray(0);

//    target->setVaoHandle(vao);
//    target->setVboHandle(vbo[0]);
//}

//void CoffeeRenderingMethod::allocTextures()
//{
//    for(texturemap::iterator it = textureMapping.begin();it!=textureMapping.end();++it){
//        target->getShader()->getUniformLocation(tunitMapping.at(it->second));
//        it->first->loadTexture();
//    }
//}

//void CoffeeRenderingMethod::bindTextures()
//{
//    for(texturemap::iterator it = textureMapping.begin();it!=textureMapping.end();++it){
//        glActiveTexture(it->second);
//        glBindTexture(GL_TEXTURE_2D,it->first->getHandle());
//    }
//}

//void CoffeeRenderingMethod::unbindTextures()
//{
//    for(texturemap::iterator it = textureMapping.begin();it!=textureMapping.end();++it){
//        glActiveTexture(it->second);
//        glBindTexture(GL_TEXTURE_2D,0);
//    }
//}
