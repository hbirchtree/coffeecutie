#include "coffeeskybox.h"

CoffeeSkybox::CoffeeSkybox(QObject *parent,CoffeeCamera* camera) : CoffeeObject(parent)
{
    setCamera(camera);
    this->shader = new ShaderContainer(this);
    shader->setVertexShader("ubw/shaders/skybox/vsh.txt");
    shader->setFragmentShader("ubw/shaders/skybox/fsh.txt");
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

    GLint oldCullMode,oldDepthFunc;
    glGetIntegerv(GL_CULL_FACE_MODE,&oldCullMode);
    glGetIntegerv(GL_DEPTH_FUNC,&oldDepthFunc);

    glDisable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,texture->getHandle());

    shader->setUniform("cubemapTexture",0);
    shader->setUniform("wvp",camera->getMatrix()*
                       RenderingMethods::translateObjectMatrix(camera->getPosition()->getValue(),
                                                               glm::quat(1,0,0,0),
                                                               glm::vec3(5,5,5)));

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,(GLvoid*)0);

    glCullFace(static_cast<GLenum>(oldCullMode));
    glDepthFunc(static_cast<GLenum>(oldDepthFunc));
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
}

void CoffeeSkybox::load()
{
    if(!texture)
        texture = new CoffeeTexture(this,cubemapping);

    qDebug("Skybox initializing");

    texture->loadTexture();

    glGenBuffers(2,buffs);
    glGenVertexArrays(1,&vao);

    glm::vec3 positiondata[8] = {
        glm::vec3(1.0,-1.0,-1.0),
        glm::vec3(1.0,-1.0,1.0),
        glm::vec3(-1.0,-1.0,1.0),
        glm::vec3(-1.0,-1.0,-1.0),
        glm::vec3(1.0,1.0,-1.0),
        glm::vec3(1.0,1.0,1.0),
        glm::vec3(-1.0,1.0,1.0),
        glm::vec3(-1.0,1.0,-1.0)
    };

    GLuint ibodata[36] = {
        1, 2, 3,
        5, 8, 6,
        1, 5, 2,
        2, 6, 3,
        3, 7, 8,
        5, 1, 8,

        4, 1, 3,
        5, 6, 2,
        1, 4, 8,
        8, 7, 6,
        4, 3, 8,
        6, 7, 3,
    };

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER,buffs[0]);
    glBufferData(GL_ARRAY_BUFFER,8*sizeof(glm::vec3),positiondata,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,36*sizeof(GLuint),ibodata,GL_STATIC_DRAW);

    glBindVertexArray(0);

    shader->buildProgram();
    shader->getUniformLocation("wvp");
    shader->getUniformLocation("cubemapTexture");

    baked = true;

    qDebug("Skybox loaded");
}
