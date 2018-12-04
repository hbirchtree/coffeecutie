#include <coffee/core/CApplication>
#include <coffee/core/CArgParser>
#include <coffee/core/CDynamicLink>
#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/VirtualFS>
#include <coffee/core/internal_state.h>
#include <coffee/core/resource_prefix.h>
#include <coffee/interfaces/content_pipeline.h>
#include <peripherals/stl/string_casting.h>
#include <platforms/sysinfo.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

#include <coffee/core/terminal/cursor.h>

using namespace Coffee;
using namespace platform::url::constructors;

static Vector<CString> compressFilter = {"fbx",
                                         "bin",
                                         "vert",
                                         "frag",
                                         "geom",
                                         "tesc",
                                         "tese",
                                         "comp",
                                         "blend",
                                         "blend1",
                                         "svg",
                                         "kra",
                                         "jpg",
                                         "png"};

static Vector<CString> ignoreFiler = {
    "kra", "kra~", "blend", "blend1", "zbin", "bin"};

static Vector<CString> baseDirs = {};

static Vector<CoffeePipeline::FileProcessor*> extProcessors;

void recurse_directories(
    Path const&               prepath,
    DirFun::DirItem_t const&  item,
    Vector<VirtFS::VirtDesc>& files)
{
    file_error ec;

    switch(item.type)
    {
    case FileType::File:
    case FileType::Link:
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

        files.emplace_back(filename.internUrl.c_str(), Bytes(), flag);
        break;
    }
    case FileType::Directory:
    {
        Path path = prepath + item.name.c_str();

        auto            url = MkUrl(path.internUrl.c_str());
        DirFun::DirList content;

        DirFun::Ls(MkUrl(path.internUrl.c_str()), content, ec);

        std::for_each(
            content.begin(),
            content.end(),
            [&](DirFun::DirItem_t const& subItem) {
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
            ext = v.substr(it + 1, v.size() - it - 1);
        else
            ext = v.substr(it + 1, next - it - 1);

        out.push_back(ext);

        it = next;
    }
}

void load_extension(cstring name)
{
    load::Function::error_type ec;

    auto library =
        load::Function::GetLibrary(name, ec, load::Function::NoFlags);

    if(!library)
    {
        cWarning("Failed to load library: {0}", ec.message());
        return;
    }

    using T = CoffeePipeline::FileProcessor;

    auto constructor = load::Object::GetConstructor<T>(
        library, load::DefaultConstructorFunction, ec);

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
        } else if(arg.first == "extensions")
        {
            Vector<CString> extensions;
            csv_parse(arg.second, extensions);

            for(auto const& ext : extensions)
                load_extension(ext.c_str());
        } else if(arg.first == "ignore")
        {
            csv_parse(arg.second, ignoreFiler);
        } else if(arg.first == "basedir")
        {
            csv_parse(arg.second, baseDirs);
        } else if(arg.first == "deep_profile")
        {
            State::GetProfilerStore()->flags.deep_enabled = true;
        }
    }
}

void test_vfs(
    Vector<byte_t>&                 outputData,
    Vector<VirtFS::VirtDesc> const& descriptors,
    TerminalCursor&                 cursor)
{
    /* This part is about testing */
    Bytes vfsData = Bytes::CreateFrom(outputData);

    VirtFS::vfs_error_code ec;

    VirtFS::VFS const* vfs = nullptr;
    VirtFS::VFS::OpenVFS(vfsData, &vfs, ec);

    for(auto& desc : descriptors)
    {
        auto file = VirtFS::VFS::GetFile(vfs, desc.filename.c_str(), ec);

        if(!file)
            continue;

        auto rsize = file->rsize;
        auto fsize = file->size;

        if(fsize == 0)
            continue;

        scalar percentage = scalar(fsize) / scalar(rsize) * 100.f;

        cursor.print(
            "File squash: {0} : {1}% size ({2}B -> {3}B)",
            desc.filename,
            percentage,
            rsize,
            fsize);
    }
}

