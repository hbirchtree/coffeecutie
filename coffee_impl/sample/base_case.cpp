#include "base_case.h"

#include "coffee_impl/graphics/cgraphics_quirks.h"
#include "coffee_impl/assimp/assimpfun.h"
#include "coffee_impl/graphics/ctexture_dxtc.h"
#include "datasources/blam/cblam_bitm.h"

namespace Coffee{
namespace CRendering{
namespace CTest{

void coffee_test_fun_set(game_context *ctxt)
{
    if(ctxt->features->ext_multi_draw_indirect)
        ctxt->funptrs.renderfun = coffee_multidraw_render;
    else
        ctxt->funptrs.renderfun = coffee_multidraw_render_safe;

    if(ctxt->features->ext_bindless_texture)
    {
        ctxt->funptrs.textures.load = coffee_graphics_tex_use;
        ctxt->funptrs.textures.unload = coffee_graphics_tex_unload;
    }else{
        ctxt->funptrs.textures.load = coffee_graphics_tex_use_safe;
        ctxt->funptrs.textures.unload = coffee_graphics_tex_unload_safe;
    }

    if(ctxt->features->ext_texture_storage)
    {
        ctxt->funptrs.textures.define = CTextureTools::coffee_graphics_tex_define;
        ctxt->funptrs.textures.store = CTextureTools::coffee_graphics_tex_store;
    }else{
        ctxt->funptrs.textures.define = CTextureTools::coffee_graphics_tex_define_safe;
        ctxt->funptrs.textures.store = CTextureTools::coffee_graphics_tex_store_safe;
    }

    if(ctxt->features->ext_direct_state_access)
    {
        ctxt->funptrs.buffers.copy = coffee_graphics_buffer_copy;
        ctxt->funptrs.buffers.download = coffee_graphics_buffer_download_buffer;

        ctxt->funptrs.buffers.store = coffee_graphics_buffer_store;
        ctxt->funptrs.buffers.store_immutable = coffee_graphics_buffer_store_immutable;
        ctxt->funptrs.buffers.substore = coffee_graphics_buffer_substore;

        ctxt->funptrs.buffers.map = coffee_graphics_buffer_map;
        ctxt->funptrs.buffers.unmap = coffee_graphics_buffer_unmap;

    }else{
        ctxt->funptrs.buffers.copy = coffee_graphics_buffer_copy_safe;
        ctxt->funptrs.buffers.download = coffee_graphics_buffer_download_buffer_safe;

        ctxt->funptrs.buffers.store = coffee_graphics_buffer_store_safe;
        ctxt->funptrs.buffers.store_immutable = coffee_graphics_buffer_store_immutable_safe;
        ctxt->funptrs.buffers.substore = coffee_graphics_buffer_substore_safe;

        ctxt->funptrs.buffers.map = coffee_graphics_buffer_map_safe;
        ctxt->funptrs.buffers.unmap = coffee_graphics_buffer_unmap_safe;
    }

    ctxt->funptrs.buffers.subdata = coffee_graphics_buffer_sub_data;

    if(ctxt->features->ext_invalidate_subdata)
    {
        ctxt->funptrs.buffers.invalidate = coffee_graphics_buffer_invalidate;
    }else{
        ctxt->funptrs.buffers.invalidate = coffee_graphics_buffer_invalidate_safe;
    }
}

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
    coffee_graphics_shader_compile(vshdr,&v,GL_VERTEX_SHADER,GL_VERTEX_SHADER_BIT);
    coffee_graphics_shader_compile(fshdr,&f,GL_FRAGMENT_SHADER,GL_FRAGMENT_SHADER_BIT);
    //

    v.free_data();
    f.free_data();

    //GL calls separable programs
    coffee_graphics_alloc(p,true);
    if(p->handle==0)
        cFatal("Failed to create ShaderProgram");
    coffee_graphics_shader_attach(p,vshdr);
    coffee_graphics_shader_attach(p,fshdr);
    coffee_graphics_shader_link(p);
    //

    coffee_graphics_alloc(pl);
    if(pl->handle==0)
        cFatal("Failed to create Pipeline");
    coffee_graphics_shader_attach(pl,p,p->stages);

    //GL call
    coffee_graphics_store(p,&dmp);
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
    coffee_graphics_alloc(tex);
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
    coffee_graphics_bind(tex);
    coffee_graphics_unbind(tex);
    ctxt->funptrs.textures.define(tex,&dt);
    ctxt->funptrs.textures.store(tex,&dt,0);
    //
    CTextureTools::coffee_graphics_tex_free_texdata(&dt);
    CStbImageLib::coffee_stb_image_free(&img);

