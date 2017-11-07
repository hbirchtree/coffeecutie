//#include <coffee/android/android_main.h>
//#include <android_native_app_glue.h>

//using namespace Coffee;

//CString Coffee_GetDataPath()
//{
//    return coffee_app->activity->internalDataPath;
//}

//CString Coffee_GetObbDataPath()
//{
//    return coffee_app->activity->obbPath;
//}

//CString Coffee_GetExternalDataPath()
//{
//    return coffee_app->activity->externalDataPath;
//}

//AAssetManager* Coffee_GetAssetManager()
//{
////    return coffee_app->activity->assetmanager;
//    return nullptr;
//}

//AAsset* Coffee_AssetGet(cstring fname)
//{
//    return AAssetManager_open(coffee_app->activity->assetManager,fname,AASSET_MODE_BUFFER);
//}

//void Coffee_AssetClose(AAsset* fp)
//{
//    AAsset_close(fp);
//}

//int64 Coffee_AssetGetSizeFn(cstring fname)
//{
//    AAsset* fp = AAssetManager_open(coffee_app->activity->assetManager,fname,0);
//    if(!fp)
//        return 0;
//    int64 sz = AAsset_getLength64(fp);
//    AAsset_close(fp);
//    return sz;
//}

//int64 Coffee_AssetGetSize(AAsset* fp)
//{
//    return AAsset_getLength64(fp);
//}

//c_cptr Coffee_AssetGetPtr(AAsset* fp)
//{
//    return AAsset_getBuffer(fp);
//}
