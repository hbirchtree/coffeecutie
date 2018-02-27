#include <coffee/core/CFiles>
#include <coffee/core/VirtualFS>
#include <coffee/core/coffee.h>
#include <coffee/core/CApplication>
#include <coffee/core/CDebug>

using namespace Coffee;

static Vector<CString> compressFilter = {
    "fbx", "bin", "vert", "frag", "geom", "tesc",
    "tese", "comp", "blend", "svg"
};

void recurse_directories(Path const& prepath,
                         DirFun::DirItem_t const& item,
                         Vector<VirtFS::VirtDesc>& files)
{
    switch(item.type)
    {
    case DirFun::Type::File:
    case DirFun::Type::Link:
    {
        auto filename = (prepath + Path(item.name.c_str()));

        u32 flag = 0;

        for(auto ext : compressFilter)
            if(ext == filename.extension())
            {
                flag = VirtFS::File_Compressed;
                break;
            }

        files.push_back({
                            filename.internUrl.c_str(),
                            {},
                            flag
                        });
        break;
    }
    case DirFun::Type::Directory:
    {
        Path path = prepath + item.name.c_str();

        auto url = MkUrl(path.internUrl.c_str());
        DirFun::DirList content;

        DirFun::Ls(MkUrl(path.internUrl.c_str()), content);

        std::for_each(content.begin(), content.end(),
                      [&](DirFun::DirItem_t const& subItem)
        {
            recurse_directories(path, subItem, files);
        });

        break;
    }
    default:
        break;
    }
}

i32 coffee_main(i32, cstring_w*)
{
    ArgumentParser parser;

//    parser.addPositionalArgument("resource_dir",
//                                 "Source resource directory");
    parser.addPositionalArgument("out_vfs",
                                 "Output VirtualFS");

    parser.addArgument("compress_types", "compress-types", "c",
                       "File extensions to perform compression for");

    auto args = parser.parseArguments(C_CCAST<AppArg&>(GetInitArgs()));

    if(args.positional.size() != 1)
    {
        cBasicPrint("{0}", parser.helpMessage());
        return 1;
    }

    for(auto arg : args.arguments)
    {
        if(arg.first == "compress_types")
        {
            compressFilter.clear();

            auto it = arg.second.find(",");

            if(arg.second.size() > 0 && it == CString::npos)
                compressFilter.push_back(arg.second);
            else
                compressFilter.push_back(arg.second.substr(0, it));

            while(it != CString::npos)
            {
                auto next = arg.second.find(",", it + 1);

                CString ext;
                if(next == CString::npos)
                    ext = arg.second.substr(it + 1,
                                            arg.second.size() - it -  1);
                else
                    ext = arg.second.substr(it + 1, next - 1);

                compressFilter.push_back(ext);

                it = next;
            }
        }
    }

    auto rscDir = "."_url;

    Vector<VirtFS::VirtDesc> descriptors;

    DirFun::DirList content;
    if(!DirFun::Ls(rscDir, content))
    {
        cFatal("Failed to list resource directory");
        return 1;
    }

    std::for_each(content.begin(), content.end(),
                  [&](DirFun::DirItem_t const& subItem)
    {
        recurse_directories(rscDir, subItem, descriptors);
    });

    Vector<CResources::Resource> resources;

    u64 totalSize = 0;

    for(auto& desc : descriptors)
    {
        auto url = MkUrl(desc.filename.c_str());
        resources.emplace_back(url);

        desc.data = C_OCAST<Bytes>(resources.back());

        if(desc.data.size == 0)
        {
            cWarning("Failed to add {0}", url);
            continue;
        }

        auto fsize = FileFun::Size(url);
        cDebug("Adding file: {0}, {1} bytes, {2}", url,
               fsize,
               (desc.flags == VirtFS::File_Compressed)
               ? "(compressed)" : "");

        totalSize += fsize;
    }

    cDebug("Total size: {0}B/{1}MB",
           totalSize, totalSize / 1_MB);

    cDebug("Creating filesystem...");

    Vector<byte_t> outputData;
    if(!VirtFS::GenVirtFS(descriptors, &outputData))
    {
        cWarning("Failed to create VirtFS");
    }
    else
        cDebug("Done!");

    Bytes vfsData = Bytes::CreateFrom(outputData);

    VirtFS::VFS const* vfs = nullptr;
    VirtFS::VFS::OpenVFS(vfsData, &vfs);

    for(auto& desc : descriptors)
    {
        auto rsize = 0;
        auto fsize = 0;

        auto file = VirtFS::VFS::GetFile(vfs, desc.filename.c_str());

        if(!file)
            continue;

        rsize = file->rsize;
        fsize = file->size;

        if(fsize == 0)
            continue;

        scalar percentage = scalar(fsize) / scalar(rsize) * 100.f;

        cDebug("File squash: {0} : {1}% size ({2}B -> {3}B)",
               desc.filename, percentage,
               rsize, fsize);
    }

    cDebug("Total output size: {0}/{1}MB",
           outputData.size(), outputData.size() / 1_MB);

    for(auto& rsc : resources)
    {
        FileUnmap(rsc);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
