#include <coffee/core/CUnitTesting>
#include <coffee/core/CIniParser>

using namespace Coffee;

bool readwrite_file()
{
    auto testFile = MkUrl("test.ini",
                          ResourceAccess::SpecifyStorage
                          |ResourceAccess::TemporaryFile);
    auto testFileDupe = MkUrl("test.ini.ini",
                              ResourceAccess::SpecifyStorage
                              |ResourceAccess::TemporaryFile);

    CResources::Resource testfile(testFile);
    CResources::FileMap(testfile);
    Profiler::Profile("Mapping time");

    INI::Document doc2 = INI::Read(testfile);
    Profiler::Profile("Reading time");

    CResources::Resource rsc(testFileDupe);
    INI::Write(doc2,rsc);
    CResources::FileCommit(rsc);
    CResources::FileFree(rsc);
    Profiler::Profile("Write-back");

    CResources::FileUnmap(testfile);
    Profiler::Profile("Unmapping");
    return true;
}

/*!
 * \brief Compound test; both writing a file and creating a document.
 * \return
 */
bool write_file()
{
    Url testfile = MkUrl("testoutfile.ini",
                         ResourceAccess::SpecifyStorage
                         |ResourceAccess::TemporaryFile);

    INI::Document doc;

    INI::Section t1 = doc.newSection();
    INI::Section t2 = doc.newSection();

    doc.insertSection("Test",t1);
    doc.insertSection("Rest",t2);

    INI::Variant v1 = doc.newString("Hello test string");
    INI::Variant v2 = doc.newBool(false);
    INI::Variant v3 = doc.newInteger(100);

    t1->insertValue("hello1",v1);
    t1->insertValue("hello2",v2);
    t1->insertValue("hello3",v3);

    Profiler::Profile("Creating and setting values");

    CResources::Resource rsc(testfile);
    Profiler::Profile("File object");
    INI::Write(doc,rsc);
    Profiler::Profile("Writing object to file");
    CResources::FileCommit(rsc,false,ResourceAccess::Discard);
    Profiler::Profile("Committing file");
    CResources::FileFree(rsc);
    Profiler::Profile("Free'ing file");

    return CResources::FileFun::Exists(testfile);
}

const constexpr CoffeeTest::Test _tests[2] = {
    {readwrite_file, "Reading INI file from disk"},
    {write_file, "Writing INI document to disk"},
};

COFFEE_RUN_TESTS(_tests);
