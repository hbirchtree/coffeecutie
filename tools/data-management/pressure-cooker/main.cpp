#include <coffee/core/CFiles>
#include <coffee/core/VirtualFS>
#include <coffee/core/coffee.h>
#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/plat/plat_linking.h>
#include <coffee/interfaces/content_pipeline.h>
#include <coffee/core/terminal/terminal_cursor.h>

using namespace Coffee;
using namespace Library;

static Vector<CString> compressFilter = {
    "fbx", "bin", "vert", "frag", "geom", "tesc",
    "tese", "comp", "blend", "blend1", "svg", "kra", "jpg",
    "png"
};

static Vector<CString> ignoreFiler = {
    "kra", "kra~", "blend", "blend1", "zbin", "bin"
};

static Vector<CString> baseDirs = {};

static Vector<CoffeePipeline::FileProcessor*> extProcessors;

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

        for(auto ext : ignoreFiler)
        {
            if(ext == filename.extension())
                return;
        }

        for(auto ext : compressFilter)
        {
            if(ext == filename.extension())
            {
                flag = VirtFS::File_Compressed;
                break;
            }
        }

        files.emplace_back(
                    filename.internUrl.c_str(),
                    Bytes(), flag);
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

void csv_parse(CString const& v, Vector<CString>& out)
{
    auto it = v.find(",");

    if(v.size() > 0 && it == CString::npos)
        out.push_back(v);
    else
        out.push_back(v.substr(0, it));

    while(it != CString::npos)
    {
        auto next = v.find(",", it + 1);

        CString ext;
        if(next == CString::npos)
            ext = v.substr(it + 1,
                           v.size() - it -  1);
        else
            ext = v.substr(it + 1, next - it - 1);

        out.push_back(ext);

        it = next;
    }
}

void load_extension(cstring name)
{
    auto library = FunctionLoader::GetLibrary(
                name, FunctionLoader::GlobalSymbols
                );

    if(!library)
    {
        cWarning("Failed to load library: {0}",
                 FunctionLoader::LinkError());
        return;
    }

    using T = CoffeePipeline::FileProcessor;

    auto constructor = ObjectLoader::GetConstructor<T>(
                library, DefaultConstructorFunction, nullptr);

    if(!constructor.loader)
        return;

    extProcessors.push_back(constructor.loader());
}

void parse_args(ArgumentResult& args)
{
    for(auto arg : args.arguments)
    {
        if(arg.first == "compress_types")
        {
            compressFilter.clear();

            csv_parse(arg.second, compressFilter);
        }else if(arg.first == "extensions")
        {
            Vector<CString> extensions;
            csv_parse(arg.second, extensions);

            for(auto const& ext : extensions)
                load_extension(ext.c_str());
        }else if(arg.first == "ignore")
        {
            csv_parse(arg.second, ignoreFiler);
        }else if(arg.first == "basedir")
        {
            csv_parse(arg.second, baseDirs);
        }
    }
}

void test_vfs(Vector<byte_t>& outputData,
              Vector<VirtFS::VirtDesc> const& descriptors,
              TerminalCursor& cursor)
{
    /* This part is about testing */
    Bytes vfsData = Bytes::CreateFrom(outputData);

    VirtFS::VFS const* vfs = nullptr;
    VirtFS::VFS::OpenVFS(vfsData, &vfs);

    for(auto& desc : descriptors)
    {
        auto file = VirtFS::VFS::GetFile(vfs, desc.filename.c_str());

        if(!file)
            continue;

        auto rsize = file->rsize;
        auto fsize = file->size;

        if(fsize == 0)
            continue;

        scalar percentage = scalar(fsize) / scalar(rsize) * 100.f;

        cursor.print("File squash: {0} : {1}% size ({2}B -> {3}B)",
                     desc.filename, percentage,
                     rsize, fsize);
    }
}

