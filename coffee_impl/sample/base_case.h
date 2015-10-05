#ifndef BASE_CASE
#define BASE_CASE

#include "coffee_impl/memory/cgame_dataset.h"
#include "datasources/blam/cblam.h"

namespace Coffee{
namespace CRendering{
namespace CTest{

using namespace CMemoryManagement;

extern CPipeline* coffee_shader_program_load(
        const game_shader_program_desc& desc,
        game_context* ctxt);
extern CTexture* coffee_texture_2d_load(
        CResource* textureres,
        game_context* ctxt);

extern CTexture* coffee_texture_2d_load_blam(
        const CBlam::blam_bitm_image* text,
        const CBlam::blam_file_header* head,
        int32 magic,
        game_context* ctxt);

extern bool coffee_test_load(
        game_context* ctxt);
extern void coffee_prepare_test(
        game_context* ctxt);
extern void coffee_render_test(
        game_context* ctxt,
        double delta);
extern void coffee_unload_test(
        game_context* ctxt);

}
}
}

#endif // BASE_CASE

