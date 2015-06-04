#include "coffeeskybox.h"

CoffeeSkybox::CoffeeSkybox(QObject *parent,CoffeeCamera* camera) : CoffeeObject(parent)
{
    setCamera(camera);
//    this->shader = new ShaderContainer(this);
//    shader->setVertexShader("ubw/shaders/skybox/vsh.txt");
//    shader->setFragmentShader("ubw/shaders/skybox/fsh.txt");
}

void CoffeeSkybox::addMap(GLenum side, QString source)
{
    cubemapping.insert(side,source);
}

void CoffeeSkybox::setCamera(CoffeeCamera *camera)
{
    this->camera = camera;
}

void CoffeeSkybox::render()
{
    if(!baked)
        load();
    glUseProgram(shader->getProgramId());

    GLint oldCullMode;
    glGetIntegerv(GL_CULL_FACE_MODE,&oldCullMode);

    glDisable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,texture->getHandle());

    shader->setUniform("cubemapTexture",0);
    shader->setUniform("wvp",camera->getMatrix()*
                       RenderingMethods::translateObjectMatrix(camera->getPosition()->getValue(),
                                                               glm::quat(1,0,0,0),
                                                               glm::vec3(1,1,1)));

    glBindVertexArray(skymesh->getVertexArrayHandle());
    glDrawElements(GL_TRIANGLES,skymesh->getIndicesCount(),GL_UNSIGNED_INT,(GLvoid*)0);

    glCullFace(static_cast<GLenum>(oldCullMode));
    glEnable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void CoffeeSkybox::unload()
{
    glDeleteVertexArrays(1,&vao);
    glDeleteBuffers(2,buffs);
    texture->unloadTexture();
    glDisable(GL_TEXTURE_CUBE_MAP);
}

void CoffeeSkybox::load()
{
    if(!texture)
        texture = new CoffeeTexture(this,cubemapping);

    if(glIsEnabled(GL_TEXTURE_CUBE_MAP)==GL_FALSE)
        glEnable(GL_TEXTURE_CUBE_MAP);
    qDebug("Skybox initializing");

    texture->loadTexture();

    skymesh->loadMesh();

    shader->buildProgram();
    shader->getUniformLocation("wvp");
    shader->getUniformLocation("cubemapTexture");

    baked = true;

    qDebug("Skybox loaded");
}
QPointer<ShaderContainer> CoffeeSkybox::getShader() const
{
    return shader;
}

void CoffeeSkybox::setShader(QPointer<ShaderContainer> value)
{
    if(value)
        shader = value;
    else
        qDebug("Skybox shader does not exist!");
}

QPointer<CoffeeMesh> CoffeeSkybox::getSkymesh() const
{
    return skymesh;
}

void CoffeeSkybox::setSkymesh(QPointer<CoffeeMesh> value)
{
    if(value&&value->hasPositions())
        skymesh = value;
    else
        qDebug("Could not assign mesh without positions!");
}

QPointer<CoffeeTexture> CoffeeSkybox::getTexture() const
{
    return texture;
}

void CoffeeSkybox::setTexture(QPointer<CoffeeTexture> value)
{
    if(value&&value->isCubemap())
        this->texture = value;
    else
        qDebug("Could not assign 2D texture as cubemap!");
}
