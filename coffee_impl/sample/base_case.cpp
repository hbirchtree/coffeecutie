#include "base_case.h"

#include "coffee_impl/graphics/cgraphics_quirks.h"

namespace Coffee{
namespace CRendering{
namespace CTest{

CPipeline *coffee_shader_program_load(const game_shader_program_desc &desc, game_context *ctxt)
{
    szptr pl_idx = ctxt->shaders.pipelines.size;
    szptr pr_idx = ctxt->shaders.shaders.size;
    szptr sh_idx = ctxt->shaders.programs.size;

    coffee_mem_expand_array<CShader>(&ctxt->shaders.shaders,2);
    coffee_mem_expand_array<CShaderProgram>(&ctxt->shaders.programs,1);
    coffee_mem_expand_array<CPipeline>(&ctxt->shaders.pipelines,1);

    CPipeline* pl = &ctxt->shaders.pipelines.d[pl_idx];
    CShaderProgram* p = &ctxt->shaders.programs.d[pr_idx];
    CShader* vshdr = &ctxt->shaders.shaders.d[sh_idx];
    CShader* fshdr = &ctxt->shaders.shaders.d[sh_idx+1];

    p->stages = GL_VERTEX_SHADER_BIT|GL_FRAGMENT_SHADER_BIT;

    CResource dmp(desc.shader_dump);

    //    if(dmp.exists())
    //    {
    //        dmp.read_data();
    //        if(p->fetchProgram(&dmp)){
    //            dmp.free_data();
    //            pl->attachProgram(p,p->stages);
    //            cDebug("Shader loaded from file");
    //            return pl;
    //        }
    //    }

    CResource v(desc.shader_v); //Vertex shader
    CResource f(desc.shader_f); //Fragment shader
    if(!v.exists()||!f.exists())
        cFatal("Failed to locate shaders");
    v.read_data(true);
    f.read_data(true);

    //GL calls
    vshdr->compile(&v,GL_VERTEX_SHADER);
    fshdr->compile(&f,GL_FRAGMENT_SHADER);
    //

    v.free_data();
    f.free_data();

    //GL calls
    p->create();
    if(p->handle==0)
        cFatal("Failed to create ShaderProgram");
    p->attachShader(vshdr,GL_VERTEX_SHADER_BIT);
    p->attachShader(fshdr,GL_FRAGMENT_SHADER_BIT);
    p->link();
    //

    pl->create();
    if(pl->handle==0)
        cFatal("Failed to create Pipeline");
    pl->attachProgram(p,GL_VERTEX_SHADER_BIT|GL_FRAGMENT_SHADER_BIT);

    //GL call
    p->storeProgram(&dmp);
    //
    if(!dmp.save_data())
        cDebug("Failed to save shader to file!");
    dmp.free_data();

    return pl;
}

CTexture *coffee_texture_2d_load(CResource *textureres, game_context *ctxt)
{
    coffee_mem_expand_array<CTexture>(&ctxt->texstorage,1);
    CTexture *tex = &ctxt->texstorage.d[ctxt->texstorage.size-1];
    //GL call
    tex->create();
    //

    CStbImageLib::CStbImage img;
    CStbImageLib::coffee_stb_image_load(&img,textureres);

    CTextureTools::CTextureData dt;
    dt.data = img.data;
    dt.datatype = GL_UNSIGNED_BYTE;
    dt.format = GL_RGBA8;

    CTextureTools::coffee_create_texturesize(&dt,img.size.w,img.size.h);

    tex->textureType = GL_TEXTURE_2D;
    tex->levels = 1;
    tex->format = GL_RGBA;
    //GL calls
    tex->bind();
    tex->unbind();
    CTextureTools::coffee_texture2d_define(tex,&dt);
    CTextureTools::coffee_texture2d_store(tex,&dt,0);
    //

    return tex;
}

CTexture *coffee_texture_2d_load_blam(const CBlam::blam_bitm_image *text, const CBlam::blam_file_header *head, int32 magic, game_context *ctxt)
{
    coffee_mem_expand_array<CTexture>(&ctxt->texstorage,1);
    CTexture *tex = &ctxt->texstorage.d[ctxt->texstorage.size-1];
    //GL call
    tex->create();
    //

    uint32* d = CBlam::coffee_bitm_decode_a8r8g8b8(text,head);

    CTextureTools::CTextureData dt;
    dt.data = d;
    dt.datatype = GL_UNSIGNED_BYTE;
    dt.format = GL_RGBA8;

    CTextureTools::coffee_create_texturesize(&dt,text->isize.w,text->isize.h);

    tex->textureType = GL_TEXTURE_2D;
    tex->levels = 1;
    tex->format = GL_RGBA;
    //GL calls
    tex->bind();
    tex->unbind();
    CTextureTools::coffee_texture2d_define(tex,&dt);
    CTextureTools::coffee_texture2d_store(tex,&dt,0);
    //

    return tex;
}

bool coffee_test_load(game_context *ctxt)
{
    //Create multidraw object
    coffee_mem_expand_array<CMultiDrawDataSet>(&ctxt->renderdata.datasets,1);
    CMultiDrawDataSet* multidraw = &ctxt->renderdata.datasets.d[0];
    *multidraw = coffee_multidraw_create();

    //Load basic shader
    {
        game_shader_program_desc d;
        d.shader_dump = "basic_dump.shdr";
        d.shader_f = "ubw/shaders/fragment/direct/fsh_nolight_ssbo.fs";
        d.shader_v = "ubw/shaders/vertex/vsh_instanced_ssbo.vs";

        coffee_shader_program_load(d,ctxt);
    }

    //Specify VAO for vertex data, we will only use one for now

    szptr numGears = 100;

    //We have two formats
    coffee_mem_expand_array<CVertexFormat>(&ctxt->vertexdata.descriptor.formats,3);
    coffee_mem_expand_array<CVertexBufferBinding>(&ctxt->vertexdata.descriptor.bindings,2);
    coffee_mem_expand_array<CBuffer>(&ctxt->renderdata.buffers,7);
    coffee_mem_expand_array<CVertexArrayObject>(&ctxt->vertexdata.descriptor.arrays,1);
    {
        CBuffer* vbuffer = &ctxt->renderdata.buffers.d[0];
        CBuffer* tbuffer = &ctxt->renderdata.buffers.d[6];
        CBuffer* mbuffer = &ctxt->renderdata.buffers.d[2];

        mbuffer->bufferType = GL_ARRAY_BUFFER;
        mbuffer->flags = GL_MAP_COHERENT_BIT|GL_MAP_PERSISTENT_BIT|GL_MAP_WRITE_BIT;
        //GL calls
        mbuffer->create();
        mbuffer->bind();
        mbuffer->store(sizeof(CMath::mat4)*numGears,nullptr);
        mbuffer->unbind();
        mbuffer->map(GL_MAP_PERSISTENT_BIT|GL_MAP_WRITE_BIT);

        vbuffer->create();
        tbuffer->create();

        {
            CBuffer* sbuffer = &ctxt->renderdata.buffers.d[5];
            sbuffer->bufferType = GL_SHADER_STORAGE_BUFFER;
            sbuffer->create();

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
        //

        CVertexFormat* stdFmt = &ctxt->vertexdata.descriptor.formats.d[0];
        stdFmt->normalized = GL_FALSE;
        stdFmt->offset = 0;
        stdFmt->size = 3;
        stdFmt->type = GL_FLOAT;

        CVertexFormat* texFmt = &ctxt->vertexdata.descriptor.formats.d[2];
        texFmt->normalized = GL_FALSE;
        texFmt->offset = 0;
        texFmt->size = 2;
        texFmt->type = GL_FLOAT;

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

        CVertexBufferBinding* texBnd = &ctxt->vertexdata.descriptor.bindings.d[1];
        texBnd->binding = 1;
        texBnd->buffer = tbuffer;
        texBnd->divisor = 0;
        texBnd->offset = 0;
        texBnd->stride = sizeof(CVec2);

        CVertexAttribute texAtt;
        texAtt.attribIdx = 1;
        texAtt.bnd = texBnd;
        texAtt.fmt = texFmt;

        CVertexAttribute posAtt;
        posAtt.attribIdx = 0;
        posAtt.bnd = posBnd;
        posAtt.fmt = stdFmt;

        CMultiDrawDescriptor desc;

        desc.attributes.push_back(posAtt);
        desc.attributes.push_back(texAtt);

        //GL calls
        coffee_mesh_define_matrix_attribs(mbuffer,*matFmt,desc,5,2);
        coffee_multidraw_load_vao(*multidraw,desc);
        //
    }

    //Load mesh
    {
        CBuffer* vbuffer = &ctxt->renderdata.buffers.d[0];
        CBuffer* tbuffer = &ctxt->renderdata.buffers.d[6];

        //Temporary storage for data
        std::vector<byte> *vertexdata = new std::vector<byte>();
        std::vector<byte> *texcdata = new std::vector<byte>();

        std::function<void(CAssimpMesh*)> lmesh = [=](CAssimpMesh* mesh){

            CResource meshtest(cStringFormat("ubw/%s.mesh",mesh->name).c_str());
            coffee_dump_mesh(mesh,&meshtest);

            szptr voffset = 0;
            for(int i=0;i<mesh->numBuffers;i++){
                if(mesh->bufferType[i]==CAssimpMesh::PositionType)
                {
                    coffee_mesh_fill_vertexdata(*vertexdata,mesh->buffers[i],
                                                0,mesh->bufferSize[i]*sizeof(CVec3));
                    voffset=mesh->bufferSize[i];
                }
                if(mesh->bufferType[i]==CAssimpMesh::TextCoordType)
                    coffee_mesh_fill_vertexdata(*texcdata,mesh->buffers[i],
                                                0,mesh->bufferSize[i]*sizeof(CVec2));
            }
            //Store indices and create the drawcall
            coffee_multidraw_create_call(*multidraw,mesh);
            multidraw->drawcalls->vertexoffset = voffset;
        };

        //Future improvement: Do this in parallel with reserved memory chunks
        szptr i = 0;
        for(CAssimpMesh* mesh : ctxt->meshes)
        {
            lmesh(mesh);
            ctxt->mesh_indices.push_back(i);
            i++;
        }

        //GL calls
        coffee_multidraw_load_buffer(vbuffer,*vertexdata);
        coffee_multidraw_load_buffer(tbuffer,*texcdata);

        if(ctxt->features->render_multidraw)
            coffee_multidraw_load_drawcalls(*multidraw);
        coffee_multidraw_load_indices(*multidraw);
        //
        vertexdata->resize(0);
        delete vertexdata;
        delete texcdata;
    }

    //Define instance matrices and camera matrix
    coffee_mem_expand_array<CGCamera>(&ctxt->transforms.cameras,1);
    coffee_mem_expand_array<CModelTransform>(&ctxt->transforms.transforms,1);
    coffee_mem_expand_array<CSubBuffer>(&ctxt->renderdata.subbuffers,2);
    coffee_mem_expand_array<CUniformBlock>(&ctxt->renderdata.uniformblocks,2);
    {
        {
            CGCamera* cam = &ctxt->transforms.cameras.d[0];
            cam->fieldOfView = 60.f;
            cam->aspect = 1.6f;
            cam->zVals.far = 100.f;
            cam->zVals.near = 0.1f;
            cam->genPerspective();

            CBuffer* ubuffer = &ctxt->renderdata.buffers.d[4];
            ubuffer->bufferType = GL_UNIFORM_BUFFER;
            ubuffer->flags = GL_MAP_COHERENT_BIT|GL_MAP_PERSISTENT_BIT|GL_MAP_WRITE_BIT;


            CSubBuffer* camBuffer = &ctxt->renderdata.subbuffers.d[0];
            camBuffer->parent = ubuffer;
            camBuffer->offset = 0;
            camBuffer->size = sizeof(CMath::mat4)*2;
            camBuffer->bufferType = GL_UNIFORM_BUFFER;

            CSubBuffer* matBuffer = &ctxt->renderdata.subbuffers.d[1];
            matBuffer->bufferType = GL_SHADER_STORAGE_BUFFER;
            matBuffer->parent = ubuffer;
            matBuffer->size = sizeof(GLuint)*numGears+sizeof(CMath::vec3)*numGears;
            matBuffer->offset = camBuffer->size;

            //GL calls
            ubuffer->create();
            ubuffer->bind();
            ubuffer->store(matBuffer->size+camBuffer->size,nullptr);
            ubuffer->unbind();
            ubuffer->map(GL_MAP_PERSISTENT_BIT|GL_MAP_WRITE_BIT);
            //

            {
                CUniformBlock *ublock = &ctxt->renderdata.uniformblocks.d[0];
                ublock->buffer = camBuffer;
                ublock->name = "MatrixBlock";
                memcpy(camBuffer->data(),&cam->matrix,sizeof(CMath::mat4));
                ublock->blockBinding = 0;
                //GL calls
                ctxt->shaders.programs.d[0].uniformBlockIndex(ublock);
                ctxt->shaders.programs.d[0].setUniformBlockBind(*ublock);
                //
            }
        }

        {
            const CBlam::blam_bitm_image* img_t = nullptr;

            CResources::CResource mapfile("/home/havard/.local/share/winedata/Halo/Maps/Halo Combat Evolved/ratrace.map");
            mapfile.memory_map();
            const CBlam::blam_file_header* map =
                    CBlam::blam_file_header_get(mapfile.data,CBlam::blam_version_pc);
            CBlam::blam_tag_index tags = CBlam::blam_tag_index_get(map);

            const CBlam::blam_index_item* base_idx = CBlam::blam_tag_meta_offset(map);
            const CBlam::blam_index_item* idx = nullptr;
            for(int32 i=0;i<tags.tagCount;i++)
            {
                idx = &base_idx[i];
                //        cDebug("Index: %s",
                //               CBlam::blam_index_item_get_string(idx,map,&tags));
                if(coffee_cmp_memarea(idx->tagclass[0],CBlam::blam_index_item_type_bitm,4))
                {
                    int32 num = 0;
                    //                    cDebug("Bitmap: %s",CBlam::blam_index_item_get_string(idx,map,&tags));
                    const CBlam::blam_bitm_image* img =
                            CBlam::coffee_bitm_get(idx,map,tags.index_magic,&num);
                    //                    cDebug("Image: %i: %ix%i@%i, %i,%i,%i",
                    //                           num,
                    //                           img->width,img->height,img->depth,
                    //                           img->format,img->mipmaps,img->size);
                    if(img->isize.w==img->isize.h&&
                            img->format==CBlam::blam_bitm_format_A8R8G8B8)
                    {
                        cstring t = CBlam::blam_index_item_get_string(idx,map,&tags);
                        cDebug("Image: %s,d=%i",t,img->depth);
                        img_t = img;
                    }
                }
            }

            //            CStbImageLib::CStbImageConst img;
            //            img.data = ((const ubyte*)map)+img_t->offset;
            //            img.bpp = 1;
            //            img.size.w = img_t->isize.w;
            //            img.size.h = img_t->isize.h;
            //            CResource res("test.png");

            //            CStbImageLib::coffee_stb_image_save_png(&res,&img);
            //            CStbImageLib::coffee_stb_error();

            //            res.save_data();

            coffee_texture_2d_load_blam(img_t,map,tags.index_magic,ctxt);
        }

        {
            CModelTransform* mod = &ctxt->transforms.transforms.d[0];
            mod->position.z = -1.f;
            mod->scale.x = mod->scale.y = mod->scale.z = 1.f;
            mod->rotation.w = 2.f;

            mod->position.x = 5.f;

            std::vector<CMath::mat4> transform;
            transform.reserve(numGears);
            for(szptr i=1;i<numGears-2;i++){
                mod->position.z = (float)(-i);
                mod->position.y = (float)((i%11)/2);
                mod->genMatrix();
                transform.push_back(mod->matrix);
            }

            mod->position.x = mod->position.y = 0.f;
            mod->position.z = -1.f;

            mod->genMatrix();
            CBuffer* mbuffer = &ctxt->renderdata.buffers.d[2];

            transform.push_back(mod->matrix);
            transform.push_back(mod->matrix);

            memcpy(mbuffer->data,
                   transform.data(),
                   transform.size()*sizeof(transform.data()[0]));
            transform.resize(0);
        }
    }
    return true;
}

void coffee_prepare_test(game_context *ctxt)
{
    coffee_multidraw_bind_states(ctxt->renderdata.datasets.d[0]);
    ctxt->renderdata.buffers.d[2].bind();
    ctxt->renderdata.uniformblocks.d[0].buffer->bindRange();
    ctxt->shaders.pipelines.d[0].bind();
    ctxt->renderdata.datasets.d[0].vao->bind();
    ctxt->renderdata.datasets.d[0].drawcalls->drawbuffer->bind();

    GLint loc = ctxt->shaders.programs.d[0].uniformLocation("diffuseSampler");
    GLuint64 handle = coffee_tex_get_handle(&ctxt->texstorage.d[0]);
    coffee_tex_make_resident(handle);
    glProgramUniformHandleui64ARB(ctxt->shaders.programs.d[0].handle,loc,handle);
}

void coffee_render_test(game_context *ctxt, double delta)
{
    //Generate our matrices
    ctxt->transforms.transforms.d[0].genMatrix();
    ctxt->transforms.cameras.d[0].genPerspective();

    //Copy memory into GL
    memcpy(ctxt->renderdata.buffers.d[4].data,
            &ctxt->transforms.cameras.d[0].matrix,
            sizeof(CMath::mat4));
    memcpy(ctxt->renderdata.buffers.d[2].data,
            &ctxt->transforms.transforms.d[0].matrix,
            sizeof(CMath::mat4));

    //Send it off
    ctxt->renderfun(ctxt->renderdata.datasets.d[0]);
}

void coffee_unload_test(game_context *ctxt)
{
    ctxt->renderdata.buffers.d[2].unbind();
    //    ctxt->renderdata.uniformblocks.d[2].buffer->unbind();

    szptr i;

    for(i=0;i<ctxt->renderdata.buffers.size;i++)
    {
        ctxt->renderdata.buffers.d[i].unmap();
        ctxt->renderdata.buffers.d[i].free();
    }

    for(i=0;i<ctxt->vertexdata.descriptor.arrays.size;i++)
        ctxt->vertexdata.descriptor.arrays.d[i].free();
    for(i=0;i<ctxt->vertexdata.data.size;i++)
        ctxt->vertexdata.data.d[i]->freeData();

    for(i=0;i<ctxt->shaders.pipelines.size;i++)
        ctxt->shaders.pipelines.d[i].free();
    for(i=0;i<ctxt->shaders.shaders.size;i++)
        ctxt->shaders.shaders.d[i].free();
    for(i=0;i<ctxt->shaders.programs.size;i++)
        ctxt->shaders.programs.d[i].free();

    for(i=0;i<ctxt->renderdata.datasets.size;i++)
        coffee_multidraw_free(&ctxt->renderdata.datasets.d[i]);

    free(ctxt->vertexdata.buffers.d);
    free(ctxt->vertexdata.data.d);
    free(ctxt->vertexdata.descriptor.arrays.d);
    free(ctxt->vertexdata.descriptor.bindings.d);
    free(ctxt->vertexdata.descriptor.formats.d);

    free(ctxt->renderdata.buffers.d);
    free(ctxt->renderdata.datasets.d);
    free(ctxt->renderdata.subbuffers.d);
    free(ctxt->renderdata.uniformblocks.d);

    free(ctxt->resources.d);

    free(ctxt->shaders.pipelines.d);
    free(ctxt->shaders.programs.d);
    free(ctxt->shaders.shaders.d);

    free(ctxt->transforms.cameras.d);
    free(ctxt->transforms.transforms.d);
}

}
}
}
