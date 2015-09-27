#ifndef BASE_CASE
#define BASE_CASE

#include "coffee_impl/memory/cgame_dataset.h"

namespace Coffee{
namespace CRendering{
namespace CTest{

using namespace CMemoryManagement;

struct game_context
{
    game_vertexdata_chunk   vertexdata;
    game_shader_manager     shaders;
    game_resource_chunk     resources;
    game_memory_chunk       renderdata;

    game_transform_chunk    transforms;
};

static bool coffee_test_load(game_context* ctxt)
{
    //Create multidraw object
    coffee_mem_expand_array<CMultiDrawDataSet>(&ctxt->renderdata.datasets,1);
    CMultiDrawDataSet* multidraw = &ctxt->renderdata.datasets.d[0];
    *multidraw = coffee_multidraw_create();

    //Load basic shader
    coffee_mem_expand_array<CShader>(&ctxt->shaders.shaders,2);
    coffee_mem_expand_array<CShaderProgram>(&ctxt->shaders.programs,1);
    coffee_mem_expand_array<CPipeline>(&ctxt->shaders.pipelines,1);
    {
        CResource v("ubw/shaders/vertex/vsh_instanced.vs"); //Vertex shader
        CResource f("ubw/shaders/fragment/direct/fsh_nolight.fs"); //Fragment shader
        if(!v.exists()||!f.exists())
            cFatal("Failed to locate shaders");
        v.read_data(true);
        f.read_data(true);

        CShader* vshdr = &ctxt->shaders.shaders.d[0];
        CShader* fshdr = &ctxt->shaders.shaders.d[1];
        //GL calls
        vshdr->compile(&v,GL_VERTEX_SHADER);
        fshdr->compile(&f,GL_FRAGMENT_SHADER);
        //

        v.free_data();
        f.free_data();

        CShaderProgram* p = &ctxt->shaders.programs.d[0];
        //GL calls
        p->create();
        if(p->handle==0)
            cFatal("Failed to create ShaderProgram");
        p->attachShader(vshdr,GL_VERTEX_SHADER_BIT);
        p->attachShader(fshdr,GL_FRAGMENT_SHADER_BIT);
        p->link();
        //

        CPipeline* pl = &ctxt->shaders.pipelines.d[0];
        pl->create();
        if(pl->handle==0)
            cFatal("Failed to create Pipeline");
        pl->attachProgram(p,GL_VERTEX_SHADER_BIT|GL_FRAGMENT_SHADER_BIT);

        //Try dumping it to a file, we don't care about the resource later on
        {
            CResource dmp("shader.dmp");
            //GL call
            p->storeProgram(&dmp);
            //
            if(!dmp.save_data())
                cDebug("Failed to save shader to file!");
            dmp.free_data();
        }

        /*
         *
         * Loading a shader from file:
         *
         * if(progStore.read_data())
         *     cDebug("Loaded program: %i",prog->fetchProgram(&progStore),true);
         *
         */
    }

    //Specify VAO for vertex data, we will only use one for now

    szptr numGears = 1000;

    //We have two formats
    coffee_mem_expand_array<CVertexFormat>(&ctxt->vertexdata.descriptor.formats,2);
    coffee_mem_expand_array<CVertexBufferBinding>(&ctxt->vertexdata.descriptor.bindings,1);
    coffee_mem_expand_array<CBuffer>(&ctxt->renderdata.buffers,5);
    coffee_mem_expand_array<CVertexArrayObject>(&ctxt->vertexdata.descriptor.arrays,1);
    {
        CBuffer* vbuffer = &ctxt->renderdata.buffers.d[0];
        CBuffer* mbuffer = &ctxt->renderdata.buffers.d[2];

        mbuffer->bufferType = GL_ARRAY_BUFFER;
        mbuffer->flags = GL_MAP_COHERENT_BIT|GL_MAP_PERSISTENT_BIT|GL_MAP_WRITE_BIT;
        mbuffer->create();
        mbuffer->bind();
        mbuffer->store(sizeof(glm::mat4)*numGears,nullptr);
        mbuffer->unbind();
        mbuffer->map(GL_MAP_PERSISTENT_BIT|GL_MAP_WRITE_BIT);

        vbuffer->create();

        {
            CBuffer* ibuffer = &ctxt->renderdata.buffers.d[1];
            CBuffer* dbuffer = &ctxt->renderdata.buffers.d[3];
            multidraw->drawcalls->drawbuffer = dbuffer;

            multidraw->index->buffer = ibuffer;

            CVertexArrayObject* vao = &ctxt->vertexdata.descriptor.arrays.d[0];
            multidraw->vao = vao;

            vao->create();
            dbuffer->create();

            ibuffer->create();
            ibuffer->bufferType = GL_ELEMENT_ARRAY_BUFFER;
        }

        CVertexFormat* stdFmt = &ctxt->vertexdata.descriptor.formats.d[0];
        stdFmt->normalized = GL_FALSE;
        stdFmt->offset = 0;
        stdFmt->size = 3;
        stdFmt->type = GL_FLOAT;

        CVertexFormat* matFmt = &ctxt->vertexdata.descriptor.formats.d[1];
        matFmt->normalized = GL_FALSE;
        matFmt->offset = 0;
        matFmt->size = 4;
        matFmt->type = GL_FLOAT;

        CVertexBufferBinding* posBnd = &ctxt->vertexdata.descriptor.bindings.d[0];
        posBnd->binding = 0;
        posBnd->buffer = vbuffer;
        posBnd->divisor = 0;
        posBnd->offset = 0;
        posBnd->stride = sizeof(CVec3);

        CVertexAttribute posAtt;
        posAtt.attribIdx = 0;
        posAtt.bnd = posBnd;
        posAtt.fmt = stdFmt;

        CMultiDrawDescriptor desc;

        desc.attributes.push_back(posAtt);

        //GL calls
        coffee_mesh_define_matrix_attribs(mbuffer,*matFmt,desc,5,1);
        coffee_multidraw_load_vao(*multidraw,desc);
        //
    }

    //Load mesh
    coffee_mem_expand_array<CAssimpData*>(&ctxt->vertexdata.data,1);
    {
        CResource m("ubw/models/ubw.blend");
        if(!m.exists())
            cFatal("Failed to locate scene");
        m.read_data();
        CAssimpData* d = CAssimpImporters::importResource(&m,"blend");
        m.free_data();

        //Store a pointer to the data for freeing later
        ctxt->vertexdata.data.d[0] = d;

        if(d->numMeshes<5)
            cFatal("Failed to load mesh data! File does not match!");

        CBuffer* vbuffer = &ctxt->renderdata.buffers.d[0];

        //Temporary storage for data
        std::vector<byte> *vertexdata = new std::vector<byte>();

        std::function<void(CAssimpMesh*)> lmesh = [=](CAssimpMesh* mesh){
            for(int i=0;i<mesh->numBuffers;i++)
                if(mesh->bufferType[i]==CAssimpMesh::PositionType){
                    coffee_mesh_fill_vertexdata(*vertexdata,mesh->buffers[i],
                                                0,mesh->bufferSize[i]*sizeof(CVec3));
                }
            //Store indices and create the drawcall
            coffee_multidraw_create_call(*multidraw,mesh);
        };

        //Future improvement: Do this in parallel with reserved memory chunks
        lmesh(d->meshes[4]);
//        lmesh(d->meshes[1]);
        lmesh(d->meshes[3]);

        multidraw->drawcalls->drawcalls.data()[0].instanceCount = numGears-2;

        //GL calls
        coffee_multidraw_load_buffer(vbuffer,*vertexdata);
        coffee_multidraw_load_drawcalls(*multidraw);
        coffee_multidraw_load_indices(*multidraw);
        //
        vertexdata->resize(0);
        delete vertexdata;
        d->freeData();
        delete d;
    }

    //Define instance matrices and camera matrix
    coffee_mem_expand_array<CGCamera>(&ctxt->transforms.cameras,1);
    coffee_mem_expand_array<CModelTransform>(&ctxt->transforms.transforms,1);
    coffee_mem_expand_array<CSubBuffer>(&ctxt->renderdata.subbuffers,2);
    coffee_mem_expand_array<CUniformChunk>(&ctxt->renderdata.uniformchunks,2);
    {
        {
            CGCamera* cam = &ctxt->transforms.cameras.d[0];
            cam->fieldOfView = 120.f;
            cam->aspect = 1.6f;
            cam->zVals.far = 100.f;
            cam->zVals.near = 0.1f;
            cam->genPerspective();

            CBuffer* ubuffer = &ctxt->renderdata.buffers.d[4];
            ubuffer->bufferType = GL_UNIFORM_BUFFER;
            ubuffer->flags = GL_MAP_COHERENT_BIT|GL_MAP_PERSISTENT_BIT|GL_MAP_WRITE_BIT;


            CSubBuffer* camBuffer = &ctxt->renderdata.subbuffers.d[0];
            camBuffer->parent = ubuffer;
            camBuffer->size = sizeof(glm::mat4)*2;
            camBuffer->bufferType = GL_UNIFORM_BUFFER;

            CSubBuffer* matBuffer = &ctxt->renderdata.subbuffers.d[1];
            matBuffer->bufferType = GL_SHADER_STORAGE_BUFFER;
            matBuffer->parent = ubuffer;
            matBuffer->size = sizeof(GLuint)*numGears+sizeof(glm::vec3)*numGears;
            matBuffer->offset = camBuffer->size;

            //GL calls
            ubuffer->create();
            ubuffer->bind();
            ubuffer->store(matBuffer->size+camBuffer->size,nullptr);
            ubuffer->unbind();
            ubuffer->map(GL_MAP_PERSISTENT_BIT|GL_MAP_WRITE_BIT);
            //

            {
                szptr matrixSz[2] = {sizeof(glm::mat4),sizeof(glm::mat4)};
                CUniformChunk* uchunk =
                        coffee_create_uchunk(
                            camBuffer,
                            camBuffer->size,
                            2,
                            matrixSz,
                            "MatrixBlock");
                uchunk = (CUniformChunk*)memmove(
                            &ctxt->renderdata.uniformchunks.d[0],
                        uchunk,sizeof(CUniformChunk));
                memcpy(uchunk->buffer->parent->data,&cam->matrix,sizeof(glm::mat4));
                uchunk->ublock.blockBinding = 0;
                //GL calls
                uchunk->ublock.shaderIndex = glGetUniformBlockIndex(
                            ctxt->shaders.programs.d[0].handle,
                        uchunk->ublock.name);
                glUniformBlockBinding(
                            ctxt->shaders.programs.d[0].handle,
                        uchunk->ublock.shaderIndex,
                        uchunk->ublock.blockBinding);
                //
            }

//            {
//                CUniformChunk* uchunk =
//                        coffee_create_uchunk(
//                            matBuffer,
//                            matBuffer->size,
//                            2,
//                            nullptr,
//                            "material_data");
//                uchunk = (CUniformChunk*)memmove(
//                            &ctxt->renderdata.uniformchunks.d[1],
//                        uchunk,sizeof(CUniformChunk));

//                memcpy(uchunk->buffer->parent->data,&cam->matrix,sizeof(glm::mat4));

//                uchunk->ublock.blockBinding = 1;
//                //GL calls
//                uchunk->ublock.shaderIndex = glGetProgramResourceIndex(
//                            ctxt->shaders.programs.d[0].handle,
//                        GL_SHADER_STORAGE_BLOCK,
//                        uchunk->ublock.name);
//                glShaderStorageBlockBinding(
//                            ctxt->shaders.programs.d[0].handle,
//                        uchunk->ublock.shaderIndex,
//                        uchunk->ublock.blockBinding);

//                //
//            }
        }

        {
            CModelTransform* mod = &ctxt->transforms.transforms.d[0];
            mod->position.z = -1.f;
            mod->scale.x = mod->scale.y = mod->scale.z = 1.f;
            mod->rotation.w = 2.f;

            std::vector<glm::mat4> transform;
            transform.reserve(numGears);
            for(szptr i=1;i<numGears;i++){
                mod->position.z = (float)(-i);
                mod->position.y = (float)((i%11)-5);
                mod->genMatrix();
                transform.push_back(mod->matrix);
            }

            mod->position.x = mod->position.y = 0.f;
            mod->position.z = -1.f;
            CBuffer* mbuffer = &ctxt->renderdata.buffers.d[2];

            memcpy(mbuffer->data,
                   transform.data(),
                   transform.size()*sizeof(transform.data()[0]));
            transform.resize(0);
        }
    }
    return true;
}

static void coffee_prepare_test(game_context* ctxt)
{
    coffee_multidraw_bind_states(ctxt->renderdata.datasets.d[0]);
    ctxt->renderdata.buffers.d[2].bind();
    ctxt->renderdata.uniformchunks.d[0].buffer->bindRange();
    ctxt->shaders.pipelines.d[0].bind();
}

static void coffee_render_test(game_context* ctxt, double delta)
{
    glClear(GL_COLOR_BUFFER_BIT);

    //Generate our matrices
    ctxt->transforms.transforms.d[0].genMatrix();
    ctxt->transforms.cameras.d[0].genPerspective();

    //Copy memory into GL
    memcpy(ctxt->renderdata.buffers.d[4].data,
            &ctxt->transforms.cameras.d[0].matrix,
            sizeof(glm::mat4));
    memcpy(ctxt->renderdata.buffers.d[2].data,
            &ctxt->transforms.transforms.d[0].matrix,
            sizeof(glm::mat4));

    //Send it off
    coffee_multidraw_render(ctxt->renderdata.datasets.d[0]);
    glFlush();
}

static void coffee_unload_test(game_context* ctxt)
{
    ctxt->renderdata.buffers.d[2].unbind();
    ctxt->renderdata.uniformchunks.d[0].buffer->unbind();
}

}
}
}

#endif // BASE_CASE

