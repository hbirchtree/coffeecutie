#include "cdrenderer.h"

//#define LOAD_FILE

#include "coffee/cfunctional.h"
#include "coffee_impl/graphics/cshader.h"
#include "coffee/cfiles.h"
#include "coffee_impl/assimp/cassimpimporters.h"

#include "coffee_impl/graphics/cgraphicsdata.h"
#include "coffee_impl/graphics/cframebuffer.h"
#include "coffee_impl/graphics/cuniformchunk.h"

#include "coffee_impl/rendering/cmultidrawgraph.h"

using namespace Coffee::CResources;
using namespace Coffee::CGraphicsWrappers;
using namespace Coffee::CGraphicsData;
using namespace Coffee::CRendering;

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
    CResource v = CResource("ubw/shaders/vertex/vsh_instanced.vs");
    CResource f = CResource("ubw/shaders/fragment/direct/fsh_nolight.fs");
    CShader* vshdr = new CShader;
    CShader* fshdr = new CShader;
    cDebug("Compile status: %i",vshdr->compile(&v,GL_VERTEX_SHADER));
    cDebug("Compile status: %i",fshdr->compile(&f,GL_FRAGMENT_SHADER));
#endif

    //Loading a mesh

    CResource testFile("ubw/models/ubw.blend");
    testFile.read_data();
    CResourceTypes::CAssimp::CAssimpData* res = CResourceTypes::CAssimp::CAssimpImporters::importResource(&testFile,"blend");
    testFile.free_data();

    //Loading up shader

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

    //End of shader loading

    //Testing: Dumping shader program to file, caching

#ifndef LOAD_FILE
    CResource progStore("shader.dmp");
    prog->storeProgram(&progStore);
    if(!progStore.save_data())
        cDebug("Failed to save shader!");
    progStore.free_data();
