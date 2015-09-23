#ifndef BASE_CASE
#define BASE_CASE

#include "coffee_impl/memory/cgame_dataset.h"

namespace Coffee{
namespace CRendering{
namespace CTest{

using namespace CMemoryManagement;

struct game_context
{
    game_vertexdata_chunk   vertexdata  = {0};
    game_shader_manager     shaders     = {0};
    game_resource_chunk     resources   = {0};
    game_memory_chunk       renderdata  = {0};

    game_transform_chunk    transforms  = {0};
};

static void coffee_test_load(game_context* ctxt)
{
    //Create multidraw object
    coffee_mem_expand_array<CMultiDrawDataSet>(&ctxt->renderdata.datasets,1);
    CMultiDrawDataSet* multidraw = &ctxt->renderdata.datasets.d[0];
    *multidraw = coffee_multidraw_create();

    //Load basic shader
    coffee_mem_expand_array<CResource>(&ctxt->resources,2);
    coffee_mem_expand_array<CShader>(&ctxt->shaders.shaders,2);
    coffee_mem_expand_array<CShaderProgram>(&ctxt->shaders.programs,1);
    coffee_mem_expand_array<CPipeline>(&ctxt->shaders.pipelines,1);
    {
        CResource* v = &ctxt->resources.d[0]; //Vertex shader
        CResource* f = &ctxt->resources.d[1]; //Fragment shader
        v->read_data(true);
        f->read_data(true);

        CShader* vshdr = &ctxt->shaders.shaders.d[0];
        CShader* fshdr = &ctxt->shaders.shaders.d[1];
        vshdr->compile(v,GL_VERTEX_SHADER);
        fshdr->compile(f,GL_VERTEX_SHADER);

        CShaderProgram* p = &ctxt->shaders.programs[0];
        p->create();
        p->attachShader(vshdr,GL_VERTEX_SHADER_BIT);
        p->attachShader(fshdr,GL_FRAGMENT_SHADER_BIT);
        p->link();

        //Try dumping it to a file, we don't care about the resource later on
        {
            CResource dmp;
            p->storeProgram(&dmp);
            if(!dmp.save_data())
                cDebug("Failed to save shader to file!");
            dmp.free_data();
        }
    }

    //Load mesh
    coffee_mem_expand_array<CResource>(&ctxt->resources,1);
    coffee_mem_expand_array<CAssimpData*>(&ctxt->vertexdata.data,1);
    coffee_mem_expand_array<CBuffer>(&ctxt->renderdata.buffers,3);
    {
        CResource* m = &ctxt->resources.d[2];
        m->read_data();
        CAssimpData* d = CAssimpImporters::importResource(m,"blend");
        m->free_data();

        //Store a pointer to the data for freeing later
        ctxt->vertexdata.data.d[0] = d;


    }
}

}
}
}

#endif // BASE_CASE

