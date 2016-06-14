#include <coffee/android/android_main.h>

#include <coffee/core/coffee.h>
#include <coffee/core/coffee_version.h>

#include <coffee/core/CDebug>
#include <coffee/core/platform_data.h>

#include <android/sensor.h>

#include <jni.h>
#include <android_native_app_glue.h>

using namespace Coffee;

struct android_app* coffee_app = nullptr;
JavaVM* coffee_jvm = nullptr;

/*
 *
 * System information APIs
 *
 */

COFFAPI int32 Coffee_AndroidGetApiVersion()
{
    return coffee_app->activity->sdkVersion;
}

COFFAPI CString Coffee_AndroidGetRelease()
{
    JNIEnv* env = coffee_app->activity->env;
    jclass buildcl = env->FindClass("/android/os/Build/VERSION");

    cDebug("Derp");

    return "";
}

struct javavar
{
    JNIEnv* env;
    jclass ass;
    jfieldID field;
};

javavar Coffee_JavaGetStaticMember(cstring clss, cstring var, cstring type)
{
    javavar _var;
    _var.env = coffee_app->activity->env;
    if(!_var.env)
        return {};


    _var.ass = _var.env->FindClass(clss);
    _var.field = _var.env->GetStaticFieldID(_var.ass,var,type);

    return _var;
}

COFFAPI CString Coffee_AndroidGetBoard()
{
    javavar brd = Coffee_JavaGetStaticMember("android/os/Build","BOARD",
                                             "Ljava/lang/String;");
    if(!brd.env)
        return "!";
    jstring str = (jstring)brd.env->GetStaticObjectField(brd.ass,brd.field);
    return brd.env->GetStringUTFChars(str,0);
}

COFFAPI CString Coffee_AndroidGetBrand()
{
    return "";
}

COFFAPI CString Coffee_AndroidGetDevice()
{
    return "";
}

COFFAPI CString Coffee_AndroidGetManufacturer()
{
    return "";
}

COFFAPI CString Coffee_AndroidGetProduct()
{
    return "";
}

COFFAPI Vector<CString> Coffee_AndroidGetABIs()
{
    return {};
}

COFFAPI CString Coffee_AndroidGetBuildType()
{
    return "";
}

COFFAPI uint64 Coffee_GetMemoryMax()
{
    return 0;
}

/*
 *
 * Asset APIs
 *
 */

COFFAPI CString Coffee_GetDataPath()
{
    return coffee_app->activity->internalDataPath;
}

COFFAPI CString Coffee_GetObbDataPath()
{
    return coffee_app->activity->obbPath;
}

COFFAPI CString Coffee_GetExternalDataPath()
{
    return coffee_app->activity->externalDataPath;
}

COFFAPI AAsset* Coffee_AssetGet(cstring fname)
{
    return AAssetManager_open(coffee_app->activity->assetManager,fname,AASSET_MODE_BUFFER);
}

COFFAPI void Coffee_AssetClose(AAsset* fp)
{
    AAsset_close(fp);
}

COFFAPI int64 Coffee_AssetGetSizeFn(cstring fname)
{
    AAsset* fp = AAssetManager_open(coffee_app->activity->assetManager,fname,0);
    if(!fp)
        return 0;
    int64 sz = AAsset_getLength64(fp);
    AAsset_close(fp);
    return sz;
}

COFFAPI int64 Coffee_AssetGetSize(AAsset* fp)
{
    return AAsset_getLength64(fp);
}

COFFAPI c_cptr Coffee_AssetGetPtr(AAsset* fp)
{
    return AAsset_getBuffer(fp);
}

/*
 *
 * Startup and runtime utilities
 *
 */

COFFAPI void CoffeeActivity_onCreate(
        ANativeActivity* activity,
        void* savedState,
        size_t savedStateSize
)
{
    /*
     * We do this because the visibility of the proper function
     *  declaration is not visible with -fvisibility=hidden, this
     *  allows us to keep the NDK as it is while also having a
     *  nice entry point for other code.
     */
    ANativeActivity_onCreate(activity,savedState,savedStateSize);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*)
{
    cDebug("Java VM called me!");
    coffee_jvm = vm;
}

extern CoffeeMainWithArgs android_entry_point;

void android_main(struct android_app* state)
{
    /* According to docs, something something glue check */
    app_dummy();

    coffee_app = state;

    cDebug("Android API version: {0}",Coffee_AndroidGetApiVersion());

//    cDebug("Board: {0}",Coffee_AndroidGetBoard());

    {
        /* Set version information */
        CoffeeApplicationData.application_name = COFFEE_APPLICATION_NAME;
        CoffeeApplicationData.organization_name = COFFEE_ORGANIZATION_NAME;
        CoffeeApplicationData.version_code = COFFEE_VERSION_CODE;

        /* Get application name, just stock */
        cstring_w appname = &(CoffeeApplicationData.application_name[0]);

        /* And then load the usual main() entry point */
        if(android_entry_point)
        {
            int32 status = CoffeeMain(android_entry_point,1,&appname);
            cDebug("Android exit: {0}",status);
        }else{
            cWarning("Failed to load application entry point!");
        }
    }
}