i32 coffee_main(i32, cstring_w*)
{
    Url outputVfs;
    auto rscDir = "."_url;
    Vector<VirtFS::VirtDesc> descriptors;
    Vector<CResources::Resource> resources;
    u64 totalSize = 0;
    Vector<byte_t> outputData;
    TerminalCursor cursor;
    bool showStats = false;

    {
        ArgumentParser parser;

        parser.addPositionalArgument("resource_dir",
                                     "Source resource directory");
        parser.addPositionalArgument("out_vfs",
                                     "Output VirtualFS");

        parser.addArgument("compress_types", "compress-types", "c",
                           "File extensions to perform compression for,"
                           " comma-separated list");

        parser.addArgument("extensions", "extensions", "e",
                           "Comma-separated list of libraries to"
                           " perform extended tasks");

        parser.addArgument("ignore", "ignore-extensions", "i",
                           "Comma-separated list of file extensions"
                           " to ignore");

        parser.addArgument("basedir", "base-dirs", "b",
                           "Comma-separated list of resource base"
                           " directories, used when filtering filenames"
                           " in resources. Can be used to remove all"
                           " absolute file paths. All of them."
                           " Even embedded in models.");

        parser.addSwitch("statistics", "stats", "s",
                         "Show statistics for files afterwards");

        auto args = parser.parseArguments(GetInitArgs());

        if(args.positional.size() != 2)
        {
            cBasicPrint("{0}", parser.helpMessage());
            return 1;
        }

        if(args.switches.find("statistics") != args.switches.end())
            showStats = true;

        parse_args(args);

        outputVfs = MkUrl(args.positional["out_vfs"].c_str(),
                RSCA::SystemFile);

        FileResourcePrefix(args.positional["resource_dir"].c_str());
    }

    /* List files */
    {
        DirFun::DirList content;
        if(!DirFun::Ls(rscDir, content))
        {
            cFatal("Failed to list resource directory");
            return 1;
        }

        std::for_each(content.begin(), content.end(),
                      [&](DirFun::DirItem_t const& subItem)
        {
            recurse_directories({}, subItem, descriptors);
        });
    }

    auto progressFun = [&]()
    {
        totalSize = 0;
        for(auto const& f :  descriptors)
            totalSize += f.data.size;

        cstring ext = "B";

        if(totalSize > 1_GB)
        {
            ext = "GB";
            totalSize /= 1_GB;
        }else if(totalSize > 1_MB)
        {
            ext = "MB";
            totalSize /= 1_MB;
        }else if(totalSize > 1_kB)
        {
            ext = "kB";
            totalSize /= 1_kB;
        }

        CString progress = cStringFormat(
                    "{0} files/{1}{2}: ",
                    descriptors.size(),
                    totalSize, ext
                    );

        return progress;
    };

    cursor.setProgress(progressFun);

    for(auto& desc : descriptors)
    {
        auto url = MkUrl(desc.filename.c_str());
        resources.emplace_back(url);

        desc.data = C_OCAST<Bytes>(resources.back());

        if(desc.data.size == 0)
        {
            cursor.print("Failed to add {0}", url);
            continue;
        }

        auto fsize = FileFun::Size(url);
        cursor.progress("Adding file: {0}, {1} bytes, {2}", url,
                     fsize,
                     (desc.flags == VirtFS::File_Compressed)
                     ? "(compressed)" : "");
    }

    cursor.progress("Post-processing files...");

    for(auto proc : extProcessors)
    {
        proc->receiveAssetPath(GetFileResourcePrefix());
        proc->setBaseDirectories(baseDirs);
        proc->process(descriptors, cursor);
    }

    cursor.progress("Creating filesystem...");

    if(!VirtFS::GenVirtFS(descriptors, &outputData))
    {
        cursor.print("Failed to create VirtFS");
    }
    else
        cursor.complete("Filesystem created!");

    /* Give some statistics on the created filesystem */
    if(showStats)
    {
        test_vfs(outputData, descriptors, cursor);
        cursor.print("Total output size: {0}/{1}MB, {2} files",
                     outputData.size(), outputData.size() / 1_MB,
                     descriptors.size());
    }else
        cursor.print("");

    /* Unmap all files */
    for(auto& rsc : resources)
    {
        FileUnmap(rsc);
    }

    /* At this point, we are writing the VFS to disk */
    {
        CResources::Resource output(outputVfs);
        output = Bytes::CreateFrom(outputData);

        FileCommit(output, false, RSCA::Discard);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN_CUSTOM_ARG(coffee_main)
