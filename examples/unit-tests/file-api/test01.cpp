#include <coffee/core/CUnitTesting>
#include <coffee/core/CFiles>

using namespace Coffee;

const cstring testfile = "fileapi_testfile.txt";

bool filedel_test()
{
    if(CResources::FileFun::Exists(testfile))
    {
        cDebug("Unclean environment! Remove file: {0}",testfile);
        return false;
    }
    CResources::FileFun::Touch(CResources::FileFun::NodeType::File,testfile);

    if(!CResources::FileFun::Exists(testfile))
        return false;

    if(!CResources::FileFun::Rm(testfile))
        return false;

    return !CResources::FileFun::Exists(testfile);
}

const cstring writetest = "writetest.txt";
byte_t write_data[100] = {
    "I'M THE TRASHMAN. I THROW GARBAGE ALL OVER THE RING, AND THEN I START EATING GARBAGE.\n"
};

bool filewrite_test()
{
    CResources::FileFun::Rm(writetest);

    CResources::Resource rsc(writetest);
    rsc.size = sizeof(write_data);
    rsc.data = write_data;
    return CResources::FileCommit(rsc);
}

bool fileread_test()
{
    CResources::Resource rsc(writetest);
    CResources::FilePull(rsc);
    if(sizeof(write_data) != rsc.size)
    {
        CResources::FileFree(rsc);
        return false;
    }
    bool status = MemCmp(write_data,rsc.data,sizeof(write_data));
    cDebug("\nTheirs:\n{1}\nMine:\n{0}",
           StrUtil::hexdump(write_data,sizeof(write_data),10),
           StrUtil::hexdump(rsc.data,rsc.size,10));
    CResources::FileFree(rsc);
    return status;
}

bool filesize_test()
{
    szptr size = CResources::FileFun::Size(writetest);
    szptr target = sizeof(write_data);
    return size==target;
}

const constexpr CoffeeTest::Test _tests[4] = {
    {filedel_test,"File handling","Creating and deleting a file",true},
    {filewrite_test,"File writing","Writing text data to disk"},
    {fileread_test,"File reading","Reading text data from disk"},
    {filesize_test,"File size","Verifying file size on disk"},
};

COFFEE_RUN_TESTS(_tests);