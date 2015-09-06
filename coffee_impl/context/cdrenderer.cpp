#include "cdrenderer.h"

//#define LOAD_FILE

#include "coffee/cfunctional.h"
#include "coffee_impl/graphics/cshader.h"
#include "coffee/cfiles.h"
#include "coffee_impl/assimp/cassimpimporters.h"
#include "coffee_impl/assimp/cassimploader.h"

#include "coffee_impl/graphics/cgraphicsdata.h"

using namespace Coffee::CResources;
using namespace Coffee::CGraphicsWrappers;

namespace Coffee {
namespace CDisplay {

CDRenderer::CDRenderer(CObject *parent) : CGLFWRenderer(parent)
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
    CResourceTypes::CAssimp::CAssimpData* res = CResourceTypes::CAssimp::CAssimpImporters::importResource(&testFile,".blend");
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

    CBufferedMesh bufm;
    bufm.load(mesh);

    res->freeData();
    delete res;

    glm::quat m_rot = glm::quat(2,0,0,0);
    glm::mat4 camera = glm::perspective(glm::radians(90.f),1.6f,0.1f,100.f);
    glm::mat4 model = glm::translate(glm::mat4(),glm::vec3(0,0,-10))*glm::mat4_cast(m_rot)*glm::scale(glm::mat4(),glm::vec3(10,10,10));

    GLuint m_u;
    m_u = glGetUniformBlockIndex(prog->handle,"MatrixBlock");

    {
        GLint thing;
        glGetProgramiv(prog->handle,GL_ACTIVE_UNIFORM_BLOCKS,&thing);
        cDebug("Uniform blocks: %i",thing);

        glGetProgramiv(prog->handle,GL_ACTIVE_UNIFORMS,&thing);
        cDebug("Uniforms: %i",thing);
    }

    CASSERT((m_u!=GL_INVALID_INDEX));

    showWindow();

    CGraphicsData::CBlock* matrixBlock =
            CGraphicsData::coffee_create_block(sizeof(glm::mat4)*3,2);
    matrixBlock->propertySizes[0] = sizeof(glm::mat4);
    matrixBlock->propertySizes[1] = sizeof(glm::mat4);

    matrixBlock->setPropertyData(0,&model,sizeof(glm::mat4));
    matrixBlock->setPropertyData(1,&camera,sizeof(glm::mat4));

    CBuffer matrixBuffer;
    matrixBuffer.create();
    matrixBuffer.bufferType = GL_UNIFORM_BUFFER;
    matrixBuffer.bind();
    matrixBuffer.store(GL_UNIFORM_BUFFER,matrixBlock->dataSize(),matrixBlock->dataPtr(),GL_DYNAMIC_STORAGE_BIT);

    bufm.vao()->bind();
    pip->bind();
    glCullFace(GL_BACK);
    glClearColor(0.175f,0.175f,0.175f,1.f);
    glViewport(0,0,1280,720);

    bigscalar mtime = 0.0;
    CElapsedTimerMicro *t = new CElapsedTimerMicro;

    setWindowTitle(cStringFormat("GLFW OpenGL renderer (init time: %fs)",contextTime()));
    cMsg("Coffee","Init time: %fs",contextTime());

    matrixBuffer.bind();
    glUniformBlockBinding(prog->handle,m_u,0);

    while(!closeFlag()){

        t->start();

        glClear(GL_COLOR_BUFFER_BIT);

        m_rot=glm::normalize(glm::quat(2,0,0.01,0)*m_rot);

        model = glm::translate(glm::mat4(),glm::vec3(0,0,-10))*glm::mat4_cast(m_rot)*glm::scale(glm::mat4(),glm::vec3(10,10,10));

        matrixBuffer.bind();
        matrixBuffer.subStore(0,sizeof(glm::mat4),&model);

        glBindBufferRange(GL_UNIFORM_BUFFER,0,matrixBuffer.handle,0,matrixBuffer.size);

        glDrawElements(GL_TRIANGLES,bufm.elements,GL_UNSIGNED_INT,0);
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

    bufm.vao()->free();
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

void CDRenderer::run(WindowState state, CSize resolution, int monitor)
{
    init(state,resolution,monitor);

    run();

    cleanup();
}

void CDRenderer::glbindingCallbackInternal(CGLReport *report) const
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

    glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING,&t);
    state->pipeline_obj = t;

    return state;
}

} // namespace CDisplay
} // namespace Coffee
