#include <platforms/android/file.h>

#include <peripherals/enum/helpers.h>
#include <platforms/base/file.h>

#include <android/asset_manager.h>
//#include <coffee/android/android_main.h>

using namespace ::enum_helpers;

namespace platform {
namespace file {
namespace android {

static AAssetManager* m_android_asset_manager = nullptr;

AAssetManager* and_asset_manager(FileFun::file_error& ec)
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

FileFun::FileHandle FileFun::Open(
    const Url& fn, RSCA ac, FileFun::file_error& ec)
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
    {
        fh    = Ancestor::Open(fn, ac, ec);
        fh.fp = nullptr;
    }

    return fh;
}

bool FileFun::Close(FileHandle&& fh, file_error& ec)
{
    if(fh.fp)
        AAsset_close(fh.fp);
    else
        return Ancestor::Close(std::move(fh), ec);
    return true;
}

Bytes FileFun::Read(FileHandle const& fh, szptr size, file_error& ec)
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

bool FileFun::Write(FileHandle const& fh, const Bytes& d, file_error& ec)
{
    if(fh.fp)
    {
        /* AAsset does not support writing to assets. */
        ec = FileError::InvalidAccess;
        return false;
    } else
        return Ancestor::Write(fh, d, ec);
}

szptr FileFun::Size(FileFun::FileHandle const& fh, file_error& ec)
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

szptr FileFun::Size(Url const& fn, file_error& ec)
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

FileFun::FileMapping FileFun::Map(
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

bool FileFun::Unmap(FileFun::FileMapping&& mp, file_error& ec)
{
    if(mp.handle.fd)
    {
        return Close(std::move(mp.handle), ec);
    } else
    {
        Ancestor::FileMapping map = {};
        map.data                  = mp.data;
        map.size                  = mp.size;
        return Ancestor::Unmap(std::move(map), ec);
    }
}

} // namespace android
} // namespace file
} // namespace platform
