#include <coffee/core/plat/file/android/file.h>

#include <coffee/core/CDebug>
#include "../file_abstraction.h"


#if defined(COFFEE_USE_SDL2)
#include <android/asset_manager_jni.h>
#include <SDL_system.h>
#else
#include <android/asset_manager.h>
#include <coffee/android/android_main.h>
#endif

namespace Coffee{
namespace CResources{
namespace Android{

static AAssetManager* m_android_asset_manager = nullptr;

#if defined(COFFEE_USE_SDL2)
#pragma error WHAT THE FUK

jobject and_get_app_context(JNIEnv* env, jobject activity)
{
    jmethodID methodApp = env->GetMethodID(env->GetObjectClass(activity),
                                           "getApplication",
                                           "()Landroid/app/Application;");
    if(!methodApp)
        return nullptr;
    jclass contextClass = env->FindClass("android/content/Context");
    if(!contextClass)
        return nullptr;
    jmethodID contextMethod = env->GetMethodID(contextClass,
                                               "getApplicationContext",
                                               "()Landroid/content/Context;");
    if(!contextMethod)
        return nullptr;
    jobject contextObject = env->CallObjectMethod(activity,contextMethod);

    return contextObject;
}

jobject and_get_asset_manager(JNIEnv* env, jobject activity)
{
    jobject appContext = and_get_app_context(env,activity);
    if(!appContext)
    {
        cVerbose(6,"Failed to acquire applicationContext object");
        return nullptr;
    }
    jmethodID getAssets = env->GetMethodID(env->GetObjectClass(appContext),
                                           "getAssets",
                                           "()Landroid/content/res/AssetManager;");
    if(!getAssets)
    {
        cVerbose(6,"Failed to acquire getAssets() method");
        return nullptr;
    }
    jobject assetManager = env->CallObjectMethod(appContext,
                                                 getAssets);
    if(!assetManager)
    {
        cVerbose(6,"Failed to get assetManager object");
        return nullptr;
    }
    jobject globalRef = env->NewGlobalRef(assetManager);
    if(!globalRef)
    {
        cVerbose(6,"Failed to acquire global reference to AssetManager");
        return nullptr;
    }
    return globalRef;
}
#endif

AAssetManager* and_asset_manager()
{
    if(!m_android_asset_manager)
    {
#if defined(COFFEE_USE_SDL2)
        cVerbose(6,"Acquiring JNI environment from SDL");
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
        cVerbose(6,"Acquiring Android activity from SDL");
        jobject manager = and_get_asset_manager(env,(jobject)SDL_AndroidGetActivity());
        if(!manager)
        {
            cVerbose(6,"Failed to acquire assetManager object");
            return nullptr;
        }
        cVerbose(6,"Calling into AAssetManager_fromJava({0},{1})",
                 (c_cptr const&)env,(c_cptr const&)manager);
        m_android_asset_manager = AAssetManager_fromJava(env, manager);
        cVerbose(6,"Acquired AAssetManager* ptr: {0}",
                 (c_cptr const&)m_android_asset_manager);
#else
        AndroidForeignCommand cmd;

        cmd.type = Android_QueryAssetManager;

        CoffeeForeignSignalHandleNA(CoffeeForeign_RequestPlatformData,
                                    &cmd, nullptr,  nullptr);

        m_android_asset_manager = C_FCAST<AAssetManager*>(cmd.data.ptr);
#endif
    }

    if(!m_android_asset_manager)
    {
        cVerbose(6,"Failed to acquire AssetManager* from Java");
    }

    return m_android_asset_manager;
}

CString AndroidFileFun::NativePath(cstring fn)
{
    cVerbose(6,"Input filename: {0}",fn);

    if(!fn)
        return "";

    if(fn[0] == '/')
        return fn;

    if(AssetApi::GetAsset(fn))
    {
        cVerbose(6,"File {0} identified as asset",fn);
        return fn;
    }

    CString prefix;

#if defined(COFFEE_USE_SDL2)
    cVerbose(6,"ExternalStorageState() function pointer: {0}",(uint64)SDL_AndroidGetExternalStorageState);

    if(SDL_AndroidGetExternalStorageState() == 0 ||
            !(SDL_AndroidGetExternalStorageState()&SDL_ANDROID_EXTERNAL_STORAGE_READ))
    {
        /* Failure! */
        cVerbose(6,"Android SDL external storage status failed");
        if(SDL_AndroidGetInternalStoragePath())
            prefix = SDL_AndroidGetInternalStoragePath();
    }else{
        if(SDL_AndroidGetExternalStoragePath())
            prefix = SDL_AndroidGetExternalStoragePath();
    }

    cVerbose(6,"Storage prefix: {0}",prefix);
#else
    AndroidForeignCommand  cmd;

    cmd.type = Android_QueryExternalDataPath;

    CoffeeForeignSignalHandleNA(CoffeeForeign_RequestPlatformData,
                                &cmd, nullptr, nullptr);

    prefix = cmd.store_string;
#endif

    if(prefix.size()==0)
        return CString(fn);

    prefix.append("/");

    cVerbose(6,"Android native path: {0}",prefix+fn);

    return prefix + fn;
}

CString AndroidFileFun::NativePath(cstring fn, ResourceAccess storage)
{
    if(feval(storage,ResourceAccess::TemporaryFile))
        return Env::ConcatPath("/data/local/tmp",fn);
    else
        return NativePath(fn);
}

AndroidFileFun::FileHandle *AndroidFileFun::Open(cstring fn, ResourceAccess ac)
{
    FileHandle* fh = nullptr;
    cstring asset = AssetApi::GetAsset(fn);
    if(asset && FileFun::VerifyAsset(asset))
    {
        AAsset* fp = AAssetManager_open(and_asset_manager(),asset,AASSET_MODE_BUFFER);
        if(!fp)
            return nullptr;
        fh = new FileHandle;
        fh->fp = fp;
    }
    else
        fh = Ancestor::Open(fn,ac);

    return fh;
}

bool AndroidFileFun::Close(FileHandle *fh)
{
    if(fh->fp)
        AAsset_close(fh->fp);
    else
        return Ancestor::Close(fh);
    return true;
}

CByteData AndroidFileFun::Read(FileHandle *fh, uint64 size, bool nterminate)
{
    if(!fh)
        return {};
    if(fh->fp)
    {
        /* In this case, the file exists as an asset */
        CByteData data;
#if (ANDROID_API_LEVEL >= 16) || !defined(ANDROID_API_LEVEL)
        data.size = AAsset_getLength64(fh->fp);
#else
        data.size = AAsset_getLength(fh->fp);
#endif
        /* NOTE: Be aware! You might fuck sh*t up real bad. */
        data.data = (byte_t*)AAsset_getBuffer(fh->fp);

        return data;
    }else
        return Ancestor::Read(fh,size,nterminate);
}

bool AndroidFileFun::Write(FileHandle *fh, const CByteData &d, bool)
{
    if(fh->fp)
    {
        /* AAsset does not support writing to assets. */
        return false;
    }else
        return Ancestor::Write(fh,d,false);
}

szptr AndroidFileFun::Size(AndroidFileFun::FileHandle *fh)
{
    if(fh->fp)
    {
#if (ANDROID_API_LEVEL >= 16) || !defined(ANDROID_API_LEVEL)
        return AAsset_getLength64(fh->fp);
#else
        return AAsset_getLength(fh->fp);
#endif
    }else
        return Ancestor::Size(fh);
}

szptr AndroidFileFun::Size(cstring fn)
{
    cstring check = AssetApi::GetAsset(fn);
    if(check && FileFun::VerifyAsset(check))
    {
        AAsset* ass = AAssetManager_open(and_asset_manager(),check,AASSET_MODE_UNKNOWN);
        if(!ass)
            return 0;
#if (ANDROID_API_LEVEL >= 16) || !defined(ANDROID_API_LEVEL)
        szptr sz = AAsset_getLength64(ass);
#else
        szptr sz = AAsset_getLength(ass);
#endif
        AAsset_close(ass);

        return sz;
    }else
        return Ancestor::Size(fn);
}

AndroidFileFun::FileMapping AndroidFileFun::Map(cstring fn, ResourceAccess acc,
                                                szptr offset, szptr size,
                                                int *)
{
    cstring asset = AssetApi::GetAsset(fn);
    if(asset)
    {
        if(feval(acc,ResourceAccess::WriteOnly))
            return {};
        FileHandle* fh = Open(fn,acc);
        if(!fh)
            return {};
        FileMapping map = {};
        map.handle = fh;
#if (ANDROID_API_LEVEL >= 16) || !defined(ANDROID_API_LEVEL)
        if(AAsset_getLength64(fh->fp)<(offset+size))
#else
        if(AAsset_getLength(fh->fp)<(offset+size))
#endif
        {
            Close(fh);
            return {};
        }
        map.acc = acc;
        map.ptr = &((byte_t*)AAsset_getBuffer(fh->fp))[offset];
        map.size = size;
        return map;
    }else
    {
        auto fm = Ancestor::Map(fn,acc,offset,size,nullptr);

        FileMapping map;
        map.handle = nullptr;
        map.acc = fm.acc;
        map.ptr = fm.ptr;
        map.size = fm.size;
        return map;
    }
}

bool AndroidFileFun::Unmap(AndroidFileFun::FileMapping *mp)
{
    if(mp->handle)
    {
        mp->acc = ResourceAccess::None;
        mp->ptr = nullptr;
        mp->size = 0;
        return Close(mp->handle);
    }else{
        Ancestor::FileMapping map = {};
        map.acc = mp->acc;
        map.ptr = mp->ptr;
        map.size = mp->size;
        bool stat = Ancestor::Unmap(&map);
        mp->acc = map.acc;
        mp->ptr = map.ptr;
        mp->size = map.size;
        return stat;
    }
}

}
}
}
