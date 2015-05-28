#ifndef COFFEERENDERINGMETHOD_H
#define COFFEERENDERINGMETHOD_H

#include "general/common.h"
#include <map>
#include "opengl/components/coffeematerial.h"
#include "opengl/components/coffeeomnilight.h"
#include "opengl/components/shadercontainer.h"
#include "general/data/numberbuffer.h"
#include "general/shadervariant.h"

class CoffeeRenderingMethod : public QObject
{
//    Q_PROPERTY(int uniformMappings READ uniformMappings)
//    Q_PROPERTY(int textureMappings READ textureMappings)

//    //Defines a generic process in which an object is processed when being rendered as well as baked
//public:
//    class VertexAttributeDescriptor {
//    public:
//        QString name;
//        int location = 0;
//        int size = 0;
//        gl::GLenum dataType = GL_FLOAT;
//        gl::GLboolean normalized = GL_FALSE;
//    };
//    class VAODescriptor {
//    public:
//        QList<VertexAttributeDescriptor*> descriptors;
//        int getStride(){
//            int atsize = 0;
//            for(VertexAttributeDescriptor* a : descriptors)
//                atsize+=a->size;
//            return atsize*dataSize;
//        }
//        GLint dataSize = 0;
//    };

//    typedef std::map<QPointer<CoffeeTexture>,gl::GLenum> texturemap;
//    typedef std::map<gl::GLenum,QString> textureunitmap;
//    typedef QMap<QString,QPointer<ShaderVariant> > uniformmap;

//    CoffeeRenderingMethod(QObject* parent);
//    CoffeeRenderingMethod(QObject* parent,RenderableObject* object);

//    QPointer<CoffeeRenderingMethod> createInstance(RenderableObject* target) const;

//    texturemap getTextureMap() const;
//    textureunitmap getTextureUnitMap() const;
//    uniformmap getUniformMap() const;

//    int countUniformMappings() const;
//    int countTextureMappings() const;

//public slots:
//    //Should only be called with Qt::DirectConnection type!
//    //These need to run in the rendering thread
//    void render();
//    void cleanupData();

//    void addTextureMapping(QPointer<CoffeeTexture> texture, gl::GLenum textureUnit, QString samplerName);
//    void addShaderUniform(QString uniform, QPointer<ShaderVariant> data);
//    void addVertexAttribute(QString name, int size);

//private:
//    CoffeeRenderingMethod(QObject* parent, RenderableObject* target, texturemap tmap, textureunitmap tunit, uniformmap umap, const VAODescriptor *vao);

//    void prepareData();

//    void resolveShader();
//    void loadUniforms();
//    void loadAttributes();
//    void allocTextures();
//    void bindTextures();
//    void unbindTextures();

//    RenderableObject* target;
//    VAODescriptor vaoDetails;

//    texturemap textureMapping;
//    textureunitmap tunitMapping;

//    //ShaderVariant is QVariant for std::function objects
//    uniformmap shaderSettings;

};

#endif // COFFEERENDERINGMETHOD_H
