#include "cdrenderer.h"

//#define LOAD_FILE

#include "coffee/cfunctional.h"
#include "coffee_impl/graphics/cshader.h"
#include "coffee/cfiles.h"
#include "coffee_impl/assimp/cassimpimporters.h"
#include "coffee_impl/assimp/cassimploader.h"

#include "coffee_impl/graphics/cgraphicsdata.h"
#include "coffee_impl/graphics/cframebuffer.h"

using namespace Coffee::CResources;
using namespace Coffee::CGraphicsWrappers;
using namespace Coffee::CGraphicsData;

namespace Coffee {
namespace CDisplay {

CDRenderer::CDRenderer(CObject *parent) : CGLBindingRenderer(parent)
{
}

CDRenderer::~CDRenderer()
{
}

void CDRenderer::run()
{

#ifndef LOAD_FILE
    CResource v = CResource("ubw/shaders/vertex/vsh_ubo.vs");
    CResource f = CResource("ubw/shaders/fragment/direct/fsh_nolight.fs");
    CShader* vshdr = new CShader;
    CShader* fshdr = new CShader;
    cDebug("Compile status: %i",vshdr->compile(&v,GL_VERTEX_SHADER));
    cDebug("Compile status: %i",fshdr->compile(&f,GL_FRAGMENT_SHADER));
#endif

    CResource testFile("ubw/models/ubw.blend");
    testFile.read_data();
    CResourceTypes::CAssimp::CAssimpData* res = CResourceTypes::CAssimp::CAssimpImporters::importResource(&testFile,"blend");
    testFile.free_data();

    CShaderProgram* prog = new CShaderProgram;
#ifndef LOAD_FILE
    prog->create();
    prog->attachShader(vshdr,GL_VERTEX_SHADER_BIT);
    prog->attachShader(fshdr,GL_FRAGMENT_SHADER_BIT);
    prog->link();
#else
    CResource progStore("shader.dmp");
    if(progStore.read_data())
        cDebug("Loaded program: %i",prog->fetchProgram(&progStore),true);
#endif

    CPipeline* pip = new CPipeline;
    pip->create();
    pip->attachProgram(prog,GL_VERTEX_SHADER_BIT|GL_FRAGMENT_SHADER_BIT);

#ifndef LOAD_FILE
    CResource progStore("shader.dmp");
    prog->storeProgram(&progStore);
    if(!progStore.save_data())
        cDebug("Failed to save shader!");
    progStore.free_data();
#endif
    CResourceTypes::CAssimp::CAssimpMesh* mesh = res->meshes[4];

    szptr posit_pos;
    szptr index_pos;

    for(int i=0;i<mesh->numBuffers;i++)
        switch(mesh->bufferType[i]){
        case 1:
            posit_pos = i;
            break;
        case 6:
            index_pos = i;
            break;
        }

    CBuffer vertexBuffer;
    vertexBuffer.bufferType = GL_ARRAY_BUFFER;
    vertexBuffer.flags = GL_DYNAMIC_STORAGE_BIT;
    vertexBuffer.create();
    vertexBuffer.bind();
    vertexBuffer.store(sizeof(glm::vec3)*mesh->bufferSize[posit_pos],nullptr);

    CBuffer uniformBuffer;
    uniformBuffer.bufferType = GL_UNIFORM_BUFFER;
    uniformBuffer.flags = GL_DYNAMIC_STORAGE_BIT;
    uniformBuffer.create();
    uniformBuffer.bind();
    uniformBuffer.store(sizeof(glm::mat4)*2,nullptr);

    CVertexArrayObject stdVao;
    stdVao.create();

    stdVao.bind();
    CSubBuffer posBuffer;
    posBuffer.parent = &vertexBuffer;
    posBuffer.bufferType = GL_ARRAY_BUFFER;
    posBuffer.size = sizeof(glm::vec3)*mesh->bufferSize[posit_pos];
    posBuffer.offset = 0;

    CVertexAttribute posAttr;
    posAttr.size = 3;
    posAttr.type = GL_FLOAT;
    posAttr.location = 0;
    posAttr.offset = 0;
    stdVao.addAttribute(&posAttr);

    posBuffer.bindParent();
    posBuffer.store(mesh->buffers[posit_pos]);

    CBuffer idxBuffer;
    idxBuffer.create();
    idxBuffer.bufferType = GL_ELEMENT_ARRAY_BUFFER;
    idxBuffer.bind();
    idxBuffer.store(sizeof(GLuint)*mesh->bufferSize[index_pos],mesh->buffers[index_pos]);

    stdVao.unbind();

    CGLDrawCall meshcall;
    meshcall.baseInstance = 0;
    meshcall.baseVertex = 0;
    meshcall.count = mesh->bufferSize[index_pos];
    meshcall.firstIndex = 0;
    meshcall.instanceCount = 1;

    res->freeData();
    delete res;

    CGCamera camera;
    camera.fieldOfView = 120.f;
    camera.aspect = 1.6f;
    camera.zVals.near = 0.1f;
    camera.zVals.far = 100.f;
    camera.genPerspective();

    CModelTransform model;
    model.position.z = -1.f;
    model.scale.x = model.scale.y = model.scale.z = 1.f;
    model.rotation.w = 2.f;

    CGraphicsData::CBlock* matrixBlock =
            CGraphicsData::coffee_create_block(sizeof(glm::mat4)*2,2);
    matrixBlock->propertySizes[0] = sizeof(glm::mat4);
    matrixBlock->propertySizes[1] = sizeof(glm::mat4);

    matrixBlock->setPropertyData(0,&model.matrix,sizeof(glm::mat4));
    matrixBlock->setPropertyData(1,&camera.matrix,sizeof(glm::mat4));

    uniformBuffer.bind();
    uniformBuffer.subStore(0,matrixBlock->dataSize(),matrixBlock->dataPtr());
    uniformBuffer.unbind();

    CUniformBlock matrices;
    matrices.blockBinding = 0;
    matrices.shaderIndex = glGetUniformBlockIndex(prog->handle,"MatrixBlock");

    {
        GLint thing;
        glGetProgramiv(prog->handle,GL_ACTIVE_UNIFORM_BLOCKS,&thing);
        cDebug("Uniform blocks: %i",thing);

        glGetProgramiv(prog->handle,GL_ACTIVE_UNIFORMS,&thing);
        cDebug("Uniforms: %i",thing);
    }

    CASSERT((matrices.shaderIndex!=GL_INVALID_INDEX));

    showWindow();

    CBuffer drawcalls;
    drawcalls.create();
    drawcalls.bufferType = GL_DRAW_INDIRECT_BUFFER;
    drawcalls.flags = (BufferStorageMask)0;
    drawcalls.bind();
    drawcalls.store(sizeof(CGLDrawCall),&meshcall);

    pip->bind();
    glCullFace(GL_BACK);
    glClearColor(0.175f,0.175f,0.175f,1.f);
    glViewport(0,0,m_properties.size.w,m_properties.size.h);

    bigscalar mtime = 0.0;
    CElapsedTimerMicro *t = new CElapsedTimerMicro;

    setWindowTitle(cStringFormat("GLFW OpenGL renderer (init time: %fs)",contextTime()));
    cMsg("Coffee","Init time: %fs",contextTime());

    idxBuffer.bind();
    stdVao.bind();
    glUniformBlockBinding(prog->handle,matrices.shaderIndex,matrices.blockBinding);
    uniformBuffer.bind();

    while(!closeFlag()){
        t->start();

        glClear(GL_COLOR_BUFFER_BIT);

        model.rotation=glm::normalize(glm::quat(2,0,0,-0.1)*model.rotation);
        model.genMatrix();

        uniformBuffer.subStore(0,sizeof(glm::mat4),&(model.matrix));

        uniformBuffer.bindRange(matrices.blockBinding,0,sizeof(glm::mat4)*2);

        glMultiDrawElementsIndirect(GL_TRIANGLES,GL_UNSIGNED_INT,0,1,sizeof(CGLDrawCall));
        if(contextTime()>mtime){
            cDebug("Render time: %lldus",t->elapsed());
            mtime = contextTime()+1.0;
        }

        executeRunQueue();
        pollEvents();
        swapBuffers();
    }

    t->start();

    hideWindow();

    pip->free();
#ifndef LOAD_FILE
    v.free_data();
    f.free_data();
    delete vshdr;
    delete fshdr;
#endif
    delete prog;
    delete pip;

    cMsg("Coffee","Termination time: %lldus",t->elapsed());
}

void CDRenderer::run(CDWindowProperties props)
{
    init(props);

    run();

    cleanup();
}

void CDRenderer::bindingCallback(CGLReport *report) const
{
    CString smsg = report->message;
    CString out = glbinding::Meta::getString(report->type)+":"
            +glbinding::Meta::getString(report->severity)+":"
            +glbinding::Meta::getString(report->source)+": "+smsg;
    cDebug("OpenGL: %s",out.c_str());
    CGLState* state = _dump_state(); //Should provide a view of OpenGL state
    delete state;
    free(report);
}

void CDRenderer::eventWHandle(CDEvent *event)
{
    if(event->type==CDEvent::FramebufferResized)
        if(m_properties.contextProperties.flags&CGLContextProperties::GLAutoResize){
            CDResizeEvent* resize = reinterpret_cast<CDResizeEvent*>(&event[1]);
            cDebug("Resize: %ix%i",resize->w,resize->h);
            glViewport(0,0,resize->w,resize->h);
        }
}

void CDRenderer::eventIHandle(CIEvent *event)
{
    if(event->type==CIEvent::Keyboard){
        CIKeyEvent* kev = reinterpret_cast<CIKeyEvent*>(&event[1]);
        cDebug("Key event: t=%i,key=%i,mods=%i",kev->type,
               kev->keyCode,
               kev->modifier);
        if(kev->keyCode==256) //TODO: Add key mapping
            this->closeWindow();
    }
}

CGLState *CDRenderer::_dump_state() const
{
    CGLState *state = new CGLState;
    GLint t;

    glGetIntegerv(GL_VERTEX_ARRAY_BINDING,&t);
    state->vertex_array = t;

    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,&t);
    state->element_buffer = t;

    glGetIntegerv(GL_ARRAY_BUFFER_BINDING,&t);
    state->array_buffer = t;

    glGetIntegerv(GL_UNIFORM_BUFFER_BINDING,&t);
    state->uniform_buffer = t;

    glGetIntegerv(GL_DRAW_INDIRECT_BUFFER_BINDING,&t);
    state->indirect_buffer = t;

    glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING,&t);
    state->pipeline_obj = t;

    glGetIntegerv(GL_TEXTURE_BINDING_2D,&t);
    state->texture_2d = t;

    glGetIntegerv(GL_TEXTURE_BINDING_3D,&t);
    state->texture_3d = t;

    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP,&t);
    state->texture_cube = t;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING,&t);
    state->fb_all = t;

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,&t);
    state->fb_draw = t;

    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,&t);
    state->fb_read = t;

    glGetFloatv(GL_COLOR_CLEAR_VALUE,reinterpret_cast<GLfloat*>(&state->clear_color));

    return state;
}

} // namespace CDisplay
} // namespace Coffee
