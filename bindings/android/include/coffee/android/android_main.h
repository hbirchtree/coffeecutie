#pragma once

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/tdef/stltypes.h>

#include <coffee/core/base/renderer/eventapplication_wrapper.h>

#include <jni.h>
#include <android/sensor.h>
#include <android/asset_manager.h>

struct android_app;

extern struct android_app* coffee_app;

struct AAsset;
struct AAssetManager;

using Coffee::cstring;
using Coffee::CString;
using Coffee::i64;
using Coffee::u64;
using Coffee::scalar;
using Coffee::bigscalar;

struct javavar
{
    JNIEnv* env;
    jclass ass;
    jfieldID field;
};

extern javavar Coffee_JavaGetStaticMember(cstring clss,
                                          cstring var,
                                          cstring type);

template<typename T>
FORCEDINLINE T CJ_GetStatic(cstring classname, cstring varname)
{
    return T();
}

template<>
FORCEDINLINE CString CJ_GetStatic(cstring classname, cstring varname)
{
    auto varobj = Coffee_JavaGetStaticMember(classname, varname,
                                             "Ljava/lang/String;");

    if(!varobj.env)
        return {};

    jstring s = (jstring)varobj.env->GetStaticObjectField(varobj.ass, varobj.field);
    auto chars = varobj.env->GetStringUTFChars(s, 0);

    CString output = chars;

    varobj.env->ReleaseStringUTFChars(s, chars);

    return output;
}

enum FCmdType
{
    Android_QueryNull,

    /* System information queries */
    Android_QueryAPI,
    Android_QueryReleaseName,
    Android_QueryDeviceBoardName,
    Android_QueryDeviceBrand,
    Android_QueryDeviceName,
    Android_QueryDeviceManufacturer,
    Android_QueryDeviceProduct,

    /* Low-level information */
    Android_QueryPlatformABIs,
    Android_QueryBuildType,

    /* Hardware information */
    Android_QueryMaxMemory,

    /* System-level functionality */
    Android_QueryNativeWindow,
    Android_QueryAssetManager,

    /* Filesystem queries */
    Android_QueryDataPath,
    Android_QueryExternalDataPath,
    Android_QueryObbPath,
};

struct AndroidForeignCommand
{
    FCmdType type;

    union {
        u64 scalarU64;
        i64 scalarI64;
        scalar scalarF32;
        bigscalar scalarF64;
        cstring ptr_string;
        void* ptr;
    } data;

    CString store_string;
};
