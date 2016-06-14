#pragma once

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/tdef/stltypes.h>

struct AAsset;

namespace Coffee{
    /* Android API version */
    COFFAPI int32 Coffee_AndroidGetApiVersion();

    /* Directories */
    COFFAPI CString Coffee_GetDataPath();
    COFFAPI CString Coffee_GetExternalDataPath();
    COFFAPI CString Coffee_GetObbDataPath();

    /* Asset API */
    COFFAPI AAsset* Coffee_AssetGet(cstring fname);
    COFFAPI void Coffee_AssetClose(AAsset* fp);
    COFFAPI int64 Coffee_AssetGetSizeFn(cstring fname);
    COFFAPI int64 Coffee_AssetGetSize(AAsset* fp);
    COFFAPI c_cptr Coffee_AssetGetPtr(AAsset* fp);

    /* Software info */
    COFFAPI CString Coffee_AndroidGetRelease();
    COFFAPI CString Coffee_AndroidGetBuildType();
    COFFAPI Vector<CString> Coffee_AndroidGetABIs();

    /* Hardware info */
    COFFAPI CString Coffee_AndroidGetBoard();
    COFFAPI CString Coffee_AndroidGetBrand();
    COFFAPI CString Coffee_AndroidGetManufacturer();
    COFFAPI CString Coffee_AndroidGetProduct();
    COFFAPI CString Coffee_AndroidGetDevice();
}