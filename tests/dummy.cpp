#include <coffee/core/CUnitTesting>

/* Core components */
#include <coffee/core/CDebug>
#include <coffee/core/CDisplay>
#include <coffee/core/CFiles>
#include <coffee/core/CMD>
#include <coffee/core/CMath>
#include <coffee/core/CThreading>
#include <coffee/core/CThreadingTypes>
#include <coffee/core/plat/plat_environment.h>
#include <peripherals/stl/string_casting.h>
#include <coffee/core/types/cdef/memsafe.h>

/* Graphics API interfaces */
#include <coffee/interfaces/cgraphics_api.h>
#include <coffee/interfaces/cgraphics_api_thread.h>
#include <coffee/interfaces/cgraphics_ext.h>
#include <coffee/interfaces/cgraphics_pixops.h>

/* General interfaces */
#include <coffee/interfaces/byte_provider.h>
#include <coffee/interfaces/caudiomixer_api.h>
#include <coffee/interfaces/caudiomixer_utils.h>
#include <coffee/interfaces/cdialog_api.h>
#include <coffee/interfaces/content_pipeline.h>
#include <coffee/interfaces/file_resolver.h>

/* Common graphics stuff */
#include <coffee/graphics/common/SMesh>
#include <coffee/graphics/common/cgraphicsdata.h>
#include <coffee/graphics/common/gltypes.h>
#include <coffee/graphics/common/query/gpu_query.h>

/* GLEAM graphics API */
#include <coffee/graphics/apis/CGLeamRHI>

/* Blam! integration */
#include <coffee/blam/cblam.h>

/* STB-related APIs */
#include <coffee/audio/caudio.h>
#include <coffee/image/cimage.h>

/* ASSIMP APIs */
#if defined(HAS_ASSIMP)
#include <coffee/assimp/assimp_deserializer.h>
#include <coffee/assimp/assimp_iterators.h>
#include <coffee/assimp/assimp_material_iterators.h>
#include <coffee/assimp/cassimpimporters.h>
#endif

bool dummy_test()
{
    return true;
}

COFFEE_TEST_SUITE(1) = {{dummy_test, "Dummy test for coverage"}};

COFFEE_EXEC_TESTS()
