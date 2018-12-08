#include <coffee/core/CApplication>
#include <coffee/core/CArgParser>
#include <coffee/core/CFiles>
#include <coffee/core/CMath>
#include <coffee/core/CProfiling>
#include <coffee/core/VirtualFS>
#include <coffee/core/coffee.h>
#include <peripherals/stl/string_casting.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/terminal/table.h>

#include <coffee/core/CDebug>

using namespace Coffee;
using namespace ::platform;

enum ColumnsShown
{
    Show_RSize = 0x1,
    Show_Size  = 0x2,
};

static void pretty_print(VirtFS::vfs_view& vfsView)
{
    Table::Column tableCols = {
        "Name", "Size (B)", "On-disk size (B)", "Compression ratio (%)"};
    Table::Table table({});
    table.resize(4);

    for(auto const& file : vfsView)
    {
        auto perc = u32(scalar(file.size) * 100.f / scalar(file.rsize));

        table[0].push_back(file.name);
        table[1].push_back(str::convert::to_string(file.rsize));
        table[2].push_back(str::convert::to_string(file.size));
        table[3].push_back(str::convert::to_string(perc));
    }

    cOutputPrintNoNL("{0}", Table::GenTable(table, tableCols));
}

static CString humanize_size(szptr size)
{
    if(size > 1_GB)
        return cast_pod(size / 1_GB) + "G";
    else if(size > 1_MB)
        return cast_pod(size / 1_MB) + "M";
    else if(size > 1_kB)
        return cast_pod(size / 1_kB) + "k";
    else
        return cast_pod(size / 1_GB) + "B";
}

static CString srpad(CString src, szptr len)
{
    return str::pad::right(src, ' ', len);
}

static void normal_printing(
    VirtFS::vfs_view& vfsView, bool human_readable, u32 columns)
{
    ProfContext _("Listing all files");

    szptr rsizePadLength = 0;
    szptr sizePadLength  = 0;

    auto print_fmt = "{0} {1} {2}";

    switch(columns)
    {
    case Show_RSize:
        print_fmt = "{0} {2}";
        break;
    case Show_Size:
        print_fmt = "{1} {2}";
        break;
    default:
        break;
    }

    for(auto const& file : vfsView)
    {
        if(human_readable)
        {
            rsizePadLength =
                CMath::max(humanize_size(file.rsize).size(), rsizePadLength);
            sizePadLength =
                CMath::max(humanize_size(file.size).size(), sizePadLength);
        } else
        {
            rsizePadLength =
                CMath::max(cast_pod(file.rsize).size(), rsizePadLength);
            sizePadLength =
                CMath::max(cast_pod(file.size).size(), sizePadLength);
        }
    }

    for(auto const& file : vfsView)
    {
        if(human_readable)
            cOutputPrint(
                print_fmt,
                srpad(humanize_size(file.rsize), rsizePadLength),
                srpad(humanize_size(file.size), sizePadLength),
                file.name);
        else
            cOutputPrint(
                print_fmt,
                srpad(cast_pod(file.rsize), rsizePadLength),
                srpad(cast_pod(file.size), sizePadLength),
                file.name);
    }
}

