#ifndef BASE_CASE
#define BASE_CASE

#include <coffee/core/CTypes>
#include "memory/cgame_dataset.h"
#include <coffee/blam/cblam.h>

namespace Coffee{
namespace CRendering{
namespace CTest{

using namespace CMemoryManagement;

constexpr int blam_dxtc_tex_mapping_count = 3;
constexpr struct {int16 in; CTexIntFormat out;} blam_dxtc_tex_mapping[blam_dxtc_tex_mapping_count] = {

};

extern void coffee_test_fun_set(game_context* ctxt);

extern bool coffee_test_load(
        game_context* ctxt);
extern void coffee_prepare_test(
        game_context* ctxt);
extern void coffee_render_test(game_context* ctxt, double);
extern void coffee_unload_test(
        game_context* ctxt);

}
}
}

#endif // BASE_CASE

