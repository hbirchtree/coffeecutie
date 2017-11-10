#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>

using namespace Coffee;

const cstring testfile = "fileapi_testfile.txt";

using File = FileFun;

const cstring writetest = "writetest.txt";
byte_t write_data[100] = {
    "I'M THE TRASHMAN. I THROW GARBAGE ALL OVER THE RING, AND THEN I START EATING GARBAGE.\n"
};

bool filewrite_test()
{
    Resource rsc(writetest);
    rsc.size = sizeof(write_data);
    rsc.data = write_data;
    return CResources::FileCommit(rsc,false, ResourceAccess::WriteOnly | ResourceAccess::Discard);
}

bool fileread_test()
{
    CResources::Resource rsc(writetest);
    CResources::FilePull(rsc);
	
    cDebug("\nTheirs:\n{1}\nMine:\n{0}",
        StrUtil::hexdump(write_data, sizeof(write_data), true, 10),
        StrUtil::hexdump(rsc.data, rsc.size, true, 10));

    if(sizeof(write_data) != rsc.size)
    {
        CResources::FileFree(rsc);
        return false;
    }
    bool status = MemCmp(write_data,rsc.data,sizeof(write_data));
    CResources::FileFree(rsc);
    return status;
}

bool filesize_test()
{
    szptr size = CResources::FileFun::Size(writetest);
    szptr target = sizeof(write_data);
	cDebug("{0}?={1}",size,target);

    File::Rm(writetest);

    return size==target;
}

const constexpr CoffeeTest::Test _tests[3] = {
    {filewrite_test,"File writing","Writing text data to disk"},
    {fileread_test,"File reading","Reading text data from disk"},
    {filesize_test,"File size","Verifying file size on disk"},
};

COFFEE_RUN_TESTS(_tests);