i32 coffee_main(i32, cstring_w*)
{
    /* Very important:
     * We unset environment variables that may affect the input to the program.
     */

    /* AppImage, Snappy and Flatpak storage locations */
    Env::UnsetVar("APPIMAGE_DATA_DIR");
    Env::UnsetVar("SNAP");

    /* End of messing with the environment */

    Url                      outputVfs;
    Path                     cacheDir;
    auto                     rscDir = "."_url;
    Vector<VirtFS::VirtDesc> descriptors;
    Vector<Resource>         resources;
    u64                      totalSize = 0;
    Vector<byte_t>           outputData;
    TerminalCursor           cursor;
    bool                     showStats = false;
    file_error               ec;

    u32 globalNumWorkers = C_FCAST<u32>(SysInfo::ThreadCount());

    {
        ArgumentParser parser;

#if MODE_DEBUG
        parser.addSwitch(
            "deep_profile", "deep-profile", "d", "Enable deep profiling");
#endif

        parser.addPositionalArgument(
            "resource_dir", "Source resource directory");
        parser.addPositionalArgument("out_vfs", "Output VirtualFS");

        parser.addArgument(
            "compress_types",
            "compress-types",
            "c",
            "File extensions to perform compression for,"
            " comma-separated list");

        parser.addArgument(
            "extensions",
            "extensions",
            "e",
            "Comma-separated list of libraries to"
            " perform extended tasks");

        parser.addArgument(
            "ignore",
            "ignore-extensions",
            "i",
            "Comma-separated list of file extensions"
            " to ignore");

        parser.addArgument(
            "basedir",
            "base-dirs",
            "b",
            "Comma-separated list of resource base"
            " directories, used when filtering filenames"
            " in resources. Can be used to remove all"
            " absolute file paths. All of them."
            " Even embedded in models.");

        parser.addArgument(
            "workers",
            "jobs",
            "j",
            "Max number of workers, defaults to number of cores on the system");

        parser.addArgument(
            "cachedir",
            "cache",
            "m",
            "Caching directory for optimizing"
            " cooking time");

        parser.addSwitch(
            "statistics", "stats", "s", "Show statistics for files afterwards");

        auto args = parser.parseArguments(GetInitArgs());

        if(args.positional.size() != 2)
        {
            cBasicPrint("{0}", parser.helpMessage());
            return 1;
        }

        if(args.switches.find("statistics") != args.switches.end())
            showStats = true;
        if(args.arguments.find("workers") != args.arguments.end())
            globalNumWorkers =
                cast_string<u32>(args.arguments.find("workers")->second);

        parse_args(args);

        outputVfs = MkUrl(args.positional["out_vfs"].c_str(), RSCA::SystemFile);

        cacheDir = Path(outputVfs).dirname() + "vfscache";

        if(args.arguments.find("cachedir") != args.arguments.end())
            cacheDir = Path(args.arguments.find("cachedir")->second);

        file::ResourcePrefix(args.positional["resource_dir"].c_str());
    }

    /* List files */
    {
        ProfContext _("Collecting files");

        DirFun::DirList content;
        if(!DirFun::Ls(rscDir, content, ec))
        {
            cFatal("{0}:0: Failed to list resource directory", *rscDir);
            return 1;
        }

        std::for_each(
            content.begin(),
            content.end(),
            [&](DirFun::DirItem_t const& subItem) {
                recurse_directories({}, subItem, descriptors);
            });
    }

    auto progressFun = [&]() {
        totalSize = 0;
        for(auto const& f : descriptors)
            totalSize += f.data.size;

        cstring ext = "B";

        if(totalSize > 1_GB)
        {
            ext = "GB";
            totalSize /= 1_GB;
        } else if(totalSize > 1_MB)
        {
            ext = "MB";
            totalSize /= 1_MB;
        } else if(totalSize > 1_kB)
        {
            ext = "kB";
            totalSize /= 1_kB;
        }

        CString progress = cStringFormat(
            "{0} files/{1}{2}: ", descriptors.size(), totalSize, ext);

        return progress;
    };

    cursor.setProgress(progressFun);

    Profiler::PushContext("Filtering files");
    for(auto& desc : descriptors)
    {
        auto url = MkUrl(desc.filename.c_str());
        resources.emplace_back(url);

        desc.data = C_OCAST<Bytes>(resources.back());

        if(desc.data.size == 0)
        {
            cursor.print(
                "{0}:0: File of size 0 cannot be added",
                (Path(*rscDir) + url.internUrl).canonical().internUrl);
            continue;
        }

        auto fsize = FileFun::Size(url, ec);
        cursor.progress(
            "Adding file: {0}, {1} bytes, {2}",
            url,
            fsize,
            (desc.flags == VirtFS::File_Compressed) ? "(compressed)" : "");
    }

    auto removeIt = std::remove_if(
        descriptors.begin(),
        descriptors.end(),
        [](VirtFS::VirtDesc const& desc) { return desc.data.size == 0; });

    descriptors.erase(removeIt, descriptors.end());
    Profiler::PopContext();

    cursor.progress("Post-processing files...");

    for(auto proc : extProcessors)
    {
        ProfContext _("Running extension");
        proc->setCacheBaseDirectory(cacheDir);

        proc->setInternalState(
            State::GetInternalState(), State::GetInternalThreadState());
        proc->setBaseDirectories(baseDirs);
        proc->numWorkers = globalNumWorkers;

        proc->process(descriptors, cursor);
    }

    cursor.progress("Creating filesystem...");

    VirtFS::vfs_error_code virt_ec;

    if(!VirtFS::GenVirtFS(descriptors, &outputData, virt_ec))
    {
        cursor.print(
            "{0}:0: Failed to create VirtFS: {1}",
            outputVfs.internUrl,
            virt_ec.message());
    } else
        cursor.complete(
            "Filesystem created! ({0}MB)", outputData.size() / 1_MB);

    /* Give some statistics on the created filesystem */
    if(showStats)
    {
        test_vfs(outputData, descriptors, cursor);
        cursor.print(
            "Total output size: {0}/{1}MB, {2} files",
            outputData.size(),
            outputData.size() / 1_MB,
            descriptors.size());
    } else
        cursor.print("");

    /* Unmap all files */
    Profiler::PushContext("Unmapping resources");
    for(auto& rsc : resources)
    {
        FileUnmap(rsc);
    }
    Profiler::PopContext();

    /* At this point, we are writing the VFS to disk */
    {
        ProfContext _("Writing to disk");

        Resource output(outputVfs);

        FileOpenMap(
            output, outputData.size(), RSCA::ReadWrite | RSCA::Persistent);

        Bytes outputView = output;
        MemCpy(outputData, outputView);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN_CUSTOM_ARG(coffee_main)