static i32 extract_file(
    VirtFS::VirtualFS const* vfs,
    CString                  source_file,
    CString                  output_file,
    RSCA                     write_flags)
{
    using namespace ::enum_helpers;

    ProfContext _("Searching for file");

    Profiler::PushContext("Search");
    VirtFS::Resource resource(vfs, MkUrl(source_file));
    Profiler::PopContext();

    VirtFS::vfs_error_code ec;
    VirtFS::VFS::SearchFile(vfs, source_file.c_str(), ec);

    auto data         = C_OCAST<Bytes>(resource);
    auto output_fname = output_file;

    if(output_file == "-")
        fwrite(data.data, data.size, 1, stdout);
    else
    {
        if(output_file.size() == 0)
            output_fname = source_file;

        Resource output(MkUrl(output_fname, RSCA::SystemFile));

        if(FileExists(output) && !feval(write_flags & RSCA::Discard))
        {
            cBasicPrint("{0}: file exists", output_fname);
            return 1;
        }

        output = data;
        if(!FileCommit(output, write_flags | RSCA::NewFile | RSCA::WriteOnly))
        {
            cBasicPrint("{0}: failed to write file", output_fname);
            return 1;
        }
    }

    return 0;
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

    Url targetFile = {};

    Profiler::PushContext("Parsing arguments");

    ArgumentParser parser;

    parser.addPositionalArgument("vfs-file", "target virtual file system");

    parser.addArgument("infile", "in-file", "g", "Get file from the VFS");
    parser.addArgument("outfile", "out-file", "o", "Output file when using -g");

    parser.addSwitch("pretty", "pretty", "p", "Print in a pretty fashion");
    parser.addSwitch("force", "force", "f", "Forceful behavior, be careful");

    parser.addSwitch("human", nullptr, "h", "Human-readable size");
    parser.addSwitch("size", nullptr, "c", "Show compressed size");
    parser.addSwitch("rsize", nullptr, "r", "Show real size");

#if MODE_DEBUG
    parser.addSwitch(
        "deep_profile", "deep-profile", nullptr, "Enable deep profiling");
#endif

    auto args = parser.parseArguments(GetInitArgs());

    for(auto pos : args.positional)
    {
        if(pos.first == "vfs-file")
            targetFile = MkUrl(pos.second.c_str(), RSCA::SystemFile);
    }

    if(targetFile.category == Url::Undefined)
    {
        cBasicPrint("{0}", parser.helpMessage());
        return 1;
    }

    if(args.switches.find("deep_profile")->second > 0)
        State::GetProfilerStore()->flags.deep_enabled = true;

    auto targetResource = targetFile.rsc<Resource>();
    auto targetData     = C_OCAST<Bytes>(targetResource);

    if(targetData.size == 0)
    {
        cBasicPrint("Filesystem not found");
        return 1;
    }

    VirtFS::vfs_view vfsView(targetData);
    auto             vfsData = mem_chunk<const VirtFS::VirtualFS>(targetData);

    if(vfsView.begin() == vfsView.end())
    {
        cBasicPrint("Filesystem is empty");
        return 1;
    }

    u32  shown_columns   = 0;
    bool forceful        = args.switches.find("force") != args.switches.end();
    auto get_file        = args.arguments.find("infile");
    bool human_readable  = args.switches.find("human") != args.switches.end();
    bool pretty_printing = args.switches.find("pretty") != args.switches.end();

    if(args.switches.find("rsize") != args.switches.end())
        shown_columns |= Show_RSize;
    else if(args.switches.find("size") != args.switches.end())
        shown_columns |= Show_Size;

    Profiler::PopContext();

    if(get_file != args.arguments.end())
    {
        auto out_it = args.arguments.find("outfile");

        return extract_file(
            vfsData.data,
            get_file->second,
            out_it != args.arguments.end() ? out_it->second : "",
            forceful ? RSCA::Discard : RSCA::None);
    }

    if(pretty_printing)
    {
        pretty_print(vfsView);
        return 0;
    }

    VirtFS::vfs_error_code ec;

    auto node = VirtFS::VFS::SearchFile(vfsData.data, "blendcache_Shine/4", ec);

    Queue<decltype(node)> outNodes;
    outNodes.push(node);

    while(!outNodes.empty())
    {
        auto node = outNodes.front();
        outNodes.pop();

        if(node.node->is_leaf())
        {
            cDebug("File: {0}", node.file(vfsData.data)->name);
            continue;
        }

        if(node.left().valid())
            outNodes.push(node.left());
        if(node.right().valid())
            outNodes.push(node.right());
    }

    normal_printing(vfsView, human_readable, shown_columns);

    return 0;
}

COFFEE_APPLICATION_MAIN_CUSTOM(coffee_main, 0x1 | 0x2)
