#include <coffee/core/CApplication>
#include <coffee/core/plat/environment/argument_parse.h>
#include <coffee/core/coffee.h>
#include <coffee/core/CFiles>
#include <coffee/core/VirtualFS>
#include <coffee/core/terminal/table-print.h>
#include <coffee/core/CDebug>

using namespace Coffee;

i32 coffee_main(i32, cstring_w*)
{
    Url targetFile = {};

    ArgumentParser parser;

    parser.addPositionalArgument("vfs-file", "target virtual file system");

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
    auto targetData = C_OCAST<Bytes>(targetResource);

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

    Table::Column tableCols = {
        "Name",
        "Size (B)",
        "On-disk size (B)",
        "Compression ratio (%)"
    };
    Table::Table table({});
    table.resize(4);

    for(auto const& file : vfsView)
    {
        table[0].push_back(file.name);
        table[1].push_back(to_string(file.rsize));
        table[2].push_back(to_string(file.size));
        table[3].push_back(to_string(
                               scalar(file.size) * 100.f
                               / scalar(file.rsize)));
    }

    cBasicPrint("{0}", Table::GenTable(table, tableCols));

    return 0;
}

COFFEE_APPLICATION_MAIN_CUSTOM_ARG(coffee_main)
