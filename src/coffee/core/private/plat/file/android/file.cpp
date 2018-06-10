#include <coffee/core/plat/file/android/file.h>

#include "../file_abstraction.h"
#include <coffee/core/CDebug>

#if defined(COFFEE_USE_SDL2)
#include <SDL_system.h>
#include <android/asset_manager_jni.h>
#else
#include <android/asset_manager.h>
#include <coffee/android/android_main.h>
#endif

namespace Coffee {
namespace CResources {
namespace Android {

static AAssetManager* m_android_asset_manager = nullptr;

#if defined(COFFEE_USE_SDL2)
#pragma error WHAT THE FUK

jobject and_get_app_context(JNIEnv* env, jobject activity)
{
    jmethodID methodApp = env->GetMethodID(
        env->GetObjectClass(activity),
        "getApplication",
        "()Landroid/app/Application;");
    if(!methodApp)
        return nullptr;
    jclass contextClass = env->FindClass("android/content/Context");
    if(!contextClass)
        return nullptr;
    jmethodID contextMethod = env->GetMethodID(
        contextClass, "getApplicationContext", "()Landroid/content/Context;");
    if(!contextMethod)
        return nullptr;
    jobject contextObject = env->CallObjectMethod(activity, contextMethod);

    return contextObject;
}

jobject and_get_asset_manager(JNIEnv* env, jobject activity)
{
    jobject appContext = and_get_app_context(env, activity);
    if(!appContext)
    {
        cVerbose(6, "Failed to acquire applicationContext object");
        return nullptr;
    }
    jmethodID getAssets = env->GetMethodID(
        env->GetObjectClass(appContext),
        "getAssets",
        "()Landroid/content/res/AssetManager;");
    if(!getAssets)
    {
        cVerbose(6, "Failed to acquire getAssets() method");
        return nullptr;
    }
    jobject assetManager = env->CallObjectMethod(appContext, getAssets);
    if(!assetManager)
    {
        cVerbose(6, "Failed to get assetManager object");
        return nullptr;
    }
    jobject globalRef = env->NewGlobalRef(assetManager);
    if(!globalRef)
    {
        cVerbose(6, "Failed to acquire global reference to AssetManager");
        return nullptr;
    }
    return globalRef;
}
#endif

AAssetManager* and_asset_manager(file_error& ec)
{
    if(!m_android_asset_manager)
    {
        AndroidForeignCommand cmd;

        cmd.type = Android_QueryAssetManager;

        CoffeeForeignSignalHandleNA(
            CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

        m_android_asset_manager = C_FCAST<AAssetManager*>(cmd.data.ptr);
    }

    if(!m_android_asset_manager)
    {
        ec = FileError::SystemError;
        ec = CString("Failed to retrieve AAssetManager*");
    }

    return m_android_asset_manager;
}

AndroidFileFun::FileHandle AndroidFileFun::Open(
    const Url& fn, RSCA ac, file_error& ec)
{
    auto       url = *fn;
    FileHandle fh  = {};

    if(feval(fn.flags & RSCA::AssetFile))
    {
        AAsset* fp = AAssetManager_open(
            and_asset_manager(ec), url.c_str(), AASSET_MODE_BUFFER);

        if(!fp)
            return {};

        fh.fp = fp;
    } else
        fh = Ancestor::Open(fn, ac, ec);

    return fh;
}

bool AndroidFileFun::Close(FileHandle&& fh, file_error& ec)
{
    if(fh.fp)
        AAsset_close(fh.fp);
    else
        return Ancestor::Close(std::move(fh), ec);
    return true;
}

Bytes AndroidFileFun::Read(FileHandle const& fh, szptr size, file_error& ec)
{
    if(fh.fp)
    {
        /* In this case, the file exists as an asset */
        Bytes data;
#if(ANDROID_API_LEVEL >= 16) || !defined(ANDROID_API_LEVEL)
        data.size = AAsset_getLength64(fh.fp);
#else
        data.size = AAsset_getLength(fh.fp);
#endif
        /* NOTE: Be aware! You might fuck sh*t up real bad. */
        data.data = C_RCAST<byte_t*>(C_CCAST<void*>(AAsset_getBuffer(fh.fp)));

        return data;
    } else
        return Ancestor::Read(fh, size, ec);
}

bool AndroidFileFun::Write(FileHandle const& fh, const Bytes& d, file_error& ec)
{
    if(fh.fp)
    {
        /* AAsset does not support writing to assets. */
        ec = FileError::InvalidAccess;
        return false;
    } else
        return Ancestor::Write(fh, d, ec);
}

szptr AndroidFileFun::Size(AndroidFileFun::FileHandle const& fh, file_error& ec)
{
    if(fh.fp)
    {
#if(ANDROID_API_LEVEL >= 16) || !defined(ANDROID_API_LEVEL)
        return AAsset_getLength64(fh.fp);
#else
        return AAsset_getLength(fh.fp);
#endif
    } else
        return Ancestor::Size(fh, ec);
}

szptr AndroidFileFun::Size(Url const& fn, file_error& ec)
{
    auto url = *fn;
    if(feval(fn.flags & RSCA::AssetFile))
    {
        AAsset* ass = AAssetManager_open(
            and_asset_manager(ec), url.c_str(), AASSET_MODE_UNKNOWN);
        if(!ass)
            return 0;
#if(ANDROID_API_LEVEL >= 16) || !defined(ANDROID_API_LEVEL)
        szptr sz = AAsset_getLength64(ass);
#else
        szptr sz = AAsset_getLength(ass);
#endif
        AAsset_close(ass);

        return sz;
    } else
        return Ancestor::Size(fn, ec);
}

AndroidFileFun::FileMapping AndroidFileFun::Map(
    const Url& fn, RSCA acc, szptr offset, szptr size, file_error& ec)
{
    auto url = *fn;
    if(feval(fn.flags & RSCA::AssetFile))
    {
        if(feval(acc, RSCA::WriteOnly))
            return {};
        FileHandle fh = Open(fn, acc, ec);
        if(!fh.fp)
            return {};
#if(ANDROID_API_LEVEL >= 16) || !defined(ANDROID_API_LEVEL)
        if(AAsset_getLength64(fh.fp) < (offset + size))
#else
        if(AAsset_getLength(fh.fp) < (offset + size))
#endif
        {
            Close(std::move(fh), ec);
            return {};
        }

        FileMapping map = {};
        map.handle      = std::move(fh);
        map.data        = &(
            C_RCAST<byte_t*>(C_CCAST<void*>(AAsset_getBuffer(fh.fp))))[offset];
        map.size = size;
        map.assignAccess(acc);

        fh = {};

        return map;
    } else
    {
        auto fm = Ancestor::Map(fn, acc, offset, size, ec);

        FileMapping map;
        map.data = fm.data;
        map.size = fm.size;
        return map;
    }
}

bool AndroidFileFun::Unmap(AndroidFileFun::FileMapping&& mp, file_error& ec)
{
    if(mp.handle.fd)
    {
        return Close(std::move(mp.handle), ec);
    } else
    {
        Ancestor::FileMapping map = {};
        map.data = mp.data;
        map.size = mp.size;
        return Ancestor::Unmap(std::move(map), ec);
    }
}

} // namespace Android
} // namespace CResources
} // namespace Coffee