#endif
    CResourceTypes::CAssimp::CAssimpMesh* mesh = res->meshes[4];

    //Mesh specification
    CBuffer vertexBuffer;
    vertexBuffer.create();
    vertexBuffer.bufferType = GL_ARRAY_BUFFER;

    //Instance buffer, stores object transforms for *all* objects
    CBuffer instanceBuffer;
    instanceBuffer.bufferType = GL_ARRAY_BUFFER;
    instanceBuffer.flags = GL_DYNAMIC_STORAGE_BIT;
    instanceBuffer.create();
    instanceBuffer.bind();
    instanceBuffer.store(sizeof(glm::mat4)*10000,nullptr);
    instanceBuffer.unbind();

    CVertexFormat stdFmt;
    stdFmt.type = GL_FLOAT;
    stdFmt.offset = 0;
    stdFmt.size = 3;

    CVertexFormat matFmt;
    matFmt.type = GL_FLOAT;
    matFmt.offset = 0;
    matFmt.size = 4;

    CVertexBufferBinding posBnd;
    posBnd.buffer = &vertexBuffer;
    posBnd.binding = 0;
    posBnd.divisor = 0;
    posBnd.offset = 0;
    posBnd.stride = sizeof(CVec3);

    CMultiDrawDescriptor desc;

    CVertexAttribute posA;
    posA.attribIdx = 0;
    posA.bnd = &posBnd;
    posA.fmt = &stdFmt;
    desc.attributes.push_back(posA);

    CMultiDrawDataSet multidraw = coffee_multidraw_create();

    coffee_mesh_define_matrix_attribs(&instanceBuffer,matFmt,desc,5,1);
    coffee_multidraw_load_vao(multidraw,desc);

    //Vertex data
    std::vector<byte> verData;
    for(int i=0;i<mesh->numBuffers;i++)
        if(mesh->bufferType[i]==CAssimpMesh::PositionType){
            coffee_mesh_load_vertexdata(verData,mesh->buffers[i],
                                        0,mesh->bufferSize[i]*sizeof(CVec3));
        }
    vertexBuffer.bind();
    vertexBuffer.store(verData.size(),verData.data());
    vertexBuffer.unbind();
    coffee_multidraw_create_call(multidraw,mesh);

    multidraw.drawcalls->drawcalls.data()[0].instanceCount = 10000;
    //END Vertex data

    coffee_multidraw_load_drawcalls(multidraw);
    coffee_multidraw_load_indices(multidraw);

    // END mesh specification

    //Uniform buffer storage

    CBuffer uniformBuffer;
    uniformBuffer.bufferType = GL_UNIFORM_BUFFER;
    uniformBuffer.flags = GL_DYNAMIC_STORAGE_BIT;
    uniformBuffer.create();
    uniformBuffer.bind();
    uniformBuffer.store(sizeof(glm::mat4)*2,nullptr);
    uniformBuffer.unbind();

    res->freeData();
    delete res;

    //Representation of camera

    CGCamera camera;
    camera.fieldOfView = 120.f;
    camera.aspect = 1.6f;
    camera.zVals.near = 0.1f;
    camera.zVals.far = 100.f;
    camera.genPerspective();

    //Model transform

    CModelTransform model;
    model.position.z = -1.f;
    model.scale.x = model.scale.y = model.scale.z = 1.f;
    model.rotation.w = 2.f;

    instanceBuffer.bind();
    for(szptr i=1;i<10000;i++)
    {
        model.position.z = (float)(-i);
        model.position.y = (float)((i%11)-5);
        model.genMatrix();
        instanceBuffer.subStore(sizeof(glm::mat4)*i,sizeof(glm::mat4),&model.matrix);
    }
    instanceBuffer.unbind();

    model.position.z = -1.f;
    model.position.y = 0.f;
    model.position.x = 0.f;

    showWindow();

    pip->bind();
    glCullFace(GL_BACK);
    glClearColor(0.175f,0.175f,0.175f,1.f);
    glViewport(0,0,m_properties.size.w,m_properties.size.h);

    bigscalar mtime = 0.0;
    CElapsedTimerMicro *swap = new CElapsedTimerMicro;

    //Organizing the uniform chunk

    //Create sub-buffer from UBO
    CSubBuffer matrixBuf;
    matrixBuf.parent = &uniformBuffer;
    matrixBuf.size = sizeof(glm::mat4)*2;
    matrixBuf.bufferType = GL_UNIFORM_BUFFER;
    szptr matrixSz[2] = {sizeof(glm::mat4),sizeof(glm::mat4)};
    //Function creates our chunk
    CUniformChunk* uchunk =
            coffee_create_uchunk(&matrixBuf,sizeof(glm::mat4)*2,2,matrixSz,"MatrixBlock");
    //Set initial data
    uchunk->buffer->parent->bind();
    uchunk->buffer->subStore(0,sizeof(glm::mat4),&camera.matrix);
    uchunk->buffer->parent->unbind();

    //Set binding for the chunk
    uchunk->ublock.blockBinding = 0;
    uchunk->ublock.shaderIndex = glGetUniformBlockIndex(prog->handle,uchunk->ublock.name);
    glUniformBlockBinding(prog->handle,uchunk->ublock.shaderIndex,uchunk->ublock.blockBinding);

    setWindowTitle(cStringFormat("%s renderer (init time: %fs)",
                                 m_contextString.c_str(),
                                 contextTime()));
    cMsg("Coffee","Init time: %fs",contextTime());

    double delta = contextTime();
    double deltaT = 0;
    uint64 frames = 0;
    uint64 rendertime = 0;
    uint64 inputtime = 0;
    uint64 qtime = 0;
    uint64 swaptime = 0;

    setSwapInterval(0);

    instanceBuffer.bind();
    coffee_multidraw_bind_states(multidraw);
    uchunk->buffer->bindRange();

    while(!closeFlag()){
        delta = contextTime();
        swap->start();

//        Rendering part
        glClear(GL_COLOR_BUFFER_BIT);

        model.rotation=glm::normalize(glm::quat(2,0,0,-0.1*deltaT)*model.rotation);
        model.genMatrix();

        instanceBuffer.bind();
        instanceBuffer.subStore(0,sizeof(glm::mat4),&(model.matrix));

        uchunk->buffer->bindRange();
        coffee_multidraw_render(multidraw);
        glFlush();

        rendertime = swap->elapsed();
//        // END Rendering part

//        //Event handling
        swap->start();
        executeRunQueue();
        qtime = swap->elapsed();
        swap->start();
        pollEvents();
        inputtime = swap->elapsed();
        //Buffer swapping. Really slow...
        swap->start();
        swapBuffers();
        swaptime = swap->elapsed();
        frames++;

        //Info
        deltaT = contextTime()-delta;
        if(contextTime()>mtime){
            cDebug("Render time: %lldus, swap time: %lldus, "
                   "input: %lldus, queue: %lldus, frames: %lld, frametime: %lldus",
                   rendertime,swaptime,
                   inputtime,qtime,frames-1,
                   rendertime+swaptime+inputtime+qtime);
            mtime = contextTime()+1.0;
            frames = 0;
        }
    }

    swap->start();

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

    cMsg("Coffee","Termination time: %lldus",swap->elapsed());
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
//    CGLState* state = _dump_state(); //Should provide a view of OpenGL state
//    delete state;
    free(report);
}

