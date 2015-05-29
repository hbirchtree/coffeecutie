#include "coffeeoutputsurface.h"

CoffeeOutputSurface::CoffeeOutputSurface(QObject *parent,CoffeeFrameBufferObject* display) : CoffeeObject(parent)
{
    setFramebuffer(display);
}

void CoffeeOutputSurface::setFramebuffer(CoffeeFrameBufferObject *display)
{
    this->framebuffer = display;
}

void CoffeeOutputSurface::load()
{
    mdl = NumberBuffer<GLfloat>::createArray(this,18);
    GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };
    shader = new ShaderContainer(this);
    shader->buildProgram("ubw/shaders/vsh_passthrough.txt",
                         "ubw/shaders/fsh_simple.txt");
    mdl->put(g_quad_vertex_buffer_data);

    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(g_quad_vertex_buffer_data),g_quad_vertex_buffer_data,GL_STATIC_DRAW);

    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(shader->getAttributeLocation("vert"));
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    shader->getUniformLocation("screen");
    shader->getUniformLocation("time");
    setBaked(true);
}

void CoffeeOutputSurface::render()
{
    if(!isBaked())
        load();
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader->getProgramId());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,framebuffer->getTextureHandle());
    shader->setUniform("screen",0);
    shader->setUniform("time",(float)glfwGetTime());

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES,0,6);
    glBindTexture(GL_TEXTURE_2D,0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void CoffeeOutputSurface::unload()
{
    glDeleteBuffers(1,&vbo);
    glDeleteVertexArrays(1,&vao);
}

bool CoffeeOutputSurface::isBaked() const
{
    return baked;
}

void CoffeeOutputSurface::setBaked(bool val)
{
    this->baked = val;
}
