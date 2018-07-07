#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>
#include <coffee/core/types/cdef/memsafe.h>

using namespace Coffee;

const constexpr sbyte_t probe_text[25] = "I'M THE TRASHMAN! Hello.";

bool resource_exist_test()
{
    {
        Resource rsc("the-one-that-works",
                     ResourceAccess::SpecifyStorage|ResourceAccess::AssetFile);
        /* Tests limitations in implementation for names */
        if(!FileExists(rsc))
            return false;
    }

    {
        Resource rsc("filled-dir/subdir/we-need-to-go-deeper/odd-file-with-long-extension.txt.zip.exe",
                     ResourceAccess::SpecifyStorage|ResourceAccess::AssetFile);
        /* Tests limitations in implementation for names */
        if(!FileExists(rsc))
            return false;
    }

    {
        Resource rsc("filled-dir/subdir/we-need-to-go-deeper/Spaced filename.txt",
                     ResourceAccess::SpecifyStorage|ResourceAccess::AssetFile);
        /* Tests errors in scripts regarding spaces */
        if(!FileExists(rsc))
            return false;
    }

    return true;
}

const constexpr cstring filename_odd = "Oddball: æøå, カケ";

bool resource_exists_odd_characters_test()
{
    Resource rsc(filename_odd,
                 ResourceAccess::SpecifyStorage|ResourceAccess::AssetFile);
    /* Tests what happens to unexpected characters. We want these to work. */
    /* If existence passes, we assume the rest works too for these cases. */
	/* It is known that it does not work with Win32's resource system. */
    return FileExists(rsc);
}

const constexpr cstring filename_testable = "filled-dir/subdir/we-need-to-go-deeper/odd-file-with-long-extension.txt.zip.exe";

bool resource_read_test()
{
    Resource rsc(filename_testable,
                 ResourceAccess::SpecifyStorage
                 |ResourceAccess::AssetFile);

    if(!FilePull(rsc))
        return false;

    Bytes probeData = Bytes::From(probe_text, sizeof(probe_text));

    if(MemCmp(probeData, C_OCAST<Bytes>(rsc)))
    {
        cDebug("Sizes: {0} ?= {1}", sizeof(probe_text) - 1,rsc.size);
        cDebug("Data:\n"
               "Internal: {0}\n"
               "Resource: {1}\n",
               StrUtil::hexdump(probe_text, sizeof(probe_text) - 1),
               StrUtil::hexdump(rsc.data,rsc.size));
        return false;
    }

    return true;
}

bool resource_map_test()
{
    Resource rsc(filename_testable,
                 ResourceAccess::SpecifyStorage
                 |ResourceAccess::AssetFile);

    if(!FileMap(rsc,ResourceAccess::ReadOnly))
        return false;

    if(!FileUnmap(rsc))
        return false;

    return true;
}

bool resource_write_test()
{
    Resource rsc(filename_testable,
                 ResourceAccess::SpecifyStorage
                 |ResourceAccess::AssetFile);

    if(!FileMap(rsc,ResourceAccess::ReadWrite))
        return false;

    if(!FileUnmap(rsc))
        return false;

    return true;
}

const constexpr CoffeeTest::Test _tests[5] = {
    {resource_exist_test,
     "Resource existence",
     "Test whether the resources are there"},
    {resource_read_test,
     "Resource reading",
     "Opening and reading a resource, uses an odd filename"},

    /* The ones below only test feature sets */

    {resource_exists_odd_characters_test,
     "Gettin weird with filenames",
     "Test what happens with multibyte characters",true},

    {resource_map_test,
     "Resource mapping",
     "If the resources are mappable",true},
    {resource_write_test,
     "Resource writing",
     "If the resources are writable",true},
};

COFFEE_RUN_TESTS(_tests);
