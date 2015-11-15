#ifndef BASE_CASE
#define BASE_CASE

#include "memory/cgame_dataset.h"
#include <blam/cblam.h>

namespace Coffee{
namespace CRendering{
namespace CTest{

using namespace CMemoryManagement;

constexpr int blam_dxtc_tex_mapping_count = 3;
constexpr struct {int16 in; CTexIntFormat out;} blam_dxtc_tex_mapping[blam_dxtc_tex_mapping_count] = {
    {CBlam::blam_bitm_tex_DXT1,CTexIntFormat::DXT1},
    {CBlam::blam_bitm_tex_DXT3,CTexIntFormat::DXT3},
    {CBlam::blam_bitm_tex_DXT5,CTexIntFormat::DXT5},
};

extern void coffee_test_fun_set(game_context* ctxt);

extern CPipeline* coffee_shader_program_load(
        const game_shader_program_desc& desc,
        game_context* ctxt);
extern CTexture* coffee_texture_2d_load(
        CResource* textureres,
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