    return tex;
}

CTexture* coffee_texture_2d_load(const CBlam::blam_bitm_texture_def& tex, game_context* ctxt)
{
    coffee_mem_expand_array<CTexture>(&ctxt->texstorage,1);
    CTexture* t = &ctxt->texstorage.d[ctxt->texstorage.size-1];

    switch(tex.format)
    {
    case CBlam::blam_bitm_tex_RGBA:{
        CTextureTools::CTextureData dt;
        dt.data = tex.data;
        dt.datatype = GL_UNSIGNED_BYTE;
        dt.format = GL_RGBA8;
        CTextureTools::coffee_create_texturesize(&dt,tex.resolution.w,tex.resolution.h);

        t->textureType = (tex.type==CBlam::blam_bitm_type_2D) ? GL_TEXTURE_2D : GL_TEXTURE_3D;
        t->levels = tex.mipmaps;
        t->format = GL_RGBA;
        coffee_graphics_tex_activate(t);
        ctxt->funptrs.textures.define(t,&dt);
        ctxt->funptrs.textures.store(t,&dt,0);
        break;
    }
    }

    return t;
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

        if(!ctxt->features->ext_ssbo_support)
        {
            d.shader_f = "ubw/shaders/fragment/direct/fsh_nolight.fs";
            d.shader_v = "ubw/shaders/vertex/vsh_instanced.vs";
        }

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
        coffee_graphics_alloc(mbuffer);
        coffee_graphics_activate(mbuffer);
        ctxt->funptrs.buffers.store_immutable(
                    mbuffer,nullptr,sizeof(CMath::mat4)*numGears,
                    mbuffer->flags);
        ctxt->funptrs.buffers.map(mbuffer,GL_MAP_PERSISTENT_BIT|GL_MAP_WRITE_BIT);

        coffee_graphics_alloc(vbuffer);
        coffee_graphics_alloc(tbuffer);

        {
            CBuffer* sbuffer = &ctxt->renderdata.buffers.d[5];
            sbuffer->bufferType = GL_SHADER_STORAGE_BUFFER;
            coffee_graphics_alloc(sbuffer);

            CBuffer* ibuffer = &ctxt->renderdata.buffers.d[1];
            CBuffer* dbuffer = &ctxt->renderdata.buffers.d[3];
            multidraw->drawcalls->drawbuffer = dbuffer;

            multidraw->index->buffer = ibuffer;

            CVertexArrayObject* vao = &ctxt->vertexdata.descriptor.arrays.d[0];
            multidraw->vao = vao;

            coffee_graphics_alloc(vao);
            coffee_graphics_alloc(dbuffer);

            coffee_graphics_alloc(ibuffer);
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
            CResourceTypes::coffee_assimp_dump_mesh(mesh,&meshtest);

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

        szptr i = 0;
        for(CAssimpMesh* mesh : ctxt->meshes)
        {
            lmesh(mesh);
            ctxt->mesh_indices.push_back(i);
            i++;
        }

        //GL calls
        coffee_multidraw_load_buffer(vbuffer,*vertexdata,ctxt->funptrs.buffers);
        coffee_multidraw_load_buffer(tbuffer,*texcdata,ctxt->funptrs.buffers);

        if(ctxt->features->ext_multi_draw_indirect)
            coffee_multidraw_load_drawcalls(*multidraw,ctxt->funptrs.buffers);
        coffee_multidraw_load_indices(*multidraw,ctxt->funptrs.buffers);
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
            coffee_graphics_gen_matrix_perspective(cam);

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
            coffee_graphics_alloc(ubuffer);
            ctxt->funptrs.buffers.store_immutable(
                        ubuffer,nullptr,
                        matBuffer->size+camBuffer->size,ubuffer->flags);
            ctxt->funptrs.buffers.map(ubuffer,GL_MAP_PERSISTENT_BIT|GL_MAP_WRITE_BIT);
            //

            {
                CUniformBlock *ublock = &ctxt->renderdata.uniformblocks.d[0];
                ublock->buffer = camBuffer;
                ublock->name = "MatrixBlock";
                void* data = ctxt->funptrs.buffers.subdata(camBuffer);
                memcpy(data,&cam->matrix,sizeof(CMath::mat4));
                ublock->blockBinding = 0;
                //GL calls
                coffee_graphics_shader_uniform_block_get(
                            &ctxt->shaders.programs.d[0],
                        ublock->name,
                        &ublock->shaderIndex);
                coffee_graphics_shader_uniform_block_set(
                            &ctxt->shaders.programs.d[0],
                        *ublock);
                //
            }
        }

        {
            CResources::CResource mapfile("bloodgulch.map");
            CResources::CResource bitmfile("bitmaps.map");
            bitmfile.memory_map();
            mapfile.memory_map();
            const CBlam::blam_file_header* map =
                    CBlam::blam_file_header_get(mapfile.data,CBlam::blam_version_pc);
            CBlam::blam_tag_index tags = CBlam::blam_tag_index_get(map);

            const CBlam::blam_index_item* base_idx = CBlam::blam_tag_meta_offset(map);
            const CBlam::blam_index_item* idx = nullptr;
            for(int32 i=0;i<tags.tagCount;i++)
            {
                idx = &base_idx[i];
                if(coffee_cmp_memarea(idx->tagclass[0],CBlam::blam_index_item_type_bitm,4))
                {
                    int32 num = 0;
                    const CBlam::blam_bitm_image* img =
                            CBlam::coffee_bitm_get(idx,map,tags.index_magic,&num);
                    cstring t = CBlam::blam_index_item_get_string(idx,map,&tags);
                    CBlam::blam_bitm_texture_def tex =
                            CBlam::coffee_bitm_get_texture(img,bitmfile.data);

                    coffee_texture_2d_load(tex,ctxt);
                    cDebug("Image: %s,d=%i,f=%i",t,img->depth,img->format);
                }
            }
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
                coffee_graphics_gen_matrix(mod);
                transform.push_back(mod->matrix);
            }

            mod->position.x = mod->position.y = 0.f;
            mod->position.z = -1.f;

            coffee_graphics_gen_matrix(mod);
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
    coffee_graphics_bind(&ctxt->renderdata.buffers.d[2]);
    coffee_graphics_buffer_sub_bind(ctxt->renderdata.uniformblocks.d[0].buffer);
    coffee_graphics_bind(&ctxt->shaders.pipelines.d[0]);
    coffee_graphics_bind(ctxt->renderdata.datasets.d[0].vao);
    coffee_graphics_bind(ctxt->renderdata.datasets.d[0].drawcalls->drawbuffer);

    GLint loc = coffee_graphics_shader_uniform_value_get(
                &ctxt->shaders.programs.d[0],"diffuseSampler");

    szptr texIndex = rand()%ctxt->texstorage.size;

    if(ctxt->features->ext_bindless_texture)
        coffee_graphics_tex_get_handle(&ctxt->texstorage.d[texIndex]);

    coffee_graphics_tex_param(
                &ctxt->texstorage.d[0],GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    coffee_graphics_tex_param(
                &ctxt->texstorage.d[0],GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

    ctxt->texstorage.d[texIndex].unit = 0;
    ctxt->funptrs.textures.load(&ctxt->texstorage.d[texIndex]);
    if(ctxt->features->ext_bindless_texture)
    {
        glProgramUniformHandleui64ARB(
                    ctxt->shaders.programs.d[0].handle,loc,
                ctxt->texstorage.d[texIndex].bhandle);
    }else{
        glProgramUniform1i(ctxt->shaders.programs.d[0].handle,loc,ctxt->texstorage.d[texIndex].unit);
    }
}

void coffee_render_test(game_context *ctxt, double delta)
{
    //Generate our matrices
    coffee_graphics_gen_matrix(&ctxt->transforms.transforms.d[0]);
    coffee_graphics_gen_matrix_perspective(&ctxt->transforms.cameras.d[0]);

    //Copy memory into GL
    memcpy(ctxt->renderdata.buffers.d[4].data,
            &ctxt->transforms.cameras.d[0].matrix,
            sizeof(CMath::mat4));
    memcpy(ctxt->renderdata.buffers.d[2].data,
            &ctxt->transforms.transforms.d[0].matrix,
            sizeof(CMath::mat4));

    //Send it off
    ctxt->funptrs.renderfun(ctxt->renderdata.datasets.d[0]);
}

void coffee_unload_test(game_context *ctxt)
{
    coffee_graphics_unbind(&ctxt->renderdata.buffers.d[2]);

    szptr i;

    for(i=0;i<ctxt->renderdata.buffers.size;i++)
    {
        coffee_graphics_unbind(&ctxt->renderdata.buffers.d[i]);
        ctxt->funptrs.buffers.unmap(&ctxt->renderdata.buffers.d[i]);
        coffee_graphics_free(&ctxt->renderdata.buffers.d[i]);
    }

    for(i=0;i<ctxt->vertexdata.descriptor.arrays.size;i++)
        coffee_graphics_free(&ctxt->vertexdata.descriptor.arrays.d[i]);
    for(i=0;i<ctxt->vertexdata.data.size;i++)
        CResourceTypes::coffee_assimp_free(ctxt->vertexdata.data.d[i]);

    for(i=0;i<ctxt->shaders.pipelines.size;i++)
        coffee_graphics_free(&ctxt->shaders.pipelines.d[i]);
    for(i=0;i<ctxt->shaders.shaders.size;i++)
        coffee_graphics_free(&ctxt->shaders.shaders.d[i]);
    for(i=0;i<ctxt->shaders.programs.size;i++)
        coffee_graphics_free(&ctxt->shaders.programs.d[i]);

    for(i=0;i<ctxt->renderdata.datasets.size;i++)
        coffee_multidraw_free(&ctxt->renderdata.datasets.d[i]);

    for(i=0;i<ctxt->texstorage.size;i++)
        coffee_graphics_free(&ctxt->texstorage.d[i]);

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
