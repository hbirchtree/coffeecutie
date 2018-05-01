#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/VirtualFS>
#include <coffee/core/coffee.h>
#include <coffee/core/plat/environment/argument_parse.h>
#include <coffee/core/terminal/table-print.h>

using namespace Coffee;

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
        table[1].push_back(to_string(file.rsize));
        table[2].push_back(to_string(file.size));
        table[3].push_back(to_string(perc));
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
    return StrUtil::rpad(src, ' ', len);
}

static void normal_printing(
    VirtFS::vfs_view& vfsView, bool human_readable, u32 columns)
{
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
    VirtFS::vfs_view& view,
    CString           source_file,
    CString           output_file,
    RSCA              write_flags)
{
    auto it =
        std::find_if(view.begin(), view.end(), [&](VirtFS::VFile const& file) {
            return source_file == file.name;
        });

    if(it == view.end())
    {
        cBasicPrint("{0}: file not found", source_file);
        return 1;
    }

    auto data         = C_OCAST<Bytes>(it);
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
        if(!FileCommit(output, false, write_flags))
        {
            cBasicPrint("{0}: failed to write file", output_fname);
            return 1;
        }
    }

    return 0;
}

i32 coffee_main(i32, cstring_w*)
{
    Url targetFile = {};

    ArgumentParser parser;

    parser.addPositionalArgument("vfs-file", "target virtual file system");

    parser.addArgument("infile", "in-file", "g", "Get file from the VFS");
    parser.addArgument("outfile", "out-file", "o", "Output file when using -g");

    parser.addSwitch("pretty", "pretty", "p", "Print in a pretty fashion");
    parser.addSwitch("force", "force", "f", "Forceful behavior, be careful");

    parser.addSwitch("human", nullptr, "h", "Human-readable size");
    parser.addSwitch("human", nullptr, "h", "Human-readable size");
    parser.addSwitch("size", nullptr, "c", "Show compressed size");
    parser.addSwitch("rsize", nullptr, "r", "Show real size");

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

    auto targetResource = targetFile.rsc<Resource>();
    auto targetData     = C_OCAST<Bytes>(targetResource);

    if(targetData.size == 0)
    {
        cBasicPrint("Filesystem not found");
        return 1;
    }

    VirtFS::vfs_view vfsView(targetData);

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

    if(get_file != args.arguments.end())
    {
        auto out_it = args.arguments.find("outfile");

        return extract_file(
            vfsView,
            get_file->second,
            out_it != args.arguments.end() ? out_it->second : "",
            forceful ? RSCA::Discard : RSCA::None);
    }

    if(pretty_printing)
    {
        pretty_print(vfsView);
        return 0;
    }

    normal_printing(vfsView, human_readable, shown_columns);

    return 0;
}

COFFEE_APPLICATION_MAIN_CUSTOM(coffee_main, 0x1 | 0x2)
