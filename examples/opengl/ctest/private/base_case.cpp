#include <base_case.h>

#include <vector>
#include <coffee/CImage>
#include <coffee/CGraphics>

namespace Coffee{
namespace CRendering{
namespace CTest{

using namespace CResourceTypes;

void coffee_test_fun_set(game_context *ctxt)
{
    if(ctxt->features->ext_multi_draw_indirect)
        ctxt->funptrs.renderfun = coffee_multidraw_render;
    else
        ctxt->funptrs.renderfun = coffee_multidraw_render_safe;

    if(ctxt->features->ext_bindless_texture)
    {
        ctxt->funptrs.textures.load = coffee_graphics_tex_load;
        ctxt->funptrs.textures.unload = coffee_graphics_tex_unload;
    }else{
        ctxt->funptrs.textures.load = coffee_graphics_tex_load_safe;
        ctxt->funptrs.textures.unload = coffee_graphics_tex_unload_safe;
    }

    if(ctxt->features->ext_texture_storage)
    {
        ctxt->funptrs.textures.define = coffee_graphics_tex_define;
        ctxt->funptrs.textures.store = coffee_graphics_tex_store;
    }else{
        ctxt->funptrs.textures.define = coffee_graphics_tex_define_safe;
        ctxt->funptrs.textures.store = coffee_graphics_tex_store_safe;
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

    ctxt->funptrs.buffers.subdata = coffee_graphics_buffer_sub_dataptr;

    if(ctxt->features->ext_invalidate_subdata)
    {
        ctxt->funptrs.buffers.invalidate = coffee_graphics_buffer_invalidate;
    }else{
        ctxt->funptrs.buffers.invalidate = coffee_graphics_buffer_invalidate_safe;
    }
}

szptr coffee_shader_program_load(const game_shader_program_desc &desc, game_context *ctxt)
{
    szptr pl_idx = ctxt->shaders.pipelines.size;

    coffee_mem_expand_array(&ctxt->shaders.pipelines,1);

    CResource v(desc.shader_v); //Vertex shader
    CResource f(desc.shader_f); //Fragment shader
    if(!FileExists(v)||!FileExists(f))
        cFatal("Failed to locate shaders");
    FilePull(v,true);
    FilePull(f,true);

    CSimplePipeline& ppl = ctxt->shaders.pipelines.d[pl_idx];

    new (&ppl) CSimplePipeline;

    ppl.create((cstring)v.data,(cstring)f.data);

    FileFree(v);
    FileFree(f);

    return pl_idx;
}

CTexture* coffee_texture_2d_load(const CBlam::bitm_texture_t& tex, game_context* ctxt)
{
    coffee_mem_expand_array(&ctxt->texstorage,1);
    coffee_mem_expand_array(&ctxt->texsamplerstorage,1);
    CTexture& t = ctxt->texstorage.d[ctxt->texstorage.size-1];
    CTextureSampler& ts = ctxt->texsamplerstorage.d[ctxt->texsamplerstorage.size-1];

    coffee_graphics_alloc(t);
    coffee_graphics_alloc(ts);

    t.type = tex.type;
    t.levels = tex.mipmaps;
    t.format = tex.cformat;
    t.size = tex.resolution;

    coffee_graphics_activate(t);

    coffee_graphics_tex_sampler_parame(ts,CTexParam::MipmapMinFilter,
                                       CTexParamOpt::LinearMipmapLinear);
    coffee_graphics_tex_sampler_parame(ts,CTexParam::MipmapMagFilter,
                                       CTexParamOpt::Linear);

    if(tex.format==CTexFormat::None && tex.dformat == CDataType::None)
    {
        coffee_graphics_tex_dxtc_load(
                    t,
                    (tex.cformat == CTexIntFormat::DXT1) ? 8 : 16,
                    tex.data);

    }else{
        CTextureData tdata = {};
        tdata.datatype = tex.dformat;
        tdata.format = tex.format;
        tdata.size = tex.resolution;
        tdata.data = tex.data;

        ctxt->funptrs.textures.define(t);
        ctxt->funptrs.textures.store(t,tdata,0);
    }

    if(ctxt->features->ext_bindless_texture)
        coffee_graphics_tex_get_handle(t,ts);

    return &t;
}

void coffee_test_def_vao(game_context* ctxt)
{
    //We have two formats
    szptr b_idx = ctxt->renderdata.buffers.size;
    szptr v_idx = ctxt->renderdata.buffers.size;
    coffee_mem_expand_array(&ctxt->renderdata.buffers,6);
    coffee_mem_expand_array(&ctxt->vertexdata.descriptor.arrays,1);
    {
        CBuffer& psbuffer = ctxt->renderdata.buffers.d[b_idx];
        CBuffer& txbuffer = ctxt->renderdata.buffers.d[b_idx+1];
        CBuffer& mxbuffer = ctxt->renderdata.buffers.d[b_idx+2];
        CBuffer& ixbuffer = ctxt->renderdata.buffers.d[b_idx+3];

        CVertexArrayObject& vao = ctxt->vertexdata.descriptor.arrays.d[v_idx];

        coffee_graphics_alloc(6,&psbuffer);
        coffee_graphics_activate(6,&psbuffer);
        coffee_graphics_alloc(vao);
        coffee_graphics_bind(vao);

        ixbuffer.type = CBufferType::Index;

        CVertexDescription vdesc;
        vdesc.addAttribute<scalar,3,CDataType::Scalar>(0,nullptr);
        vdesc.addAttribute<scalar,2,CDataType::Scalar>(1,nullptr);
        vdesc.addAttributeMatrix<scalar,4>(2,nullptr);

        for(size_t i=0;i<4;i++)
        {
            vdesc.getBinding(2+i)->binding = 2+i;
            vdesc.getBinding(2+i)->divisor = 1;
            vdesc.getBinding(2+i)->buffer = &mxbuffer;
        }
        vdesc.getBinding(0)->binding = 0;
        vdesc.getBinding(0)->buffer = &psbuffer;
        vdesc.getBinding(1)->binding = 1;
        vdesc.getBinding(1)->buffer = &txbuffer;

        vdesc.applyAttributes(vao);
        vdesc.bindAttributes(vao);

        coffee_graphics_vao_attribute_index_buffer(vao,ixbuffer);
    }
}

void coffee_test_load_meshes(game_context* ctxt)
{
    std::vector<CAssimpMesh*>& meshes = ctxt->meshes;

    std::vector<CGLDrawCall> drawcalls;

    std::vector<CVec3> positions;
    std::vector<CVec2> texcoords;
    std::vector<uint32> indices;

    for(size_t i=0;i<meshes.size();i++)
    {
        CAssimpMesh* mesh = meshes[i];
        CGLDrawCall call;
        call.baseInstance = drawcalls.size()*10;
        call.instanceCount = 10;
        call.baseVertex = positions.size();
        call.firstIndex = indices.size();
        for(size_t j=0;j<mesh->numBuffers;j++)
        {
            byte_t* data = coffee_assimp_get_reflexive_ptr(mesh,&mesh->buffers[j]);
            if(mesh->buffers[j].type == CAssimpMesh::PositionType)
            {
                positions.insert(positions.end(),data,&data[mesh->buffers[j].size]);
            }else if(mesh->buffers[j].type == CAssimpMesh::TextCoordType){
                texcoords.insert(texcoords.end(),data,&data[mesh->buffers[j].size]);
            }else if(mesh->buffers[j].type == CAssimpMesh::IndexType){
                indices.insert(indices.end(),data,&data[mesh->buffers[j].size]);
            }
        }
        call.count = indices.size()-call.firstIndex;
        drawcalls.push_back(call);
    }

    CBuffer& drawbuffer = ctxt->renderdata.buffers.d[5];
    CBuffer& posbuffer = ctxt->renderdata.buffers.d[0];
    CBuffer& texbuffer = ctxt->renderdata.buffers.d[1];
    CBuffer& idxbuffer = ctxt->renderdata.buffers.d[3];

    coffee_graphics_buffer_store(drawbuffer,drawcalls.data(),
                                 drawcalls.size()*sizeof(CGLDrawCall),
                                 CBufferUsage::StaticDraw);

    coffee_graphics_buffer_store(posbuffer,positions.data(),positions.size()*sizeof(CVec3),
                                 CBufferUsage::StaticDraw);
    coffee_graphics_buffer_store(texbuffer,texcoords.data(),texcoords.size()*sizeof(CVec2),
                                 CBufferUsage::StaticDraw);
    coffee_graphics_buffer_store(idxbuffer,indices.data(),indices.size()*sizeof(uint32),
                                 CBufferUsage::StaticDraw);
}

void coffee_test_def_transforms(game_context* ctxt, szptr numGears)
{
    szptr c_idx = ctxt->transforms.cameras.size;
    szptr t_idx = ctxt->transforms.transforms.size;
    szptr bn_idx = ctxt->renderdata.bufferbindings.size;

    coffee_mem_expand_array(&ctxt->transforms.transforms,1);
    coffee_mem_expand_array(&ctxt->transforms.cameras,1);
    coffee_mem_expand_array(&ctxt->renderdata.bufferbindings,1);

    CBuffer& matrix_buffer = ctxt->renderdata.buffers.d[2];
    CTransform& my_transform = ctxt->transforms.transforms.d[t_idx];

    CTransform transform_dummy;
    std::vector<CMat4> matrix_data;

    for(size_t i=0;i<numGears;i++)
    {
        transform_dummy.position = CVec3(0,0,i/2);
        transform_dummy.scale = CVec3(1);
        matrix_data.push_back(GenTransform(transform_dummy));
    }
    transform_dummy.position = CVec3(0,0,-3);
    my_transform = transform_dummy;

    coffee_graphics_buffer_store_immutable(matrix_buffer,matrix_data.data(),
                                           matrix_data.size()*sizeof(CMat4),
                                           CBufferConstants::PersistentStorageFlags());
    coffee_graphics_buffer_map(matrix_buffer,
                               CBufferConstants::PersistentAccessFlags());

    CBuffer& camera_buffer = ctxt->renderdata.buffers.d[4];
    CGCamera& my_camera = ctxt->transforms.cameras.d[c_idx];
    CUniformBlockBinding& blbind = ctxt->renderdata.bufferbindings.d[bn_idx];

    camera_buffer.type = CBufferType::Uniform;

    blbind.desc = new CUniformBlock;
    blbind.desc->object_name = "MatrixBlock";
    blbind.desc->index = -1;
    blbind.resource = new CSubBuffer;
    blbind.resource->parent = &camera_buffer;
    blbind.resource->type = CBufferType::Uniform;
    blbind.resource->size = sizeof(CMat4);

    coffee_graphics_uniform_block_get(ctxt->shaders.pipelines.d[0].vert,*blbind.desc);

    my_camera.aspect = 1.6f;
    my_camera.fieldOfView = 90.f;
    my_camera.position = CVec3(0);
    my_camera.rotation = CQuat();
    my_camera.zVals.far = 100.f;
    my_camera.zVals.near = 1.f;

    CMat4 cam_matrix = GenPerspective(my_camera)
            * GenTransform(my_camera);

    coffee_graphics_buffer_store_immutable(camera_buffer,&cam_matrix,sizeof(CMat4),
                                           CBufferConstants::PersistentStorageFlags());
    coffee_graphics_buffer_map(camera_buffer,CBufferConstants::PersistentAccessFlags());

    blbind.index = 0;

    coffee_graphics_buffer_bind_range(camera_buffer,CBufferType::Uniform,0,sizeof(CMat4),
                                      blbind.index);

    coffee_graphics_uniform_block_set(ctxt->shaders.pipelines.d[0].vert,blbind);
}

bool coffee_test_load(game_context *ctxt)
{
    //Load basic shader
    {
        game_shader_program_desc d;
        d.shader_dump = "basic_dump.shdr";
        d.shader_f = "ctest/basic_shader.ssbo.fs";
        d.shader_v = "ctest/instancing.ssbo.vs";

        if(!ctxt->features->ext_ssbo_support)
        {
            d.shader_f = "ctest/basic_shader.fs";
            d.shader_v = "ctest/instancing.vs";
        }

        coffee_shader_program_load(d,ctxt);
    }

    coffee_graphics_bind(ctxt->shaders.pipelines.d[0].data_ref());

    //Specify VAO for vertex data, we will only use one for now

    szptr numGears = 100;

    coffee_test_def_vao(ctxt);

    coffee_test_load_meshes(ctxt);

    coffee_test_def_transforms(ctxt,numGears);

    {
        CResources::CResource mapfile("cblam_data/bloodgulch.map");
        CResources::CResource bitmfile("cblam_data/bitmaps.map");
        FileMap(bitmfile);
        FileMap(mapfile);
        const CBlam::file_header_t* map =
                CBlam::blam_file_header_get(mapfile.data,CBlam::version_t::pc);
        CBlam::tag_index_t tags = CBlam::blam_tag_index_get(map);

        const CBlam::index_item_t* base_idx = CBlam::blam_tag_index_get_items(map);
        const CBlam::index_item_t* idx = nullptr;
        for(int32 i=0;i<tags.tagCount;i++)
        {
            idx = &base_idx[i];
            if(MemCmp(idx->tagclass[0],CBlam::blam_index_item_type_bitm,4))
            {
                int32 num = 0;
                const CBlam::bitm_image_t* img =
                        CBlam::coffee_bitm_get(idx,map,tags.index_magic,&num);
//                cstring t = CBlam::blam_index_item_get_string(idx,map,&tags);
                CBlam::bitm_texture_t tex =
                        CBlam::coffee_bitm_get_texture(img,bitmfile.data);

                coffee_texture_2d_load(tex,ctxt);
//                cDebug("Image: {0},d={1},f={2}",t,img->depth,(uint16)img->format);
            }
        }
        FileUnmap(bitmfile);
        FileUnmap(mapfile);
    }

    return true;
}

void coffee_prepare_test(game_context *ctxt)
{
    //Bind drawcall buffer
    CBuffer& drawcalls = ctxt->renderdata.buffers.d[5];

    drawcalls.type = CBufferType::DrawIndirect;
    coffee_graphics_bind(drawcalls);

    CUniform texuni;
    texuni.object_name = "diffuseSampler";
    coffee_graphics_uniform_get(ctxt->shaders.pipelines.d[0].frag,texuni);

    szptr texIndex = rand()%ctxt->texstorage.size;

    ctxt->funptrs.textures.load(ctxt->texsamplerstorage.d[texIndex],
                                ctxt->texstorage.d[texIndex]);
    if(ctxt->features->ext_bindless_texture)
    {
        coffee_graphics_uniform_set_texhandle(
                    ctxt->shaders.pipelines.d[0].frag,
                texuni,
                ctxt->texsamplerstorage.d[texIndex].bhandle);
    }else{
        coffee_graphics_uniform_set_texhandle_safe(
                    ctxt->shaders.pipelines.d[0].frag,
                texuni,
                ctxt->texsamplerstorage.d[texIndex].unit);
    }
}

static CGSync* sync_object = nullptr;

void coffee_render_test(game_context *ctxt, double)
{
    CGCamera& camera = ctxt->transforms.cameras.d[0];
    CTransform& transform = ctxt->transforms.transforms.d[0];

    //Generate our matrices
    CMat4 mtransform = GenTransform(transform);
    CMat4 mcamera = GenPerspective(camera)
            * GenTransform(camera);

    //Copy memory into GL
    MemCpy(ctxt->renderdata.buffers.d[4].data,
            &mcamera,
            sizeof(CMat4));
    MemCpy(ctxt->renderdata.buffers.d[2].data,
            &mtransform,
            sizeof(CMat4));

    if(sync_object)
    {
        coffee_graphics_fence_wait(sync_object,10);
        coffee_graphics_free(sync_object);
    }
    //Send it off
    coffee_graphics_multidraw(CPrimitiveMode::Triangles,ctxt->renderdata.buffers.d[5]);
    //
    sync_object = coffee_graphics_fence_create();
}

void coffee_unload_test(game_context *ctxt)
{
    coffee_graphics_unbind(ctxt->renderdata.buffers.d[2]);

    szptr i;

    for(i=0;i<ctxt->renderdata.buffers.size;i++)
    {
        coffee_graphics_unbind(ctxt->renderdata.buffers.d[i]);
        ctxt->funptrs.buffers.unmap(ctxt->renderdata.buffers.d[i]);
        coffee_graphics_free(ctxt->renderdata.buffers.d[i]);
    }

    for(i=0;i<ctxt->vertexdata.descriptor.arrays.size;i++)
        coffee_graphics_free(ctxt->vertexdata.descriptor.arrays.d[i]);
    for(i=0;i<ctxt->vertexdata.data.size;i++)
        CResourceTypes::CAssimp::coffee_assimp_free(ctxt->vertexdata.data.d[i]);

    for(i=0;i<ctxt->texstorage.size;i++)
        coffee_graphics_free(ctxt->texstorage.d[i]);
    for(i=0;i<ctxt->texsamplerstorage.size;i++)
        coffee_graphics_free(ctxt->texsamplerstorage.d[i]);

    CFree(ctxt->texsamplerstorage.d);
    CFree(ctxt->texstorage.d);

    CFree(ctxt->vertexdata.buffers.d);
    CFree(ctxt->vertexdata.data.d);
    CFree(ctxt->vertexdata.descriptor.arrays.d);
    CFree(ctxt->vertexdata.descriptor.bindings.d);
    CFree(ctxt->vertexdata.descriptor.formats.d);

    CFree(ctxt->renderdata.buffers.d);
    CFree(ctxt->renderdata.subbuffers.d);
    CFree(ctxt->renderdata.bufferbindings.d);
    CFree(ctxt->renderdata.storageblocks.d);

    CFree(ctxt->resources.d);

    CFree(ctxt->shaders.pipelines.d);

    CFree(ctxt->transforms.cameras.d);
    CFree(ctxt->transforms.transforms.d);
}

}
}
}