void CDRenderer::eventWHandle(CDEvent *event)
{
    if(event->type==CDEvent::FramebufferResized)
        if(m_properties.contextProperties.flags&CGLContextProperties::GLAutoResize){
            CDResizeEvent* resize = (CDResizeEvent*)&event[1];
            cDebug("Resize: %ix%i",resize->w,resize->h);
            glViewport(0,0,resize->w,resize->h);
        }
}

void CDRenderer::eventIHandle(CIEvent *event)
{
    if(event->type==CIEvent::Keyboard){
        CIKeyEvent* kev = (CIKeyEvent*)&event[1];
        cDebug("Key event: key=%i,mods=%i,scan=%i,char=%s",
               kev->key,kev->mod,kev->scan,&kev->key);
        if(kev->key==CK_Escape)
            this->closeWindow();
    }
    if(event->type==CIEvent::Scroll){
        CIScrollEvent* sev = (CIScrollEvent*)&event[1];
        cDebug("Dist: %f, %f",sev->delta.x,sev->delta.y);
    }
    if(event->type==CIEvent::MouseButton){
        CIMouseButtonEvent* mev = (CIMouseButtonEvent*)&event[1];
        cDebug("Btn: %i:%i, %f,%f",mev->btn,mev->mod,mev->pos.x,mev->pos.x);
    }
    if(event->type==CIEvent::MouseMove&&false){
        CIMouseMoveEvent* mev = (CIMouseMoveEvent*)&event[1];
        CIMouseMoveEvent* t = (CIMouseMoveEvent*)malloc(sizeof(CIMouseMoveEvent));
        memmove(t,mev,sizeof(CIMouseMoveEvent));
        CThreading::runIndependent([=](){
            cDebug("Move: %f,%f : %f,%f",t->pos.x,t->pos.y,t->rel.x,t->rel.y);
            free(t);
        });
    }
    if(event->type==CIEvent::Drop){
        CIDropEvent* dev = (CIDropEvent*)&event[1];
        cDebug("File drop: %s",(cstring)dev->data);
    }
    if(event->type==CIEvent::TextInput){
        CIWriteEvent* w = (CIWriteEvent*)&event[1];
        cDebug("Write event: %s",w->text);
    }
    if(event->type==CIEvent::TextEdit){
        CIWEditEvent* w = (CIWEditEvent*)&event[1];
        cDebug("Edit event: %s,cur=%i,len=%i",w->text,w->cursor,w->len);
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

    glGetIntegerv(GL_VERTEX_ARRAY_BUFFER_BINDING,&t);
    state->vertex_binding = t;

    glGetFloatv(GL_COLOR_CLEAR_VALUE,reinterpret_cast<GLfloat*>(&state->clear_color));

    return state;
}

} // namespace CDisplay
} // namespace Coffee
