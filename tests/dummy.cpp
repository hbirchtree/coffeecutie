#include <coffee/core/CUnitTesting>

/* Core components */
#include <coffee/core/CDebug>
#include <coffee/core/CDisplay>
#include <coffee/core/CFiles>
#include <coffee/core/CMath>
#include <coffee/core/CThreading>
#include <coffee/core/types/chunk.h>
#include <peripherals/stl/string_casting.h>

/* Graphics API interfaces */
#include <coffee/interfaces/cgraphics_api.h>
#include <coffee/interfaces/cgraphics_api_thread.h>
#include <coffee/interfaces/cgraphics_ext.h>
#include <coffee/interfaces/cgraphics_pixops.h>

/* General interfaces */
#include <coffee/interfaces/byte_provider.h>
#include <coffee/interfaces/cdialog_api.h>
#include <coffee/interfaces/content_pipeline.h>
#include <coffee/interfaces/file_resolver.h>
#if defined(FEATURE_ENABLE_Audio)
#include <coffee/interfaces/caudiomixer_api.h>
#include <coffee/interfaces/caudiomixer_utils.h>
#endif

/* Common graphics stuff */
#if defined(FEATURE_ENABLE_GraphicsAPI)
#include <coffee/graphics/common/cgraphicsdata.h>
#include <coffee/graphics/common/query/gpu_query.h>
#endif

/* GLEAM graphics API */
#if defined(FEATURE_ENABLE_GLeamCommon)
#include <coffee/graphics/apis/CGLeamRHI>
#endif

/* Blam! integration */
#if defined(FEATURE_ENABLE_Blam)
#include <blam/cblam.h>
#endif

/* STB-related APIs */
#if defined(FEATURE_ENABLE_Audio)
#include <coffee/audio/caudio.h>
#endif
#if defined(FEATURE_ENABLE_Image)
#include <coffee/image/cimage.h>
#endif

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

COFFEE_TESTS_BEGIN(1)

    {dummy_test, "Dummy test for coverage"}

COFFEE_TESTS_END()
